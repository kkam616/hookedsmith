"""강화 화면 모루 스프라이트 생성기.

assets/graphics/anvil/anvil_source.webp(대장간 일러스트)에서 모루만 다각형으로 잘라
폭 ANVIL_WIDTH로 줄이고, 투명 + 15색 4bpp 64x64 스프라이트(그림은 위쪽 가운데)로 만든다.
그림 속 칼 손잡이가 모루 윗면을 가리므로 같은 줄의 왼쪽 윗면으로 먼저 덮는다.
원본을 바꾸면 이 스크립트를 다시 돌린다: python -B tools/make_anvil.py
"""

import os
import sys

from PIL import Image, ImageDraw, ImageEnhance

from make_font import write_bmp_4bpp, write_if_changed

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
SOURCE = os.path.join(ROOT, 'assets', 'graphics', 'anvil', 'anvil_source.webp')
OUT_BMP = os.path.join(ROOT, 'graphics', 'anvil.bmp')
OUT_JSON = os.path.join(ROOT, 'graphics', 'anvil.json')

CANVAS = 64
ANVIL_WIDTH = 60
BRIGHTNESS = 1.4    # 원본 아랫부분이 거의 검정이라 어두운 화면 바탕에 묻히지 않게 밝힌다
TRANSPARENT = (255, 0, 255)

# 원본 좌표: 칼(손잡이 끝이 모루 윗면에 박혀 있음)을 같은 줄 왼쪽 112px로 덮는다
SWORD_BOX = (712, 255, 823, 531)
SWORD_PATCH_SHIFT = 112

# 원본 좌표: 모루 외곽 (윗면 가운데 돌출부, 왼쪽 뿔, 오른쪽 뒤꿈치, 허리, 발)
ANVIL_OUTLINE = [
    (590, 521), (673, 521), (673, 509), (861, 509), (861, 521), (947, 521),
    (947, 541), (921, 566), (890, 591), (861, 605), (855, 640), (858, 713),
    (667, 713), (670, 640), (676, 606), (646, 598), (616, 577), (595, 556), (590, 541),
]


def main():
    source = Image.open(SOURCE).convert('RGB')

    x0, y0, x1, y1 = SWORD_BOX
    source.paste(source.crop((x0 - SWORD_PATCH_SHIFT, y0, x1 - SWORD_PATCH_SHIFT, y1)), (x0, y0))

    mask = Image.new('L', source.size, 0)
    ImageDraw.Draw(mask).polygon(ANVIL_OUTLINE, fill=255)
    box = mask.getbbox()

    rgba = source.convert('RGBA')
    rgba.putalpha(mask)
    anvil = rgba.crop(box)

    height = round(anvil.size[1] * ANVIL_WIDTH / anvil.size[0])
    small = anvil.convert('RGBa').resize((ANVIL_WIDTH, height), Image.BOX).convert('RGBA')
    pixels = small.load()

    opaque = [(x, y) for y in range(height) for x in range(ANVIL_WIDTH) if pixels[x, y][3] >= 128]
    rgb = Image.new('RGB', small.size, pixels[opaque[0]][:3])
    for x, y in opaque:
        rgb.putpixel((x, y), pixels[x, y][:3])

    rgb = ImageEnhance.Brightness(rgb).enhance(BRIGHTNESS)
    quantized = rgb.quantize(colors=15, method=Image.Quantize.MAXCOVERAGE, kmeans=5, dither=Image.Dither.NONE)
    raw_palette = quantized.getpalette()
    palette = [TRANSPARENT] + [tuple(c & 0xF8 for c in raw_palette[i * 3:i * 3 + 3]) for i in range(15)]

    result = Image.new('P', (CANVAS, CANVAS), 0)
    offset_x = (CANVAS - ANVIL_WIDTH) // 2
    for x, y in opaque:
        result.putpixel((x + offset_x, y), quantized.getpixel((x, y)) + 1)

    write_if_changed(OUT_BMP, write_bmp_4bpp(None, result, palette))
    write_if_changed(OUT_JSON, '{\n    "type": "sprite",\n    "height": 64\n}\n')
    print('anvil: %dx%d' % (ANVIL_WIDTH, height))
    return 0


if __name__ == '__main__':
    sys.exit(main())
