"""강화 화면 배경(박스 테두리) 생성기. 박스 위치를 바꾸면 src/main.cpp의 글자 좌표도 같이 맞춘다."""

import os

from PIL import Image

from make_font import write_bmp_4bpp, write_if_changed

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))

PALETTE = [
    (32, 24, 24),     # 0 투명 (backdrop 색이 보인다)
    (200, 188, 168),  # 1 테두리
    (56, 44, 40),     # 2 박스 안
] + [(0, 0, 0)] * 13

# 화면 좌표 (x0, y0, x1, y1), 양 끝 포함
BOXES = [
    (4, 4, 51, 47),       # 창고 (L)
    (188, 4, 235, 47),    # 상점 (R)
    (196, 82, 229, 113),  # 판매 (B)
    (160, 112, 193, 143), # 강화 (A)
]


def main():
    image = Image.new('P', (256, 256), 0)

    for x0, y0, x1, y1 in BOXES:
        for y in range(y0, y1 + 1):
            for x in range(x0, x1 + 1):
                edge = x in (x0, x1) or y in (y0, y1)
                image.putpixel((x, y), 1 if edge else 2)

    write_if_changed(os.path.join(ROOT, 'graphics', 'game_frame.bmp'), write_bmp_4bpp(None, image, PALETTE))
    write_if_changed(os.path.join(ROOT, 'graphics', 'game_frame.json'),
                     '{\n    "type": "regular_bg",\n    "bpp_mode": "bpp_4_manual"\n}\n')


if __name__ == '__main__':
    main()
