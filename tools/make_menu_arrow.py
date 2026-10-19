"""메뉴의 노란 도트 화살표 (수동 실행): graphics/menu_arrow.bmp (스프라이트 8x16, 4bpp)

본문 7행(가로 최대 4칸) + 1px 검은 외곽선. 영문 대문자 높이(글자 칸 5~11행)와 같은 행(5~11)에 본문을 두어 글자 줄 가운데에 맞는다.
사용: python -B tools/make_menu_arrow.py
"""
import os
import sys

from PIL import Image

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
from make_font import write_bmp_4bpp, write_if_changed

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
OUT = os.path.join(ROOT, 'graphics')
ROWS = [1, 2, 3, 4, 3, 2, 1]
TOP = 5             # 본문 첫 행 (글자 칸 5~11행)
LEFT = 1            # 본문 왼쪽 열 (바깥 1칸이 외곽선)


def main():
    palette = [(0, 0, 0), (248, 208, 96), (248, 240, 168), (0, 0, 0)] + [(0, 0, 0)] * 12
    image = Image.new('P', (8, 16), 0)
    px = image.load()
    body = {}
    for r, w in enumerate(ROWS):
        for x in range(w):
            body[(LEFT + x, TOP + r)] = 2 if (r < 3 and x == 0) else 1
    for (x, y), value in body.items():
        px[x, y] = value
    for (x, y) in body:
        for dx in (-1, 0, 1):
            for dy in (-1, 0, 1):
                if (x + dx, y + dy) not in body and px[x + dx, y + dy] == 0:
                    px[x + dx, y + dy] = 3
    write_if_changed(os.path.join(OUT, 'menu_arrow.bmp'), write_bmp_4bpp(None, image, palette))
    write_if_changed(os.path.join(OUT, 'menu_arrow.json'), '{\n    "type": "sprite",\n    "height": 16\n}\n'.encode())
    print('ok')


if __name__ == '__main__':
    main()
