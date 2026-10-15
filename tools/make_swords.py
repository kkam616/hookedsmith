"""검 스프라이트 변환기 (빌드 전에 EXTTOOL로 실행).

assets/graphics/swords/sword_NN_name.png (32x64, 4bpp 인덱스, 투명이 0번, 16색 이하)를
Butano(grit)가 읽는 graphics/sword_NN.bmp(+json)로 바꾼다. 검마다 팔레트가 따로다.
그림은 y 0~52, x 7~24 안에 그린다 (강화 화면에서 검 끝 y 40, 맨 아래 줄이 모루 윗면과 겹침).
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
    write_if_changed(os.path.join(OUT_DIR, name + '.bmp'), write_bmp_4bpp(None, image, palette))
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

    return 0


if __name__ == '__main__':
    sys.exit(main())
