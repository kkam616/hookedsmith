"""목록 화면의 "지금 살 수 있다 / 만들 수 있다" 표시(초록불) 8x8 그림을 만든다.

수동 실행: python -B tools/make_ready_icon.py
출력: graphics/ico_ready.bmp (+ .json)
"""

import os
import sys

from PIL import Image

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
from make_font import write_bmp_4bpp, write_if_changed

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))

# 0 투명, 1 어두운 테두리, 2 초록, 3 밝은 초록(반짝임)
ROWS = [
    '..1111..',
    '.122221.',
    '12322221',
    '12322221',
    '12222221',
    '12222221',
    '.122221.',
    '..1111..',
]
PALETTE = [(255, 0, 255), (16, 64, 24), (40, 200, 64), (176, 255, 176)] + [(0, 0, 0)] * 12


def main():
    image = Image.new('P', (8, 8), 0)
    for y, row in enumerate(ROWS):
        for x, ch in enumerate(row):
            image.putpixel((x, y), 0 if ch == '.' else int(ch))
    palette = [tuple(c & 0xF8 for c in color) for color in PALETTE]
    write_if_changed(os.path.join(ROOT, 'graphics', 'ico_ready.bmp'), write_bmp_4bpp(None, image, palette))
    write_if_changed(os.path.join(ROOT, 'graphics', 'ico_ready.json'), '{\n    "type": "sprite"\n}\n')


main()
