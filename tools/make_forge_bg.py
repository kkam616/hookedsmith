"""강화 화면 배경 변환기.

assets/graphics/forge/forge_bg_source.webp(대장간 일러스트)를 240x160으로 줄여 8bpp 배경으로 만든다.
원본 한가운데의 검·모루는 스프라이트가 따로 올라오므로 바닥 무늬로 덮고(그루터기 아래쪽만 남김),
글자가 올라갈 위쪽 가운데와 아래쪽 가운데는 어둡게 눌러 읽기 쉽게 한다.
수동 실행 (빌드 때 안 돔): python -B tools/make_forge_bg.py
"""

import os
import sys

from PIL import Image, ImageEnhance

from make_font import write_if_changed

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
SOURCE = os.path.join(ROOT, 'assets', 'graphics', 'forge', 'forge_bg_source.webp')
OUT_BMP = os.path.join(ROOT, 'graphics', 'forge_bg.bmp')
OUT_JSON = os.path.join(ROOT, 'graphics', 'forge_bg.json')

WIDTH = 240
HEIGHT = 160
COLORS = 64
BRIGHTNESS = 0.85

# 덮을 영역: 원본 검이 서 있던 가는 띠와 모루 자리 (240x160 기준). y 113 아래 그루터기 옆면은 남긴다.
SWORD_X0, SWORD_X1, SWORD_Y0 = 106, 134, 36
ANVIL_X0, ANVIL_X1, ANVIL_Y0 = 88, 152, 80
CLEAR_Y1 = 113
BLEND_MARGIN = 3     # 덮을 띠 양옆에서 평균을 낼 픽셀 수 (원본 무늬는 바닥 돌 한 가지 톤이라 이걸로 충분)

# 글자 자리 어둡게: (x0, y0, x1, y1, 세기). 세로로 가장자리에서 서서히 사라진다.
SHADES = [
    (52, 0, 188, 44, 0.55),     # 이름·성공률·메시지
    (76, 124, 156, 160, 0.45),  # 강화비·판매가
]


def crop_to_ratio(image):
    width, height = image.size
    target_height = round(width * HEIGHT / WIDTH)
    top = (height - target_height) // 2
    return image.crop((0, top, width, top + target_height))


def cover_center(image):
    """검·모루 자리를 양옆 픽셀을 이어 붙인 가로 그라데이션으로 덮는다."""
    pixels = image.load()

    def edge_color(x_start, step, y):
        samples = [pixels[x_start + step * i, y] for i in range(BLEND_MARGIN)]
        return tuple(sum(channel) / BLEND_MARGIN for channel in zip(*samples))

    for y in range(SWORD_Y0, CLEAR_Y1):
        x0, x1 = (SWORD_X0, SWORD_X1) if y < ANVIL_Y0 else (ANVIL_X0, ANVIL_X1)
        left = edge_color(x0 - 1, -1, y)
        right = edge_color(x1, 1, y)
        for x in range(x0, x1):
            t = (x - x0 + 0.5) / (x1 - x0)
            pixels[x, y] = tuple(round(left[c] * (1 - t) + right[c] * t) for c in range(3))


def darken(image):
    pixels = image.load()
    for x0, y0, x1, y1, strength in SHADES:
        for y in range(y0, y1):
            for x in range(x0, x1):
                edge_x = min(x - x0, x1 - 1 - x) / 16
                edge_y = min(y - y0, y1 - 1 - y) / 20 if y0 > 0 and y1 < HEIGHT else 1
                fade = min(1, edge_x, edge_y) * strength
                r, g, b = pixels[x, y]
                pixels[x, y] = (round(r * (1 - fade)), round(g * (1 - fade)), round(b * (1 - fade)))


def to_gba_color(color):
    return tuple(channel & 0xF8 for channel in color)


def main():
    source = Image.open(SOURCE).convert('RGB')
    image = crop_to_ratio(source).resize((WIDTH, HEIGHT), Image.BOX)
    image = ImageEnhance.Brightness(image).enhance(BRIGHTNESS)
    cover_center(image)
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

    preview = canvas.convert('RGB').crop((0, 0, WIDTH, HEIGHT)).resize((WIDTH * 4, HEIGHT * 4), Image.NEAREST)
    preview.save(os.path.join(ROOT, 'build', 'forge_bg_preview.png'))
    return 0


if __name__ == '__main__':
    sys.exit(main())
