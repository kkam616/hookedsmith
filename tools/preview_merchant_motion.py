"""수상한 상인 이벤트의 움직임 시안 (build/merchant_mock/motion.gif + 프레임 시트). 상인·사진·방은 임시 그림(코드로 그린 도트).
도둑 이벤트(run_thief_event)와 같은 흐름: 알림 번쩍 → 창이 열림 → 상인이 걸어 들어옴 → 인사(▼ 깜빡임, A) → 사진 번쩍 → 대금 두 배 메시지 → 금화.
사용: python -B tools/preview_merchant_motion.py
"""
import math
import os
import random
import sys

from PIL import Image, ImageDraw, ImageEnhance

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
from make_recipe_paper import GameFont

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
OUT = os.path.join(ROOT, 'build', 'merchant_mock')
SCALE = 3
WIN = (77, 41, 77 + 86, 41 + 70)         # 창 (화면 좌표, 도둑·고양이 이벤트와 같은 자리)
WHITE = (255, 255, 255)
BLACK = (0, 0, 0)
GOLD = (255, 214, 96)

# 상인 색
HAT = (60, 36, 70)
HATBAND = (210, 160, 50)
SKIN = (232, 176, 128)
MUSTACHE = (70, 44, 30)
COAT = (110, 50, 120)
COAT_DARK = (78, 34, 90)
BUTTON = (240, 200, 80)
PACK = (140, 92, 48)
PACK_DARK = (92, 58, 30)
BOOT = (50, 36, 30)
POUCH = (236, 196, 64)


def merchant_frame(pose, step=0):
    """32x32 투명 도트. pose: walk / tip(모자 인사) / give(주머니 내밀기)"""
    im = Image.new('RGBA', (32, 32), (0, 0, 0, 0))
    d = ImageDraw.Draw(im)
    bob = 1 if (pose == 'walk' and step % 2 == 1) else 0
    # 배낭 (왼쪽, 걷기 4프레임 출렁임)
    d.rectangle((3, 10 + bob, 11, 23 + bob), fill=PACK_DARK)
    d.rectangle((4, 11 + bob, 10, 22 + bob), fill=PACK)
    d.rectangle((3, 8 + bob, 9, 10 + bob), fill=(210, 190, 150))                    # 말아 올린 이불
    d.line((11, 12 + bob, 13, 16 + bob), fill=PACK_DARK)                            # 어깨끈
    # 다리
    legs = [(13, 17), (17, 21)] if True else []
    offsets = {0: (0, 0), 1: (-1, 1), 2: (0, 0), 3: (1, -1)}[step % 4] if pose == 'walk' else (0, 0)
    d.rectangle((13 + offsets[0], 25, 16 + offsets[0], 30), fill=BOOT)
    d.rectangle((17 + offsets[1], 25, 20 + offsets[1], 30), fill=BOOT)
    # 외투
    d.rectangle((11, 13 + bob, 21, 26 + bob), fill=COAT)
    d.rectangle((11, 13 + bob, 13, 26 + bob), fill=COAT_DARK)
    d.line((12, 20 + bob, 21, 20 + bob), fill=BUTTON)                               # 허리띠
    for y in (15, 17, 23):
        d.point((19, y + bob), fill=BUTTON)
    # 머리 + 모자
    d.rectangle((13, 6 + bob, 20, 12 + bob), fill=SKIN)
    d.rectangle((15, 9 + bob, 20, 10 + bob), fill=MUSTACHE)                         # 수염
    d.point((18, 8 + bob), fill=BLACK)
    d.rectangle((10, 4 + bob, 23, 6 + bob), fill=HAT)                               # 챙
    d.rectangle((13, 0 + bob, 20, 4 + bob), fill=HAT)
    d.line((13, 3 + bob, 20, 3 + bob), fill=HATBAND)
    # 팔
    if pose == 'tip':
        d.rectangle((20, 8 + bob, 24, 10 + bob), fill=COAT)                         # 모자챙을 만지는 팔
        d.rectangle((23, 6 + bob, 25, 8 + bob), fill=SKIN)
    elif pose == 'give':
        d.rectangle((20, 15 + bob, 27, 17 + bob), fill=COAT)
        d.ellipse((26, 15 + bob, 31, 21 + bob), fill=POUCH)                         # 금화 주머니
        d.line((27, 15 + bob, 30, 15 + bob), fill=BUTTON)
    else:
        d.rectangle((19, 14 + bob, 22, 19 + bob), fill=COAT)
        d.rectangle((20, 19 + bob, 22, 20 + bob), fill=SKIN)
    return im


def forge_bg():
    bg = Image.open(os.path.join(ROOT, 'graphics', 'forge_bg_sunset.bmp')).convert('RGB').crop((0, 0, 240, 160))
    return ImageEnhance.Brightness(bg).enhance(0.75)


def room_image(coins=False):
    """창 안(86x70): 노을 방. 받침대(판매 직후라 비어 있음), coins면 금화 더미"""
    w, h = WIN[2] - WIN[0], WIN[3] - WIN[1]
    im = Image.new('RGB', (w, h))
    d = ImageDraw.Draw(im)
    for y in range(h):
        t = y / h
        c = (int(70 + 170 * t), int(40 + 80 * t), int(100 - 30 * t)) if y < 44 else (60, 38, 34)
        d.line((0, y, w, y), fill=c)
    d.ellipse((58, 18, 76, 36), fill=(255, 214, 120))                                # 지는 해
    d.rectangle((0, 44, w, h), fill=(66, 42, 36))
    for x in range(-10, w, 14):
        d.line((x, 44, x + 8, h), fill=(48, 30, 26))
    d.rectangle((66, 40, 76, 48), fill=(110, 66, 40))                                 # 받침대
    d.rectangle((66, 40, 76, 41), fill=(150, 96, 56))
    if coins:
        for i, (x, y) in enumerate([(67, 36), (71, 36), (69, 33), (73, 36), (70, 30), (68, 39)]):
            d.ellipse((x, y, x + 4, y + 3), fill=(250, 210, 70), outline=(180, 120, 20))
    return im


def photo_image():
    w, h = WIN[2] - WIN[0], WIN[3] - WIN[1]
    im = Image.new('RGB', (w, h))
    d = ImageDraw.Draw(im)
    for y in range(h):
        d.line((0, y, w, y), fill=(int(160 + 70 * y / h), int(60 + 70 * y / h), int(70 - 20 * y / h)))
    d.ellipse((20, 8, 66, 54), fill=SKIN, outline=(60, 30, 20))                       # 얼굴(임시)
    d.rectangle((12, 4, 74, 14), fill=HAT)
    d.rectangle((26, -8, 60, 6), fill=HAT)
    d.line((26, 4, 60, 4), fill=HATBAND)
    d.rectangle((28, 36, 58, 42), fill=MUSTACHE)                                       # 수염
    d.arc((30, 38, 56, 52), 10, 170, fill=(255, 255, 255), width=2)                   # 능글맞은 미소
    d.ellipse((34, 24, 38, 28), fill=BLACK)
    d.ellipse((48, 24, 52, 28), fill=BLACK)
    d.ellipse((60, 46, 82, 68), fill=POUCH, outline=(150, 100, 20))                   # 금화 주머니
    d.text((20, 58), 'PHOTO', fill=(255, 240, 200))
    return im


def draw_window(base, content, alpha=1.0, scale=1.0):
    """창 틀을 그리고 그 안에 content를 넣는다 (scale로 열리는 연출)"""
    w, h = content.size
    sw, sh = max(2, int(w * scale)), max(2, int(h * scale))
    cx, cy = (WIN[0] + WIN[2]) // 2, (WIN[1] + WIN[3]) // 2
    left, top = cx - sw // 2, cy - sh // 2
    small = content.resize((sw, sh), Image.NEAREST)
    if alpha < 1.0:
        small = Image.blend(base.crop((left, top, left + sw, top + sh)), small, alpha)
    base.paste(small, (left, top))
    d = ImageDraw.Draw(base)
    d.rectangle((left - 2, top - 2, left + sw + 1, top + sh + 1), outline=(190, 150, 70), width=2)
    d.rectangle((left - 3, top - 3, left + sw + 2, top + sh + 2), outline=(40, 24, 20), width=1)
    return left, top


def flash(img, level):
    if level <= 0:
        return img
    return Image.blend(img, Image.new('RGB', img.size, (255, 255, 255)), min(1, level))


def main():
    os.makedirs(OUT, exist_ok=True)
    random.seed(5)
    font = GameFont('hs_font.bmp', 'hs_font.h', False)
    forge = forge_bg()
    room, room_coins, photo = room_image(), room_image(True), photo_image()
    frames = []

    def snap(img, tag=None):
        frames.append(img.copy())
        return img

    def text_line(img, text, y=148, color=WHITE):
        font.draw(img, text, 120 - font.width(text) // 2, y, color, BLACK)

    def sprite_on(img, frame_img, x, y, alpha=1.0):
        # 창 안에서만 보이도록 잘라 붙인다
        layer = Image.new('RGBA', img.size, (0, 0, 0, 0))
        layer.paste(frame_img, (int(x), int(y)), frame_img)
        if alpha < 1.0:
            a = layer.getchannel('A').point(lambda v: int(v * alpha))
            layer.putalpha(a)
        mask = Image.new('L', img.size, 0)
        ImageDraw.Draw(mask).rectangle((WIN[0], WIN[1], WIN[2] - 1, WIN[3] - 1), fill=255)
        clipped = Image.new('RGBA', img.size, (0, 0, 0, 0))
        clipped.paste(layer, (0, 0), mask)
        base = img.convert('RGBA')
        base.alpha_composite(clipped)
        img.paste(base.convert('RGB'))

    # 1) 번쩍 + 창 열림 (0~24)
    for f in range(28):
        img = forge.copy()
        if f > 6:
            draw_window(img, room, scale=min(1.0, (f - 6) / 12))
        snap(flash(img, max(0, 0.9 - f * 0.08)))

    # 2) 상인이 걸어 들어옴 (40프레임) + 두리번
    stop_x = WIN[0] + 38
    start_x = WIN[0] - 6
    for u in range(40):
        img = forge.copy()
        draw_window(img, room)
        x = start_x + (stop_x - start_x) * u / 40
        sprite_on(img, merchant_frame('walk', u // 5), x, WIN[1] + 22, min(1.0, u / 10))
        snap(img)

    # 3) 모자 인사, ▼ 깜빡임 (A를 기다림)
    for f in range(72):
        img = forge.copy()
        draw_window(img, room)
        pose = 'tip' if (f // 12) % 2 == 0 else 'walk'
        sprite_on(img, merchant_frame(pose, 0), stop_x, WIN[1] + 22)
        if (f // 10) % 2 == 0:
            text_line(img, '▼', 148, GOLD)
        snap(img)

    # 4) A → 사진 번쩍 + 웃음 (30프레임) + 1단계 메시지
    for f in range(40):
        img = forge.copy()
        draw_window(img, photo)
        if f > 10:
            text_line(img, '수상한 상인이 웃고 있다...', 128)
        if f > 22 and (f // 10) % 2 == 0:
            text_line(img, '▼', 148, GOLD)
        snap(flash(img, max(0, 0.9 - f * 0.12)))

    # 5) A → 금화 더미 방 + 상인이 주머니를 내밀고 2단계 메시지 + 금화
    for f in range(70):
        img = forge.copy()
        draw_window(img, room_coins)
        sprite_on(img, merchant_frame('give', 0), stop_x, WIN[1] + 22)
        if f > 6:
            text_line(img, '대금의 100%를 더 얹어 주었다!', 126)
            text_line(img, '+ $1,250', 140, GOLD)
        snap(img)

    # 6) 강화 화면으로 복귀: 금화가 튀며 소지금 상승
    coins = [(random.randint(90, 150), -random.randint(0, 40), random.uniform(-1, 1), random.uniform(0.5, 2.5)) for _ in range(18)]
    for f in range(60):
        img = forge_bg()
        d = ImageDraw.Draw(img)
        for (x, y0, vx, vy) in coins:
            t = f - random.Random(int(x)).randint(0, 10)
            if t < 0:
                continue
            y = y0 + vy * t * 1.6 + 0.04 * t * t * 8
            if y < 130:
                d.ellipse((x + vx * t, y, x + vx * t + 4, y + 3), fill=(250, 210, 70), outline=(180, 120, 20))
        money = 5000 + int(1250 * min(1, f / 40))
        font.draw(img, '$%s' % format(money, ','), 236, 148, (248, 216, 48), (232, 104, 8), 'right')
        snap(img)

    big = [f.resize((240 * SCALE, 160 * SCALE), Image.NEAREST) for f in frames]
    palette_frames = [f.quantize(colors=96, method=Image.Quantize.MEDIANCUT, dither=Image.Dither.NONE) for f in big]
    palette_frames[0].save(os.path.join(OUT, 'motion.gif'), save_all=True, append_images=palette_frames[1:], duration=50, loop=0, optimize=False)

    # 정지 시트: 단계별 대표 장면
    picks = [10, 40, 52, 70, 120, 140, 190, 225, 250]
    pick_frames = [frames[min(p, len(frames) - 1)] for p in picks]
    sheet = Image.new('RGB', (3 * 250, 3 * 170), (30, 30, 34))
    for i, f in enumerate(pick_frames):
        sheet.paste(f, ((i % 3) * 250 + 5, (i // 3) * 170 + 5))
    sheet.resize((sheet.width * 2, sheet.height * 2), Image.NEAREST).save(os.path.join(OUT, 'sheet.png'))

    # 상인 프레임 6장 (스프라이트 시트 시안)
    names = [('walk', 0), ('walk', 1), ('walk', 2), ('walk', 3), ('tip', 0), ('give', 0)]
    strip = Image.new('RGBA', (32 * 6 * 6, 32 * 6), (60, 60, 66, 255))
    for i, (p, s) in enumerate(names):
        fr = merchant_frame(p, s).resize((192, 192), Image.NEAREST)
        strip.alpha_composite(fr, (i * 192, 0))
    strip.save(os.path.join(OUT, 'merchant_frames.png'))
    print('frames', len(frames))


if __name__ == '__main__':
    main()
