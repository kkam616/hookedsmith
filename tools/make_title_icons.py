"""타이틀 메뉴 아이콘 변환기 (스토리·하드·옵션).

assets/graphics/title/title_menu_icons.webp(일러스트 한 장)에서 아이콘 셋을 오려 32px 스프라이트로 만든다.
배경(어두운 남색)은 가장자리에서 flood fill로 지우고, 주황 외곽선은 줄인 모양 둘레에 새로 그린다
(원본 외곽선을 그대로 줄이면 1px 아래로 뭉개지기 때문). 투명 + 15색 4bpp, 32x32 캔버스.
원본을 바꾸면 직접 실행: python -B tools/make_title_icons.py
"""

import os
import sys
from collections import deque

from PIL import Image, ImageEnhance

from make_font import write_bmp_4bpp, write_if_changed

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
SOURCE = os.path.join(ROOT, 'assets', 'graphics', 'title', 'title_menu_icons.webp')
OUT_DIR = os.path.join(ROOT, 'graphics')

TRANSPARENT = (255, 0, 255)
OUTLINE = (255, 112, 0)
SIZE = 32
CANVAS = 32

# 원본에서 아이콘이 들어 있는 사각형 (글자 라벨은 뺀다)
ICONS = {
    'title_icon_story': (95, 265, 505, 612),
    'title_icon_hard': (560, 205, 985, 612),
    'title_icon_option': (1040, 255, 1420, 622),
}


def is_orange(pixel):
    r, g, b = pixel
    return r > 190 and 60 < g < 170 and b < 80


def is_background(pixel):
    """어두운 남색(하늘·바닥 무늬). 검은 외곽선은 파랑이 빨강보다 별로 크지 않아 배경이 아니다."""
    r, g, b = pixel
    return b - r >= 14 and r < 80 and g < 100 and b < 170


def cut_out(image):
    """가장자리에서 남색 배경을 따라 퍼져 나가는 부분을 지운다. 떨어져 있는 별 같은 작은 점도 지운다(주황 불꽃은 남김)."""
    width, height = image.size
    pixels = image.load()
    outside = [[False] * width for _ in range(height)]
    queue = deque()

    for x in range(width):
        queue.append((x, 0))
        queue.append((x, height - 1))
    for y in range(height):
        queue.append((0, y))
        queue.append((width - 1, y))

    while queue:
        x, y = queue.popleft()
        if x < 0 or y < 0 or x >= width or y >= height or outside[y][x] or not is_background(pixels[x, y]):
            continue
        outside[y][x] = True
        queue.extend(((x + 1, y), (x - 1, y), (x, y + 1), (x, y - 1)))

    # 남은 덩어리 중 큰 것과 주황 위주인 것만 남긴다
    seen = [[False] * width for _ in range(height)]
    keep = [[False] * width for _ in range(height)]
    for sy in range(height):
        for sx in range(width):
            if outside[sy][sx] or seen[sy][sx]:
                continue
            component = []
            queue = deque([(sx, sy)])
            seen[sy][sx] = True
            while queue:
                x, y = queue.popleft()
                component.append((x, y))
                for nx, ny in ((x + 1, y), (x - 1, y), (x, y + 1), (x, y - 1)):
                    if 0 <= nx < width and 0 <= ny < height and not outside[ny][nx] and not seen[ny][nx]:
                        seen[ny][nx] = True
                        queue.append((nx, ny))
            orange = sum(1 for x, y in component if is_orange(pixels[x, y]))
            if len(component) > 3000 or orange * 2 > len(component):
                for x, y in component:
                    keep[y][x] = True

    mask = Image.new('L', (width, height), 0)
    mask_pixels = mask.load()
    for y in range(height):
        for x in range(width):
            if keep[y][x]:
                mask_pixels[x, y] = 255
    return mask


def convert(sheet, box):
    crop = sheet.crop(box)
    mask = cut_out(crop)

    # 마스크의 바깥 경계 상자에 맞춰 SIZE-2 안에 들어가게 줄인다 (바깥 1px 외곽선 자리)
    bounds = mask.getbbox()
    crop = crop.crop(bounds)
    mask = mask.crop(bounds)
    inner = SIZE - 4
    scale = min(inner / crop.width, inner / crop.height)
    new_size = (max(1, round(crop.width * scale)), max(1, round(crop.height * scale)))
    picture = ImageEnhance.Color(ImageEnhance.Contrast(crop.resize(new_size, Image.BOX)).enhance(1.15)).enhance(1.1)
    mask = mask.resize(new_size, Image.BOX).point(lambda v: 255 if v >= 200 else 0)

    result = Image.new('RGB', (SIZE, SIZE), TRANSPARENT)
    opaque = [[False] * SIZE for _ in range(SIZE)]
    offset_x = (SIZE - new_size[0]) // 2
    offset_y = (SIZE - new_size[1]) // 2
    src = picture.load()
    msk = mask.load()

    for y in range(new_size[1]):
        for x in range(new_size[0]):
            if msk[x, y]:
                result.putpixel((x + offset_x, y + offset_y), src[x, y])
                opaque[y + offset_y][x + offset_x] = True

    # 모양 둘레에 주황 외곽선 1px (대각선 포함 8방향)
    ring = []
    for y in range(SIZE):
        for x in range(SIZE):
            if opaque[y][x]:
                continue
            for dy in (-1, 0, 1):
                for dx in (-1, 0, 1):
                    nx, ny = x + dx, y + dy
                    if 0 <= nx < SIZE and 0 <= ny < SIZE and opaque[ny][nx]:
                        ring.append((x, y))
                        break
                else:
                    continue
                break
    for x, y in ring:
        result.putpixel((x, y), OUTLINE)
        opaque[y][x] = True

    return result, opaque


def to_gba_color(color):
    return tuple(channel & 0xF8 for channel in color)


def main():
    sheet = Image.open(SOURCE).convert('RGB')

    for name, box in ICONS.items():
        rgb, opaque = convert(sheet, box)
        quantized = rgb.quantize(colors=15, method=Image.Quantize.MAXCOVERAGE, kmeans=3, dither=Image.Dither.NONE)
        raw_palette = quantized.getpalette()
        palette = [TRANSPARENT] + [to_gba_color(tuple(raw_palette[i * 3:i * 3 + 3])) for i in range(15)]

        offset = (CANVAS - SIZE) // 2
        image = Image.new('P', (CANVAS, CANVAS), 0)
        qpx = quantized.load()
        for y in range(SIZE):
            for x in range(SIZE):
                if opaque[y][x]:
                    image.putpixel((x + offset, y + offset), qpx[x, y] + 1)

        write_if_changed(os.path.join(OUT_DIR, name + '.bmp'), write_bmp_4bpp(None, image, palette))
        write_if_changed(os.path.join(OUT_DIR, name + '.json'),
                         '{\n    "type": "sprite",\n    "height": %d\n}\n' % CANVAS)

        preview = Image.new('RGB', (CANVAS * 6, CANVAS * 6), (24, 40, 90))
        pal_flat = [c for color in palette for c in color]
        image.putpalette(pal_flat)
        rgba = image.convert('RGBA')
        for y in range(CANVAS):
            for x in range(CANVAS):
                if image.getpixel((x, y)) == 0:
                    rgba.putpixel((x, y), (0, 0, 0, 0))
        preview.paste(rgba.resize((CANVAS * 6, CANVAS * 6), Image.NEAREST), (0, 0), rgba.resize((CANVAS * 6, CANVAS * 6), Image.NEAREST))
        preview.save(os.path.join(ROOT, 'build', name + '_preview.png'))

    return 0


if __name__ == '__main__':
    sys.exit(main())
