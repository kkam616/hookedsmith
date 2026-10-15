"""임시 검 스프라이트(32x64) 생성기. 진짜 도트 그래픽이 생기면 graphics/sword.bmp를 교체하고 이 파일은 지운다."""

import os

from PIL import Image

from make_font import write_bmp_4bpp, write_if_changed

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))

PALETTE = [
    (255, 0, 255),    # 0 투명
    (24, 20, 28),     # 1 외곽선
    (236, 240, 248),  # 2 칼날 밝은 면
    (168, 180, 196),  # 3 칼날 중간
    (104, 112, 132),  # 4 칼날 어두운 면
    (232, 184, 72),   # 5 가드 밝은 금색
    (160, 108, 40),   # 6 가드 어두운 금색
    (124, 72, 40),    # 7 손잡이
    (76, 44, 28),     # 8 손잡이 어두운 색
] + [(0, 0, 0)] * 7


def main():
    image = Image.new('P', (32, 64), 0)

    def fill(x0, y0, x1, y1, color):
        for y in range(y0, y1 + 1):
            for x in range(x0, x1 + 1):
                image.putpixel((x, y), color)

    # 칼끝 (y 1~6에서 점점 넓어짐)
    for y in range(1, 7):
        half = (y + 1) // 2
        fill(16 - half, y, 15 + half, y, 3)
    # 칼날
    fill(13, 7, 18, 35, 3)
    fill(13, 7, 14, 35, 2)
    fill(17, 7, 18, 35, 4)
    fill(15, 4, 16, 35, 2)
    # 가드
    fill(8, 36, 23, 39, 5)
    fill(8, 39, 23, 39, 6)
    fill(8, 36, 9, 39, 6)
    # 손잡이
    fill(14, 40, 17, 47, 7)
    for y in range(41, 47, 2):
        fill(14, y, 17, y, 8)
    # 폼멜 (맨 아랫줄 51이 모루 윗면에 닿는다)
    fill(12, 48, 19, 51, 5)
    fill(12, 51, 19, 51, 6)

    # 외곽선
    filled = {(x, y) for y in range(64) for x in range(32) if image.getpixel((x, y)) != 0}
    for x, y in list(filled):
        for dx, dy in ((1, 0), (-1, 0), (0, 1), (0, -1)):
            nx, ny = x + dx, y + dy
            if 0 <= nx < 32 and 0 <= ny < 64 and (nx, ny) not in filled:
                image.putpixel((nx, ny), 1)

    write_if_changed(os.path.join(ROOT, 'graphics', 'sword.bmp'), write_bmp_4bpp(None, image, PALETTE))
    write_if_changed(os.path.join(ROOT, 'graphics', 'sword.json'), '{\n    "type": "sprite",\n    "height": 64\n}\n')


if __name__ == '__main__':
    main()
