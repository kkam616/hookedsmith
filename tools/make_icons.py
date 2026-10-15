"""강화 화면 아이콘 변환기.

assets/graphics/icons/ui_icons_source.webp(큰 일러스트 시트)에서 아이콘 안쪽 그림만 잘라
(크기 - 4)px로 줄이고, 2px 테두리(원형 또는 나무 액자)를 직접 그려 투명 + 15색 4bpp 스프라이트로 만든다.
원본 테두리까지 줄이면 작은 크기에서 테두리가 그림을 먹고 뭉개지기 때문.
GBA 스프라이트는 32 또는 64 크기라, 32보다 큰 아이콘은 64x64 가운데에 그리고 나머지는 투명.
원본을 바꾸면 이 스크립트를 다시 돌린다: python -B tools/make_icons.py
"""

import os
import sys

from PIL import Image, ImageEnhance

from make_font import write_bmp_4bpp, write_if_changed

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
SOURCE = os.path.join(ROOT, 'assets', 'graphics', 'icons', 'ui_icons_source.webp')
OUT_DIR = os.path.join(ROOT, 'graphics')

BORDER = 2
TRANSPARENT = (255, 0, 255)
OUTLINE = (24, 20, 28)
RING = (88, 88, 104)
WOOD = (128, 80, 40)
CONTRAST = 1.25
SATURATION = 1.2

# 원본 시트에서 테두리 안쪽 그림 영역 (x0, y0, x1, y1), 테두리 모양, 화면에 그릴 크기(px).
# 같은 보따리가 두 개라 위쪽 것만 쓴다. 오른쪽 아래 화구 옆의 워터마크는 범위 밖.
# 크기를 바꾸면 src/main.cpp의 아이콘 좌표와 키 글자 위치도 맞춘다.
ICONS = {
    'icon_upgrade': ((146, 125, 401, 377), 'circle', 40),   # 망치 → 강화
    'icon_sell': ((507, 125, 760, 377), 'circle', 40),      # 지폐 → 판매
    'icon_storage': ((866, 128, 1118, 376), 'square', 40),  # 보따리 → 창고
    'icon_shop': ((505, 475, 759, 723), 'square', 40),      # 집 → 상점
    'icon_craft': ((862, 473, 1122, 725), 'square', 40),    # 화구 → 조합소
}


def to_gba_color(color):
    return tuple(channel & 0xF8 for channel in color)


def shrink(sheet, box, inner):
    picture = sheet.crop(box).resize((inner, inner), Image.BOX)
    picture = ImageEnhance.Contrast(picture).enhance(CONTRAST)
    return ImageEnhance.Color(picture).enhance(SATURATION)


def compose(picture, shape, size):
    """size x size RGB와 불투명 마스크를 만든다. 테두리는 바깥 1px 외곽선 + 안쪽 1px 테두리색."""
    image = Image.new('RGB', (size, size), OUTLINE)
    opaque = [[False] * size for _ in range(size)]
    center = (size - 1) / 2

    for y in range(size):
        for x in range(size):
            if shape == 'circle':
                distance = ((x - center) ** 2 + (y - center) ** 2) ** 0.5
                if distance > size / 2:
                    continue
                if distance > size / 2 - 1:
                    color = OUTLINE
                elif distance > size / 2 - BORDER:
                    color = RING
                else:
                    color = picture.getpixel((x - BORDER, y - BORDER))
            else:
                edge = min(x, y, size - 1 - x, size - 1 - y)
                if edge == 0:
                    color = OUTLINE
                elif edge < BORDER:
                    color = WOOD
                else:
                    color = picture.getpixel((x - BORDER, y - BORDER))

            image.putpixel((x, y), color)
            opaque[y][x] = True

    return image, opaque


def convert(sheet, box, shape, size):
    rgb, opaque = compose(shrink(sheet, box, size - BORDER * 2), shape, size)

    quantized = rgb.quantize(colors=15, method=Image.Quantize.MEDIANCUT, dither=Image.Dither.NONE)
    raw_palette = quantized.getpalette()
    palette = [TRANSPARENT] + [to_gba_color(tuple(raw_palette[i * 3:i * 3 + 3])) for i in range(15)]

    canvas = 32 if size <= 32 else 64
    offset = (canvas - size) // 2
    result = Image.new('P', (canvas, canvas), 0)
    quantized_pixels = quantized.load()
    for y in range(size):
        for x in range(size):
            if opaque[y][x]:
                result.putpixel((x + offset, y + offset), quantized_pixels[x, y] + 1)

    return result, palette


def main():
    sheet = Image.open(SOURCE).convert('RGB')

    for name, (box, shape, size) in ICONS.items():
        image, palette = convert(sheet, box, shape, size)
        write_if_changed(os.path.join(OUT_DIR, name + '.bmp'), write_bmp_4bpp(None, image, palette))
        write_if_changed(os.path.join(OUT_DIR, name + '.json'),
                         '{\n    "type": "sprite",\n    "height": %d\n}\n' % image.size[1])

    return 0


if __name__ == '__main__':
    sys.exit(main())
