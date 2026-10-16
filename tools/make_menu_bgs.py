"""목록 화면(상점·화구·창고·잡화) 배경 변환기.

assets/graphics/menu_bg/menu_backgrounds_source.png(2x2: 좌상 상점, 우상 화구, 좌하 창고, 우하 잡화)를
칸마다 잘라 240x160 흑백(15단계 회색)으로 줄이고 어둡게 만든다. 글자가 많이 올라가는 화면이라 밝기를 낮추고,
위(제목)·아래(안내 줄)는 더 어둡게 눌러 둔다. 4bpp 배경(팔레트 0번은 안 쓰는 검정).
수동 실행 (빌드 때 안 돔): python -B tools/make_menu_bgs.py
"""

import os
import sys

from PIL import Image

from make_font import write_bmp_4bpp, write_if_changed

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
SOURCE = os.path.join(ROOT, 'assets', 'graphics', 'menu_bg', 'menu_backgrounds_source.png')
OUT_DIR = os.path.join(ROOT, 'graphics')
PREVIEW_DIR = os.path.join(ROOT, 'build', 'menu_bg_preview')

WIDTH = 240
HEIGHT = 160
LEVELS = 15             # 회색 단계 (팔레트 1~15)
BRIGHTNESS = 0.72       # 전체 밝기
EDGE_DARKEN = 0.30      # 제목·안내 줄 자리를 더 눌러 주는 세기 (0~1)
EDGE_ROWS = 24          # 위·아래에서 몇 줄까지 눌러 주는지

# 2x2 칸: (이름, 열, 행). 흰 구분선은 x 759~765, y 514~518
TILES = [('shop', 0, 0), ('furnace', 1, 0), ('storage', 0, 1), ('bag', 1, 1)]
COLUMN_RANGES = [(0, 759), (766, 1523)]
ROW_RANGES = [(0, 514), (519, 1033)]


def crop_tile(sheet, column, row):
    x0, x1 = COLUMN_RANGES[column]
    y0, y1 = ROW_RANGES[row]
    tile = sheet.crop((x0, y0, x1, y1))
    target_height = round(tile.width * HEIGHT / WIDTH)      # 3:2로 맞춘다 (위아래를 조금 자름)
    top = (tile.height - target_height) // 2
    return tile.crop((0, top, tile.width, top + target_height))


def to_gray(tile):
    small = tile.resize((WIDTH, HEIGHT), Image.BOX).convert('L')
    pixels = small.load()
    for y in range(HEIGHT):
        edge = max(0, EDGE_ROWS - y, y - (HEIGHT - 1 - EDGE_ROWS)) / EDGE_ROWS
        factor = BRIGHTNESS * (1 - EDGE_DARKEN * edge)
        for x in range(WIDTH):
            pixels[x, y] = round(pixels[x, y] * factor)
    return small


def main():
    sheet = Image.open(SOURCE).convert('RGB')
    os.makedirs(PREVIEW_DIR, exist_ok=True)

    for name, column, row in TILES:
        gray = to_gray(crop_tile(sheet, column, row))
        top_value = max(1, gray.getextrema()[1])

        # 가장 밝은 값을 15단계 끝으로 늘려 단계를 아껴 쓴다
        indexed = Image.new('P', (256, 256), 0)
        for y in range(HEIGHT):
            for x in range(WIDTH):
                level = 1 + round(gray.getpixel((x, y)) / top_value * (LEVELS - 1))
                indexed.putpixel((x, y), level)

        palette = [(0, 0, 0)]
        for level in range(LEVELS):
            value = round(level / (LEVELS - 1) * top_value) & 0xF8
            palette.append((value, value, value))

        out_bmp = os.path.join(OUT_DIR, 'menu_bg_%s.bmp' % name)
        out_json = os.path.join(OUT_DIR, 'menu_bg_%s.json' % name)
        write_if_changed(out_bmp, write_bmp_4bpp(None, indexed, palette))
        write_if_changed(out_json, '{\n    "type": "regular_bg",\n    "bpp_mode": "bpp_4"\n}\n')

        preview = Image.new('RGB', (WIDTH, HEIGHT))
        for y in range(HEIGHT):
            for x in range(WIDTH):
                preview.putpixel((x, y), palette[indexed.getpixel((x, y))])
        preview.resize((WIDTH * 3, HEIGHT * 3), Image.NEAREST).save(os.path.join(PREVIEW_DIR, name + '.png'))
        print('%-8s 최대 밝기 %d' % (name, top_value))

    return 0


if __name__ == '__main__':
    sys.exit(main())
