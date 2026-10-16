"""작은 도트 고양이 시안 (게임에는 아직 적용하지 않는다).

사용자가 준 사진(하얀 털북숭이, 분홍 귀·코, 감은 눈)을 모티브로 32x32 도트 고양이를 직접 그린다.
동작: 걸어 들어오기(4장) → 앉기 → 꼬리 흔들기(3장).
색은 투명 + 7색: 외곽선, 흰 털, 밝은 그림자, 어두운 그림자, 분홍, 진분홍, 눈.

수동 실행: python -B tools/make_cat_sprite.py
출력: build/cat_sprite_preview/ (프레임 시트, 강화 화면에 얹은 움짤)
"""

import os
import sys

from PIL import Image, ImageDraw, ImageFont

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import preview_sell_fx as base

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
OUT = os.path.join(ROOT, 'build', 'cat_sprite_preview')

SIZE = 32
# 팔레트 번호: 0 투명, 1 외곽선, 2 흰 털, 3 밝은 그림자, 4 어두운 그림자, 5 분홍, 6 진분홍, 7 눈
PALETTE = {
    0: (0, 0, 0, 0),
    1: (96, 80, 104, 255),
    2: (252, 250, 250, 255),
    3: (226, 220, 232, 255),
    4: (190, 180, 204, 255),
    5: (250, 176, 184, 255),
    6: (226, 128, 148, 255),
    7: (72, 56, 72, 255),
}


class Canvas:
    def __init__(self):
        self.image = Image.new('P', (SIZE, SIZE), 0)
        self.draw = ImageDraw.Draw(self.image)

    def ellipse(self, box, color):
        self.draw.ellipse(box, fill=color)

    def ellipse_o(self, box, color):
        """바깥에 1px 외곽선 고리를 먼저 그려, 앞에 놓인 몸이 뒤의 꼬리와 구분되게 한다"""
        x0, y0, x1, y1 = box
        self.draw.ellipse((x0 - 1, y0 - 1, x1 + 1, y1 + 1), fill=1)
        self.draw.ellipse(box, fill=color)

    def circle_path(self, points, radius, color):
        for x, y, r in points:
            self.draw.ellipse((x - r, y - r, x + r, y + r), fill=color)

    def polygon(self, points, color):
        self.draw.polygon(points, fill=color)

    def rect(self, box, color):
        self.draw.rectangle(box, fill=color)

    def pixel(self, x, y, color):
        if 0 <= x < SIZE and 0 <= y < SIZE:
            self.image.putpixel((x, y), color)

    def outline(self):
        """색이 칠해진 부분 둘레(위·아래·좌·우)의 빈 칸에 외곽선을 두른다"""
        src = self.image.copy()
        for y in range(SIZE):
            for x in range(SIZE):
                if src.getpixel((x, y)) == 0:
                    for dx, dy in ((1, 0), (-1, 0), (0, 1), (0, -1)):
                        nx, ny = x + dx, y + dy
                        if 0 <= nx < SIZE and 0 <= ny < SIZE and src.getpixel((nx, ny)) not in (0, 1):
                            self.image.putpixel((x, y), 1)
                            break

    def shade(self, below_y, color=3, from_x=0):
        """흰 털(2) 중 below_y 아래쪽을 그림자 색으로 바꾼다"""
        for y in range(below_y, SIZE):
            for x in range(from_x, SIZE):
                if self.image.getpixel((x, y)) == 2:
                    self.image.putpixel((x, y), color)

    def indices(self):
        return self.image.copy()

    def rgba(self):
        out = self.image.copy()
        out.putpalette([c for i in range(8) for c in PALETTE[i][:3]] + [0] * (256 * 3 - 24))
        rgba = out.convert('RGBA')
        px = rgba.load()
        src = self.image.load()
        for y in range(SIZE):
            for x in range(SIZE):
                px[x, y] = PALETTE[src[x, y]]
        return rgba


def head(c, hx, hy, look_right=True):
    """머리: 동그란 얼굴 + 분홍 귀 둘 + 감은 눈 + 분홍 코. (hx, hy)는 얼굴 왼쪽 위"""
    # 귀
    c.polygon([(hx + 1, hy + 4), (hx + 2, hy - 2), (hx + 6, hy + 1)], 2)
    c.polygon([(hx + 8, hy + 1), (hx + 12, hy - 2), (hx + 13, hy + 4)], 2)
    c.pixel(hx + 2, hy + 0, 5)
    c.pixel(hx + 2, hy + 1, 5)
    c.pixel(hx + 3, hy + 1, 5)
    c.pixel(hx + 11, hy + 0, 5)
    c.pixel(hx + 11, hy + 1, 5)
    c.pixel(hx + 10, hy + 1, 5)
    # 얼굴
    c.ellipse((hx, hy + 1, hx + 14, hy + 11), 2)
    # 볼 털 (살짝 삐죽)
    c.pixel(hx - 1, hy + 7, 2)
    c.pixel(hx + 15, hy + 7, 2)
    # 감은 눈 ^ ^ 대신 ‿ ‿ 모양
    c.pixel(hx + 3, hy + 6, 7)
    c.pixel(hx + 4, hy + 7, 7)
    c.pixel(hx + 5, hy + 6, 7)
    c.pixel(hx + 9, hy + 6, 7)
    c.pixel(hx + 10, hy + 7, 7)
    c.pixel(hx + 11, hy + 6, 7)
    # 코와 입
    c.pixel(hx + 7, hy + 8, 6)
    c.pixel(hx + 6, hy + 9, 4)
    c.pixel(hx + 8, hy + 9, 4)


def side_head(c, hx, hy):
    """옆모습 머리 (오른쪽을 본다): 귀 둘(하나는 가려진다), 눈 하나, 코"""
    c.polygon([(hx, hy + 4), (hx + 1, hy - 2), (hx + 5, hy + 1)], 2)
    c.polygon([(hx + 6, hy + 1), (hx + 9, hy - 2), (hx + 11, hy + 3)], 2)
    c.pixel(hx + 1, hy + 0, 5)
    c.pixel(hx + 1, hy + 1, 5)
    c.pixel(hx + 8, hy + 0, 5)
    c.pixel(hx + 8, hy + 1, 5)
    c.ellipse((hx, hy + 1, hx + 12, hy + 10), 2)
    c.pixel(hx - 1, hy + 6, 2)       # 볼 털
    c.pixel(hx + 8, hy + 5, 7)       # 눈
    c.pixel(hx + 9, hy + 6, 7)
    c.pixel(hx + 12, hy + 6, 6)      # 코
    c.pixel(hx + 11, hy + 8, 4)


def legs_walk(c, phase):
    """네 다리: phase 0~3에 따라 앞뒤로 번갈아 움직인다"""
    offsets = [(0, 2, -2, 0), (1, 1, -1, 1), (2, 0, 0, 2), (1, 1, -1, 1)]
    ox = offsets[phase]
    # 뒷다리 둘, 앞다리 둘 (몸통 아래 y 24~28)
    for x, dx in ((7, ox[0]), (11, ox[1])):
        c.rect((x + dx, 23, x + dx + 2, 28), 2)
    for x, dx in ((17, ox[2]), (21, ox[3])):
        c.rect((x + dx, 23, x + dx + 2, 28), 2)


def tail_up(c, curl):
    """걸을 때의 꼬리: 몸 뒤에서 위로 솟은 복슬 꼬리, 끝이 살짝 흔들린다"""
    c.circle_path([(5, 17, 2), (3, 13, 3), (3 + curl, 9, 3), (4 + curl, 6, 2)], 0, 2)


def _walk_canvas(phase):
    c = Canvas()
    tail_up(c, 1 if phase % 2 else 0)
    c.ellipse_o((5, 12, 24, 25), 2)               # 몸통 (꼬리 앞)
    legs_walk(c, phase)
    side_head(c, 18, 7 + (1 if phase % 2 else 0))
    c.outline()
    c.shade(21, 3)
    c.shade(24, 4)
    c.outline()
    return c


def frame_walk(phase):
    return _walk_canvas(phase).rgba()


def frame_walk_index(phase):
    return _walk_canvas(phase).indices()


def _sit_canvas(wag):
    """앉은 모습 (정면): wag 0~2는 꼬리 흔들기 위치(왼쪽 → 가운데 → 오른쪽)"""
    c = Canvas()
    # 꼬리: 몸 옆 바닥에서 위로 솟아 끝이 좌우로 흔들린다
    tip = (2, 5, 8)[wag]
    c.circle_path([(7, 27, 3), (4, 22, 3), (tip - 1 + (wag == 2) * 0, 17, 3), (tip + (0, 1, 2)[wag] - 1, 13, 2)], 0, 2)
    # 몸통: 앉아서 세로로 긴 모양
    c.ellipse_o((9, 15, 24, 30), 2)
    # 앞발 둘
    c.rect((12, 25, 15, 30), 2)
    c.rect((18, 25, 21, 30), 2)
    c.pixel(16, 25, 3)
    head(c, 9, 4)
    c.outline()
    c.shade(24, 3, 8)
    c.shade(28, 4, 8)
    c.outline()
    return c


def frame_sit(wag):
    return _sit_canvas(wag).rgba()


def frame_sit_index(wag):
    return _sit_canvas(wag).indices()


def main():
    os.makedirs(OUT, exist_ok=True)
    walk = [frame_walk(i) for i in range(4)]
    sit = [frame_sit(i) for i in range(3)]

    # 시트: 8배 확대
    scale = 8
    sheet = Image.new('RGBA', (SIZE * scale * 7 // 2 + 40, SIZE * scale // 2 + 20), (60, 52, 64, 255))
    x = 10
    for frame in walk + sit:
        big = frame.resize((SIZE * scale // 2, SIZE * scale // 2), Image.NEAREST)
        sheet.alpha_composite(big, (x, 10))
        x += SIZE * scale // 2 + 4
    sheet.convert('RGB').save(os.path.join(OUT, 'frames.png'))

    # 움짤: 강화 화면 바닥에서 왼쪽에서 걸어 들어와 가운데에서 앉고 꼬리를 흔든다
    scene = base.base_scene()
    floor_y = 160 // 2 + 44 + 30 - SIZE       # 모루 바닥 근처
    font = ImageFont.truetype(r'C:\Windows\Fonts\gulim.ttc', 12, index=0)
    frames = []
    target_x = 150
    speed = 2.0
    arrive = int((target_x + 34) / speed)       # 가운데에 닿는 프레임
    total = arrive + 170
    position = -34.0

    for f in range(total):
        image = scene.copy()

        if f < arrive:
            position += speed
            sprite = walk[(f // 5) % 4]
        else:
            u = f - arrive
            sprite = sit[(u // 8) % 3] if u >= 12 else sit[1]       # 앉자마자 잠깐 멈췄다가 꼬리를 흔든다
            position = target_x

        image.alpha_composite(sprite, (int(position), floor_y))

        u = f - arrive
        if u >= 20:
            draw = ImageDraw.Draw(image)
            lines = (('고양이다!', 100), ('보은이다!', 114)) if u >= 70 else (('고양이다!', 107),)
            for line, y in lines:
                width = draw.textlength(line, font=font)
                for dx in (-1, 0, 1):
                    for dy in (-1, 0, 1):
                        draw.text(((240 - width) / 2 + dx, y + dy), line, font=font, fill=(0, 0, 0, 255))
                draw.text(((240 - width) / 2, y), line, font=font, fill=(255, 255, 255, 255))

        frames.append(image.convert('RGB').resize((240 * 4, 160 * 4), Image.NEAREST))

    # 확인용 스틸 (걷는 중 / 닿은 직후 / 꼬리 흔들기 / 메시지)
    stills = Image.new('RGB', (480 * 4, 320))
    for i, f in enumerate((arrive // 2, arrive + 4, arrive + 30, arrive + 100)):
        stills.paste(frames[f].resize((480, 320)), (i * 480, 0))
    stills.save(os.path.join(OUT, 'stills.png'))

    frames[0].save(os.path.join(OUT, 'cat_walk_sit_wag.gif'), save_all=True, append_images=frames[1:] + [frames[-1]] * 30,
                   duration=20, loop=0, optimize=False, disposal=1)
    print('출력:', OUT)
    return 0


if __name__ == '__main__':
    sys.exit(main())
