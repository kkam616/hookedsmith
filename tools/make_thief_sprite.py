"""도둑 도트 시안 (게임에는 아직 적용하지 않는다).

복면을 쓰고 자루를 멘 밤도둑 32x32. 동작: 살금살금 걷기(옆모습 4장) → 두리번(1장) → 검을 쥐고 정면으로 돌아 웃기(정면 2장 흔들림).
고양이(make_cat_sprite.py)와 같은 방식으로 그린다.

수동 실행: python -B tools/make_thief_sprite.py
출력: build/thief_preview/frames.png
"""

import os
import sys

from PIL import Image

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import make_cat_sprite as cat

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
OUT = os.path.join(ROOT, 'build', 'thief_preview')
SIZE = 32

# 0 투명, 1 외곽선, 2 망토, 3 망토 그림자, 4 두건 하이라이트, 5 살색, 6 복면 띠(검정), 7 흰색, 8 자루, 9 자루 그림자, 10 금, 11 강철, 12 붉은 두건
PALETTE = {
    0: (0, 0, 0, 0),
    1: (20, 16, 32, 255),
    2: (60, 64, 104, 255),
    3: (38, 40, 70, 255),
    4: (96, 104, 148, 255),
    5: (240, 200, 168, 255),
    6: (22, 18, 30, 255),
    7: (252, 250, 250, 255),
    8: (150, 100, 56, 255),
    9: (100, 64, 36, 255),
    10: (255, 208, 64, 255),
    11: (206, 214, 232, 255),
    12: (208, 48, 56, 255),
}


def to_rgba(canvas):
    rgba = Image.new('RGBA', (SIZE, SIZE))
    px = rgba.load()
    src = canvas.image.load()
    for y in range(SIZE):
        for x in range(SIZE):
            px[x, y] = PALETTE[src[x, y]]
    return rgba


def sack(c, x, y, w=10, h=11):
    """자루: 불룩한 보따리 + 위쪽 묶은 끈 + 금화 표시"""
    c.ellipse_o((x, y, x + w, y + h), 8)
    c.ellipse((x + 2, y + h - 5, x + w - 1, y + h - 1), 9)
    c.pixel(x + w // 2, y - 1, 9)
    c.pixel(x + w // 2 + 1, y - 2, 9)
    c.pixel(x + w // 2 - 1, y + h // 2, 10)
    c.pixel(x + w // 2, y + h // 2, 10)


def line(c, x0, y0, x1, y1, color, width=1):
    c.draw.line((x0, y0, x1, y1), fill=color, width=width)


def striped_torso(c, points, top, bottom):
    """줄무늬 셔츠: 흰색 몸통에 어두운 가로줄"""
    c.polygon(points, 7)
    for y in range(top, bottom):
        if (y - top) % 3 == 1:
            for x in range(SIZE):
                if c.image.getpixel((x, y)) == 7:
                    c.image.putpixel((x, y), 6)


def side_thief(phase, peek=False):
    """살금살금 걷는 도둑 옆모습 (오른쪽을 본다): 줄무늬 셔츠, 눈가리개, 납작모자, 어깨에 멘 자루, 구부린 무릎.
    phase 0~3: 다리가 번갈아 움직이고 몸이 살짝 출렁인다"""
    c = cat.Canvas()
    bob = (0, 1, 0, 1)[phase]
    lean = 2 if peek else 0

    # 어깨에 멘 자루 (뒤쪽 위) + 막대
    line(c, 6, 14 + bob, 15, 10 + bob, 9, 1)
    sack(c, 1, 12 + bob, 9, 9)

    # 뒷다리·앞다리: 허벅지(앞으로 굽힘) + 정강이 + 장화
    swing = [(-3, 3), (-1, 1), (3, -3), (1, -1)][phase]
    for hip_x, dx in ((13, swing[0]), (16, swing[1])):
        knee_x, foot_x = hip_x + 2 + dx // 2, hip_x + dx
        line(c, hip_x, 22 + bob, knee_x, 26, 2, 3)
        line(c, knee_x, 26, foot_x, 29, 2, 3)
        c.rect((foot_x - 1, 28, foot_x + 3, 30), 1)

    # 허리띠와 바지 윗부분
    c.rect((11 + lean, 20 + bob, 20 + lean, 23 + bob), 2)

    # 줄무늬 셔츠 몸통 (앞으로 숙임)
    top = 11 + bob
    striped_torso(c, [(13 + lean, top), (20 + lean, top + 1), (20 + lean, 21 + bob), (11, 21 + bob)], top, 21 + bob)

    # 팔: 뒤팔은 자루 막대를 잡고, 앞팔은 살금살금 앞으로 굽힘
    line(c, 13 + lean, top + 2, 10, top + 6, 7, 2)
    c.pixel(10, top + 7, 5)
    line(c, 19 + lean, top + 2, 23 + lean, top + 6, 7, 2)
    c.pixel(24 + lean, top + 7, 5)

    # 머리: 살색 얼굴 + 납작모자 + 검은 눈가리개
    hx, hy = 15 + lean, 2 + bob
    c.ellipse_o((hx, hy + 1, hx + 8, hy + 9), 5)
    c.rect((hx - 1, hy, hx + 8, hy + 2), 6)                  # 모자 몸통
    c.rect((hx + 7, hy + 2, hx + 11, hy + 3), 6)             # 모자 챙
    c.rect((hx + 1, hy + 4, hx + 8, hy + 5), 6)              # 눈가리개
    c.pixel(hx + 6, hy + 4, 7)                               # 눈 (흰 점)
    c.pixel(hx + 9, hy + 6, 5)                               # 코
    c.pixel(hx + 5, hy + 8, 6)                               # 입꼬리
    c.outline()
    return c


def grab_pose():
    """옆모습으로 한 손을 뻗어 검을 쥔 모습"""
    c = side_thief(1, peek=True)
    c.rect((24, 14, 28, 15), 7)             # 소매
    c.rect((29, 14, 30, 15), 5)             # 손
    c.rect((30, 4, 30, 13), 11)             # 검날
    c.rect((29, 14, 31, 14), 10)            # 가드
    c.outline()
    return c


def front_thief(laugh):
    """정면: 줄무늬 셔츠 + 납작모자 + 검은 눈가리개 + 크게 웃는 입, 한 손에 훔친 검, 어깨에 자루. laugh 0/1은 어깨와 입이 번갈아 움직인다"""
    c = cat.Canvas()
    dy = (0, 1)[laugh]

    # 어깨에 멘 자루 (왼쪽 뒤)
    sack(c, 0, 15 + dy, 10, 10)

    # 다리 + 장화
    c.rect((10, 23, 14, 28), 2)
    c.rect((17, 23, 21, 28), 2)
    c.rect((9, 28, 14, 30), 1)
    c.rect((17, 28, 22, 30), 1)

    # 줄무늬 셔츠 몸통
    top = 13 + dy
    striped_torso(c, [(9, top), (22, top), (23, 23), (8, 23)], top, 23)
    c.rect((8, 22, 23, 24), 2)                                # 허리띠/바지

    # 오른팔을 들어 올려 검을 쥔다 (소매 + 손)
    line(c, 22, top + 1, 26, top - 5, 7, 3)
    c.rect((25, top - 8, 28, top - 5), 5)                     # 손
    c.rect((26, 0, 27, top - 8), 11)                          # 검날
    c.rect((24, top - 8, 29, top - 8), 10)                    # 가드
    c.pixel(26, 1, 7)
    # 왼팔은 자루 끈을 쥠
    line(c, 9, top + 1, 6, top + 6, 7, 3)
    c.pixel(6, top + 7, 5)

    # 머리: 살색 얼굴 + 납작모자 + 눈가리개 + 활짝 웃는 입
    hy = 2 + dy
    c.ellipse_o((9, hy + 2, 22, hy + 13), 5)
    c.rect((8, hy, 23, hy + 3), 6)                            # 모자
    c.rect((9, hy - 1, 22, hy), 6)
    c.rect((9, hy + 5, 22, hy + 7), 6)                        # 눈가리개
    for x in (12, 13, 18, 19):
        c.pixel(x, hy + 6, 7)                                 # 눈구멍
    c.pixel(15, hy + 8, 5)
    c.pixel(16, hy + 8, 5)                                    # 코
    c.rect((11, hy + 9, 20, hy + 11), 6)                      # 입 (벌어짐)
    c.rect((12, hy + 9, 19, hy + 9), 7)                       # 윗니
    if laugh == 1:
        c.rect((13, hy + 11, 18, hy + 11), 12)                # 혀
    c.outline()
    return c


def main():
    os.makedirs(OUT, exist_ok=True)
    frames = [side_thief(i) for i in range(4)] + [grab_pose()] + [front_thief(0), front_thief(1)]
    scale = 8
    sheet = Image.new('RGBA', (SIZE * 7 * scale // 2 + 80, SIZE * scale // 2 + 20), (44, 40, 60, 255))
    x = 10
    for canvas in frames:
        big = to_rgba(canvas).resize((SIZE * scale // 2, SIZE * scale // 2), Image.NEAREST)
        sheet.alpha_composite(big, (x, 10))
        x += SIZE * scale // 2 + 8
    sheet.convert('RGB').save(os.path.join(OUT, 'frames.png'))
    print('출력:', OUT)
    return 0


if __name__ == '__main__':
    sys.exit(main())
