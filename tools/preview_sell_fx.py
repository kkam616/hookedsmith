"""판매 연출 시안 (게임에는 아직 적용하지 않는다): 화면 움짤(GIF)과 효과음(wav)을 만든다.

단계는 강화 성공 연출과 같은 결: +1~5 / +6~16 / +17~28 (+29는 판매 불가).
출력: build/sell_fx_preview/sell_1.gif ~ sell_3.gif, sell_1.wav ~ sell_3.wav, sell_all.wav
수동 실행: python -B tools/preview_sell_fx.py
"""

import math
import os
import random
import sys

from PIL import Image, ImageDraw, ImageFont

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import make_sfx as sfx

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
OUT = os.path.join(ROOT, 'build', 'sell_fx_preview')
SCALE = 3
W, H = 240, 160

GOLD = (248, 208, 56)
GOLD_DARK = (176, 120, 24)
GOLD_LIGHT = (255, 248, 184)
OUTLINE = (72, 40, 8)


# ── 화면 틀: 대장간 배경 + 모루 + 검 ─────────────────────────────────────────
def load_rgba(path):
    image = Image.open(path).convert('P')
    rgba = image.convert('RGBA')
    alpha = Image.frombytes('L', image.size, bytes(0 if v == 0 else 255 for v in image.tobytes()))
    rgba.putalpha(alpha)
    return rgba


def base_scene():
    scene = Image.open(os.path.join(ROOT, 'graphics', 'forge_bg.bmp')).convert('RGBA').crop((0, 0, W, H))
    anvil = load_rgba(os.path.join(ROOT, 'graphics', 'anvil.bmp'))
    scene.alpha_composite(anvil, (W // 2 - 32, H // 2 + 44 - 32))
    return scene


def sword_image(level):
    sheet = load_rgba(os.path.join(ROOT, 'graphics', 'sword_%02d.bmp' % level))
    return sheet.crop((0, 0, 32, 64))


SWORD_POS = (W // 2 - 16, H // 2 - 8 - 32)


# ── 동전 ─────────────────────────────────────────────────────────────────
def coin_image(phase):
    widths = [7, 5, 2, 5]
    w = widths[phase % 4]
    image = Image.new('RGBA', (7, 7), (0, 0, 0, 0))
    draw = ImageDraw.Draw(image)
    x0 = (7 - w) // 2
    draw.ellipse((x0, 0, x0 + w - 1, 6), fill=OUTLINE)
    if w > 2:
        draw.ellipse((x0 + 1, 1, x0 + w - 2, 5), fill=GOLD)
        draw.point((x0 + 1 + (1 if w > 4 else 0), 2), fill=GOLD_LIGHT)
    else:
        draw.line((x0, 1, x0, 5), fill=GOLD_DARK)
    return image


class Coin:
    def __init__(self, x, y, vx, vy, floor, delay=0, bounce=0.45):
        self.x, self.y, self.vx, self.vy = x, y, vx, vy
        self.floor = floor
        self.delay = delay
        self.bounce = bounce
        self.age = 0
        self.settled = 0

    def step(self):
        if self.delay > 0:
            self.delay -= 1
            return
        self.age += 1
        self.vy += 0.28
        self.x += self.vx
        self.y += self.vy
        if self.y >= self.floor and self.vy > 0:
            self.y = self.floor
            self.vy = -self.vy * self.bounce
            self.vx *= 0.6
            if abs(self.vy) < 0.8:
                self.vy = 0
                self.settled += 1

    def visible(self):
        return self.delay <= 0 and self.settled < 14


def draw_coins(scene, coins):
    for coin in coins:
        if coin.visible():
            if coin.settled > 8 and coin.settled % 2:
                continue        # 사라지기 전 깜빡임
            scene.alpha_composite(coin_image(coin.age // 2), (int(coin.x) - 3, int(coin.y) - 3))


# ── 효과 도구 ─────────────────────────────────────────────────────────────
def tint(image, color, amount):
    solid = Image.new('RGBA', image.size, color + (0,))
    out = image.copy()
    px = out.load()
    for y in range(out.height):
        for x in range(out.width):
            r, g, b, a = px[x, y]
            if a:
                px[x, y] = (int(r + (color[0] - r) * amount), int(g + (color[1] - g) * amount), int(b + (color[2] - b) * amount), a)
    return out


def flash(scene, amount, color=(255, 255, 255)):
    if amount <= 0:
        return scene
    overlay = Image.new('RGBA', scene.size, color + (int(255 * amount),))
    out = scene.copy()
    out.alpha_composite(overlay)
    return out


def sparkle(draw, x, y, size, color=GOLD_LIGHT):
    draw.line((x - size, y, x + size, y), fill=color)
    draw.line((x, y - size, x, y + size), fill=color)


def dissolve(image, progress, seed=7):
    """위쪽부터 점점 흩어지듯 사라진다 (progress 0~1)"""
    out = image.copy()
    px = out.load()
    rng = random.Random(seed)
    for y in range(out.height):
        for x in range(out.width):
            r, g, b, a = px[x, y]
            if not a:
                continue
            threshold = (1 - y / out.height) * 0.6 + rng.random() * 0.4
            if progress > threshold:
                px[x, y] = (0, 0, 0, 0)
    return out


FONT = ImageFont.load_default()


def money_text(scene, text, x, y, color=GOLD):
    draw = ImageDraw.Draw(scene)
    for dx in (-1, 0, 1):
        for dy in (-1, 0, 1):
            draw.text((x + dx, y + dy), text, font=FONT, fill=OUTLINE)
    draw.text((x, y), text, font=FONT, fill=color)


# ── 단계별 연출 ──────────────────────────────────────────────────────────────
def render_tier(tier):
    rng = random.Random(tier)
    level = {1: 4, 2: 10, 3: 22}[tier]
    sword = sword_image(level)
    base = base_scene()
    floor_y = H // 2 + 44 + 22
    cx, cy = W // 2, H // 2 + 44 - 10
    coins = []
    frames = []
    length = {1: 40, 2: 60, 3: 90}[tier]
    vanish = {1: 10, 2: 14, 3: 20}[tier]
    amount = {1: '+$40', 2: '+$2,300', 3: '+$250,000'}[tier]

    def spawn(count, spread, power, delay_step=1, from_top=False):
        for i in range(count):
            if from_top:
                coins.append(Coin(rng.uniform(30, W - 30), -8, rng.uniform(-0.4, 0.4), rng.uniform(0, 1), floor_y + rng.randint(-8, 8),
                                  delay=i * delay_step + rng.randint(0, 3)))
            else:
                coins.append(Coin(cx + rng.uniform(-6, 6), cy, rng.uniform(-spread, spread), -rng.uniform(power * 0.6, power),
                                  floor_y + rng.randint(-8, 8), delay=i * delay_step))

    if tier == 1:
        spawn(5, 1.6, 4.2, 1)
    elif tier == 2:
        spawn(12, 2.4, 5.2, 1)
    else:
        spawn(14, 3.2, 6.0, 1)
        spawn(26, 0, 0, 2, from_top=True)

    for f in range(length):
        scene = base.copy()

        # 검: 처음에 번쩍(흰색 → 단계 색), 위로 떠오르며 흩어진다
        if f < vanish + 6:
            glow = max(0.0, 1 - f / 5) if tier == 1 else max(0.0, 1 - f / 7)
            color = {1: (255, 255, 255), 2: (200, 230, 255), 3: (255, 235, 150)}[tier]
            image = tint(sword, color, 0.25 + 0.75 * glow if f < 6 else 0.3)
            rise = int(f * (1.2 if tier < 3 else 1.6))
            if f >= 4:
                image = dissolve(image, min(1.0, (f - 4) / vanish))
            scene.alpha_composite(image, (SWORD_POS[0], SWORD_POS[1] - rise))

        for coin in coins:
            coin.step()
        draw_coins(scene, coins)

        draw = ImageDraw.Draw(scene)
        if tier >= 2 and f < 24:
            for i in range(3 if tier == 2 else 6):
                angle = i * 2.1 + f * 0.3
                radius = 8 + f * 1.4
                sparkle(draw, int(cx + math.cos(angle) * radius), int(cy - 30 + math.sin(angle) * radius * 0.7), 1 + (f // 4) % 2)

        if f >= 6:
            rise = min(14, (f - 6) // 1)
            if f < length - 8 or f % 2 == 0:
                money_text(scene, amount, W // 2 - 4 * len(amount) // 2 - 4, 46 - rise)

        if tier == 3 and f < 4:
            scene = flash(scene, 0.7 - f * 0.18)
        if tier == 2 and f < 3:
            scene = flash(scene, 0.35 - f * 0.1, (255, 240, 200))

        if tier == 3 and f < 12:
            shake = (1, -1, 2, -2)[f % 4] * (1 if f < 6 else 0)
            shifted = Image.new('RGBA', scene.size)
            shifted.alpha_composite(scene, (shake, 0))
            scene = shifted

        frames.append(scene.convert('RGB').resize((W * SCALE, H * SCALE), Image.NEAREST))

    return frames


def save_gif(frames, path):
    frames[0].save(path, save_all=True, append_images=frames[1:] + [frames[-1]] * 20, duration=20, loop=0, optimize=False)


# ── 효과음 ───────────────────────────────────────────────────────────────────
from make_sfx import sell_1, sell_2, sell_3


def main():
    os.makedirs(OUT, exist_ok=True)

    for tier in (1, 2, 3):
        save_gif(render_tier(tier), os.path.join(OUT, 'sell_%d.gif' % tier))

    sounds = [sell_1(), sell_2(), sell_3()]
    gap = [0.0] * sfx.samples(0.7)
    combined = []

    for tier, data in enumerate(sounds, 1):
        sfx.write_wav(os.path.join(OUT, 'sell_%d.wav' % tier), data)
        combined += data + gap
        print('sell_%d  %.2f초' % (tier, len(data) / sfx.RATE))

    sfx.write_wav(os.path.join(OUT, 'sell_all.wav'), combined)
    print('출력:', OUT)
    return 0


if __name__ == '__main__':
    sys.exit(main())
