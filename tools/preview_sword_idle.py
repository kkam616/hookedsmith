"""검 대기 모션 시안 (게임에는 아직 적용하지 않는다).

강화 화면에서 검이 가만히 모루에 꽂혀 있는 대신, 메뉴 글자가 위아래로 까딱이듯 검도 움직이는 모습을 비교한다.
 A 톡톡: 14프레임마다 2px 올랐다 내려오는 단계식 (타이틀·슬롯 목록의 글자 흔들림과 같은 리듬)
 B 둥실: 사인 곡선으로 부드럽게 위아래 ±2px (주기 약 1.6초)
 C 단계별: B와 같지만 높은 단계일수록 크고 빠르며 +17 이상은 끝에서 반짝임이 인다

출력: build/sword_idle_preview/sword_idle.gif (A·B·C를 나란히, 각각 +5 / +17 / +28 검)
수동 실행: python -B tools/preview_sword_idle.py
"""

import math
import os
import random
import sys

from PIL import Image, ImageDraw

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import preview_sell_fx as base

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
OUT = os.path.join(ROOT, 'build', 'sword_idle_preview')
SWORD_POS = (104, 40)           # 검 그림 왼쪽 위 (화면 기준): 가운데 x, y = 80 - 8 - 32
FRAMES = 192                    # 3.2초 (GIF 20ms 한 장 = 1프레임을 60fps에 가깝게 3칸에 2번씩 건너뛴다)


def offset_a(f):
    return -2 if (f // 14) % 2 else 0


def offset_b(f):
    return int(round(2 * math.sin(f * 2 * math.pi / 96)))


def offset_c(f, level):
    amplitude = 1 if level <= 5 else 2 if level <= 16 else 3
    period = 120 if level <= 5 else 96 if level <= 16 else 72
    return int(round(amplitude * math.sin(f * 2 * math.pi / period)))


def sparkle(draw, f, tip_x, tip_y):
    rng = random.Random(f // 12)
    if f % 12 < 6:
        x = tip_x + rng.randint(-6, 6)
        y = tip_y + rng.randint(-2, 10)
        size = 1 if f % 12 < 3 else 2
        draw.line((x - size, y, x + size, y), fill=(255, 245, 190))
        draw.line((x, y - size, x, y + size), fill=(255, 245, 190))


def main():
    os.makedirs(OUT, exist_ok=True)
    scene = base.base_scene()
    levels = (5, 17, 28)
    swords = [base.sword_image(level) for level in levels]

    frames = []
    for f in range(FRAMES):
        panels = []
        for index in range(3):
            image = scene.copy()
            level = levels[index]
            if index == 0:
                dy = offset_a(f)
            elif index == 1:
                dy = offset_b(f)
            else:
                dy = offset_c(f, level)
            image.alpha_composite(swords[index], (SWORD_POS[0], SWORD_POS[1] + dy))
            if index == 2 and level >= 17:
                sparkle(ImageDraw.Draw(image), f, SWORD_POS[0] + 16, SWORD_POS[1] + dy + 4)
            panels.append(image.convert('RGB').crop((40, 20, 200, 130)))
        sheet = Image.new('RGB', (160 * 3 + 20, 110), (24, 24, 30))
        for i, panel in enumerate(panels):
            sheet.paste(panel, (i * 170, 0))
        frames.append(sheet.resize((sheet.width * 3, sheet.height * 3), Image.NEAREST))

    frames[0].save(os.path.join(OUT, 'sword_idle.gif'), save_all=True, append_images=frames[1:], duration=33, loop=0,
                   optimize=False, disposal=1)
    # 스틸: 위로 올라간 순간과 내려간 순간
    still = Image.new('RGB', (frames[0].width, frames[0].height * 2), (24, 24, 30))
    still.paste(frames[0], (0, 0))
    still.paste(frames[20], (0, frames[0].height))
    still.save(os.path.join(OUT, 'stills.png'))
    print('출력:', OUT)
    return 0


if __name__ == '__main__':
    sys.exit(main())
