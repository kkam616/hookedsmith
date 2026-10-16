"""검 스프라이트 변환기 (빌드 전에 EXTTOOL로 실행).

assets/graphics/swords/sword_NN_name.png (32x64, 4bpp 인덱스, 투명이 0번, 16색 이하)를
Butano(grit)가 읽는 graphics/sword_NN.bmp(+json)로 바꾼다. 검마다 팔레트가 따로다.
그림은 y 0~52, x 7~24 안에 그린다 (강화 화면에서 검 끝 y 40, 맨 아래 줄이 모루 윗면과 겹침).

실패 연출용으로 한 장에 프레임 4개(세로로 쌓음)를 만든다. 그림을 바꿔도 자동으로 다시 만들어진다.
  0 = 원래 검, 1 = 금 간 검(오른쪽 위에서 왼쪽 아래로 지그재그 틈), 2 = 위쪽 조각, 3 = 아래쪽 조각
조각선은 검의 실제 그림 범위(투명이 아닌 점)에 맞춰 정한다. 부서질 때 날아가는 파편은 shard 스프라이트.
"""

import glob
import os
import re
import sys

from PIL import Image

from make_font import write_bmp_4bpp, write_if_changed

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
SOURCE_DIR = os.path.join(ROOT, 'assets', 'graphics', 'swords')
OUT_DIR = os.path.join(ROOT, 'graphics')

WIDTH = 32
HEIGHT = 64
SWORD_COUNT = 30


FRAME_COUNT = 4
JITTER = (0, 1, 0, -1)      # 금이 곧은 선이 아니라 빠직 갈라진 모양이 되도록 x마다 위아래로 흔든다


def crack_line(pixels):
    """검 그림 범위에 맞춘 조각선: x -> y. 오른쪽 위에서 왼쪽 아래로 내려간다."""
    points = [(x, y) for y in range(HEIGHT) for x in range(WIDTH) if pixels[x, y] != 0]

    if not points:
        return lambda x: HEIGHT / 2

    x0 = min(x for x, _ in points)
    x1 = max(x for x, _ in points)
    y0 = min(y for _, y in points)
    y1 = max(y for _, y in points)
    height = y1 - y0
    top = y0 + 0.25 * height        # 오른쪽 끝(검 오른쪽 가장자리)에서의 높이
    bottom = y0 + 0.65 * height     # 왼쪽 끝에서의 높이
    span = max(1, (x1 + 1) - (x0 - 1))

    def line(x):
        t = ((x1 + 1) - x) / span   # 오른쪽 0 → 왼쪽 1
        return top + (bottom - top) * t + JITTER[x % len(JITTER)]

    return line


def build_frames(image):
    """원래 그림 → 32x(64*4) 프레임 세트 (같은 팔레트)"""
    pixels = image.load()
    line = crack_line(pixels)
    sheet = Image.new('P', (WIDTH, HEIGHT * FRAME_COUNT), 0)
    sheet.putpalette(image.getpalette())
    out = sheet.load()

    for y in range(HEIGHT):
        for x in range(WIDTH):
            value = pixels[x, y]

            if value == 0:
                continue

            offset = y - line(x)
            out[x, y] = value                                   # 0: 원래 검
            out[x, HEIGHT + y] = 0 if abs(offset) < 1.0 else value      # 1: 금 간 검 (틈은 투명)
            if offset < -0.5:                                   # 2: 위쪽 조각
                out[x, HEIGHT * 2 + y] = value
            elif offset > 0.5:                                  # 3: 아래쪽 조각
                out[x, HEIGHT * 3 + y] = value

    return sheet


FX_PALETTE = [
    (0, 0, 0),
    (248, 248, 248), (208, 208, 224),       # 1·2  별 흰색
    (152, 200, 255), (64, 120, 248),        # 3·4  별 파랑
    (255, 240, 112), (255, 184, 24),        # 5·6  별 금색
    (255, 120, 96), (216, 32, 32),          # 7·8  별 빨강
    (200, 200, 208),                        # 9    파편 어두운 면
    (200, 192, 184), (232, 224, 216), (112, 96, 88),    # 10~12 가루
]

# 프레임 번호 (main.cpp의 spark_white 등과 맞춘다): 0 별 흰색, 1 별 파랑, 2 별 금색, 3 별 빨강, 4 파편, 5~7 가루
FX_FRAME_COUNT = 8


def write_fx():
    """연출용 8x8 그림 8장을 한 장으로 (별 4색, 파편, 가루 3종). 팔레트 하나만 쓰도록 합쳤다
    (강화 화면은 스프라이트 팔레트가 16개 한도에 거의 차 있다)."""
    image = Image.new('P', (8, 8 * FX_FRAME_COUNT), 0)

    # 0~3: 십자 별
    for frame in range(4):
        base = frame * 8
        center_index = 1 + frame * 2
        arm_index = 2 + frame * 2
        for i in range(8):
            for x, y in ((3, i), (4, i), (i, 3), (i, 4)):
                image.putpixel((x, base + y), arm_index)
        for x, y in ((2, 3), (2, 4), (5, 3), (5, 4), (3, 2), (4, 2), (3, 5), (4, 5), (3, 3), (4, 3), (3, 4), (4, 4)):
            image.putpixel((x, base + y), center_index)
        for x, y in ((3, 0), (4, 0), (3, 7), (4, 7), (0, 3), (0, 4), (7, 3), (7, 4)):
            image.putpixel((x, base + y), 0)

    # 4: 파편 (삼각 조각)
    shard = {(3, 1): 1, (4, 1): 1, (2, 2): 1, (3, 2): 1, (4, 2): 1, (5, 2): 9, (2, 3): 1, (3, 3): 1, (4, 3): 9,
             (5, 3): 9, (3, 4): 1, (4, 4): 9, (5, 4): 9, (4, 5): 9, (5, 5): 9}
    for (x, y), value in shard.items():
        image.putpixel((x, 4 * 8 + y), value)

    # 5~7: 가루 (큰 재, 작은 재, 어두운 재)
    dust_shapes = [
        ([(3, 3), (4, 3), (3, 4), (4, 4), (2, 3), (5, 4)], 10),
        ([(3, 3), (4, 4)], 11),
        ([(3, 3), (4, 3), (3, 4), (4, 4)], 12),
    ]
    for offset, (points, value) in enumerate(dust_shapes):
        for x, y in points:
            image.putpixel((x, (5 + offset) * 8 + y), value)

    palette = FX_PALETTE + [(0, 0, 0)] * (16 - len(FX_PALETTE))
    write_if_changed(os.path.join(OUT_DIR, 'fx.bmp'), write_bmp_4bpp(None, image, palette))
    write_if_changed(os.path.join(OUT_DIR, 'fx.json'), '{\n    "type": "sprite",\n    "height": 8\n}\n')

    # 예전에 따로 만들던 그림은 지운다
    for name in ('shard', 'spark', 'dust'):
        for extension in ('.bmp', '.json'):
            path = os.path.join(OUT_DIR, name + extension)
            if os.path.exists(path):
                os.remove(path)


def convert(path, level):
    image = Image.open(path)

    if image.size != (WIDTH, HEIGHT) or image.mode != 'P':
        raise SystemExit('%s: 32x64 인덱스 PNG여야 함 (지금 %s %s)' % (path, image.size, image.mode))

    palette_raw = image.getpalette() or []
    palette = [tuple(palette_raw[i * 3:i * 3 + 3]) for i in range(len(palette_raw) // 3)]
    palette += [(0, 0, 0)] * (16 - len(palette))

    pixels = image.load()
    used = {pixels[x, y] for y in range(HEIGHT) for x in range(WIDTH)}

    if max(used) > 15:
        raise SystemExit('%s: 팔레트 번호가 15를 넘음 (4bpp는 16색)' % path)

    # 번호는 그대로 쓰고, 색만 GBA 5비트로 맞춘다 (0번은 투명이라 색은 상관없음)
    palette = [tuple(channel & 0xF8 for channel in color) for color in palette[:16]]

    name = 'sword_%02d' % level
    sheet = build_frames(image)
    write_if_changed(os.path.join(OUT_DIR, name + '.bmp'), write_bmp_4bpp(None, sheet, palette))
    write_if_changed(os.path.join(OUT_DIR, name + '.json'), '{\n    "type": "sprite",\n    "height": %d\n}\n' % HEIGHT)


def main():
    found = {}

    for path in glob.glob(os.path.join(SOURCE_DIR, 'sword_*.png')):
        match = re.match(r'sword_(\d\d)_', os.path.basename(path))

        if match:
            found[int(match.group(1))] = path

    missing = [level for level in range(SWORD_COUNT) if level not in found]

    if missing:
        raise SystemExit('검 스프라이트가 없음: ' + ', '.join('+%d' % level for level in missing))

    for level in range(SWORD_COUNT):
        convert(found[level], level)

    write_fx()
    return 0


if __name__ == '__main__':
    sys.exit(main())
