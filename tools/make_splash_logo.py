"""스플래시 JAY'S LOFT 로고 변환기.

assets/graphics/splash/jays_loft_logo.webp(검은 배경의 은색·금색 로고)를 SIZE 픽셀로 줄여
64x64 스프라이트 한가운데에 놓는다 (15색 + 투명, 검은 배경은 투명).
수동 실행: python -B tools/make_splash_logo.py [크기]   (기본 SIZE, 미리보기는 build/splash_preview_N.png)
"""

import os
import sys

from PIL import Image

from make_font import write_bmp_4bpp, write_if_changed

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
SOURCE = os.path.join(ROOT, 'assets', 'graphics', 'splash', 'jays_loft_logo.webp')
OUT_BMP = os.path.join(ROOT, 'graphics', 'splash_logo.bmp')
OUT_JSON = os.path.join(ROOT, 'graphics', 'splash_logo.json')

SIZE = int(sys.argv[1]) if len(sys.argv) > 1 else 50
CANVAS = 64
COLORS = 15
BACKGROUND_CUT = 40     # 이보다 어두운 점은 배경(투명)
EDGE_CUT = 64           # 줄인 뒤 이보다 어두우면 투명


def main():
    source = Image.open(SOURCE).convert('RGB')

    # 로고 바깥 상자(검은 배경 제외)로 잘라 정사각형으로 맞춘다
    box = source.convert('L').point(lambda v: 255 if v > BACKGROUND_CUT else 0).getbbox()
    logo = source.crop(box)
    side = max(logo.size)
    square = Image.new('RGB', (side, side), (0, 0, 0))
    square.paste(logo, ((side - logo.width) // 2, (side - logo.height) // 2))

    small = square.resize((SIZE, SIZE), Image.LANCZOS)
    mask = small.convert('L').point(lambda v: 255 if v > EDGE_CUT else 0)

    opaque = Image.new('RGB', (SIZE, SIZE), (0, 0, 0))
    opaque.paste(small, (0, 0), mask)
    quantized = opaque.quantize(colors=COLORS, method=Image.Quantize.MAXCOVERAGE, kmeans=3, dither=Image.Dither.NONE)
    raw = quantized.getpalette()
    palette = [(0, 0, 0)] + [tuple(channel & 0xF8 for channel in raw[i * 3:i * 3 + 3]) for i in range(COLORS)]

    image = Image.new('P', (CANVAS, CANVAS), 0)
    offset = (CANVAS - SIZE) // 2
    for y in range(SIZE):
        for x in range(SIZE):
            if mask.getpixel((x, y)):
                image.putpixel((x + offset, y + offset), quantized.getpixel((x, y)) + 1)

    write_if_changed(OUT_BMP, write_bmp_4bpp(None, image, palette))
    write_if_changed(OUT_JSON, '{\n    "type": "sprite",\n    "height": 64\n}\n')

    preview = Image.new('RGB', (CANVAS, CANVAS), (0, 0, 0))
    for y in range(CANVAS):
        for x in range(CANVAS):
            preview.putpixel((x, y), palette[image.getpixel((x, y))])
    os.makedirs(os.path.join(ROOT, 'build'), exist_ok=True)
    preview.resize((CANVAS * 8, CANVAS * 8), Image.NEAREST).save(os.path.join(ROOT, 'build', 'splash_preview_%d.png' % SIZE))
    print('logo %dx%d -> %dpx' % (logo.width, logo.height, SIZE))
    return 0


if __name__ == '__main__':
    sys.exit(main())
