"""게임 오버 배경 변환기.

assets/graphics/gameover/gameover_source.jpg(대장간 앞에 무릎 꿇은 스미스 일러스트)를 240x160으로 줄여
8bpp 배경으로 만든다. 아래쪽 가운데는 "아무 버튼이나 누르세요" 글자가 올라가므로 어둡게 눌러 둔다.
수동 실행 (빌드 때 안 돔): python -B tools/make_gameover.py
"""

import os
import sys

from PIL import Image

from make_font import write_if_changed

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
SOURCE = os.path.join(ROOT, 'assets', 'graphics', 'gameover', 'gameover_source.jpg')
OUT_BMP = os.path.join(ROOT, 'graphics', 'gameover_bg.bmp')
OUT_JSON = os.path.join(ROOT, 'graphics', 'gameover_bg.json')

WIDTH = 240
HEIGHT = 160
COLORS = 128

# 글자 자리 어둡게: (x0, y0, x1, y1, 세기). 세로로 가장자리에서 서서히 사라진다.
SHADES = [
    (36, 124, 204, 160, 0.6),   # 아무 버튼이나 누르세요
]


# 원본 오른쪽 아래의 생성 도구 별 모양 표시는 바로 아래 바닥 무늬로 덮는다 (원본 1264x848 기준)
WATERMARK_BOX = (1105, 700, 1185, 770)
WATERMARK_FROM_Y = 778


def remove_watermark(image):
    x0, y0, x1, y1 = WATERMARK_BOX
    patch = image.crop((x0, WATERMARK_FROM_Y, x1, WATERMARK_FROM_Y + (y1 - y0)))
    image.paste(patch, (x0, y0))


def crop_to_ratio(image):
    width, height = image.size
    target_height = round(width * HEIGHT / WIDTH)
    top = (height - target_height) // 2
    return image.crop((0, top, width, top + target_height))


def darken(image):
    pixels = image.load()
    for x0, y0, x1, y1, strength in SHADES:
        for y in range(y0, y1):
            for x in range(x0, x1):
                edge_x = min(x - x0, x1 - 1 - x) / 16
                edge_y = (y - y0) / 14 if y1 >= HEIGHT else min(y - y0, y1 - 1 - y) / 14
                fade = min(1, edge_x, edge_y) * strength
                r, g, b = pixels[x, y]
                pixels[x, y] = (round(r * (1 - fade)), round(g * (1 - fade)), round(b * (1 - fade)))


def to_gba_color(color):
    return tuple(channel & 0xF8 for channel in color)


def main():
    source = Image.open(SOURCE).convert('RGB')
    remove_watermark(source)
    image = crop_to_ratio(source).resize((WIDTH, HEIGHT), Image.BOX)
    darken(image)

    quantized = image.quantize(colors=COLORS, method=Image.Quantize.MAXCOVERAGE, kmeans=3,
                               dither=Image.Dither.NONE)
    raw = quantized.getpalette()
    palette = [to_gba_color(tuple(raw[i * 3:i * 3 + 3])) for i in range(COLORS)]

    # 256x256 8bpp, 그림은 왼쪽 위. 팔레트 0번은 배경에서 투명 취급되므로 안 쓰는 검정을 0번에 둔다.
    canvas = Image.new('P', (256, 256), 0)
    quantized_pixels = quantized.load()
    for y in range(HEIGHT):
        for x in range(WIDTH):
            canvas.putpixel((x, y), quantized_pixels[x, y] + 1)

    canvas.putpalette([0, 0, 0] + [channel for color in palette for channel in color])
    canvas.save(OUT_BMP + '.tmp', format='BMP')
    with open(OUT_BMP + '.tmp', 'rb') as handle:
        write_if_changed(OUT_BMP, handle.read())
    os.remove(OUT_BMP + '.tmp')

    write_if_changed(OUT_JSON, '{\n    "type": "regular_bg",\n    "bpp_mode": "bpp_8"\n}\n')

    os.makedirs(os.path.join(ROOT, 'build'), exist_ok=True)
    preview = canvas.convert('RGB').crop((0, 0, WIDTH, HEIGHT)).resize((WIDTH * 4, HEIGHT * 4), Image.NEAREST)
    preview.save(os.path.join(ROOT, 'build', 'gameover_preview.png'))
    return 0


if __name__ == '__main__':
    sys.exit(main())
