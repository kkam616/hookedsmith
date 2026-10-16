"""검 아이콘 변환기 (시안 단계: 미리보기만 만들고 게임에는 적용하지 않는다).

assets/graphics/swords/sword_NN_*.png(32x64 세로 검)를 45도 눕혀(끝이 오른쪽 위) 14x14 안에 맞춰 줄이고
16x16 칸 가운데에 놓는다. 검마다 자기 15색 팔레트를 쓴다. 창고 목록의 "+N 이름" 앞에 붙일 용도.

수동 실행: python -B tools/make_sword_icons.py
출력: build/sword_icons_preview/ (미리보기). graphics/에는 아직 아무것도 쓰지 않는다.
"""

import glob
import os
import re
import sys

from PIL import Image, ImageDraw, ImageFont

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
SOURCE_DIR = os.path.join(ROOT, 'assets', 'graphics', 'swords')
OUT_DIR = os.path.join(ROOT, 'build', 'sword_icons_preview')
NAMES_HEADER = os.path.join(ROOT, 'include', 'economy_data.h')

ART = 14
CANVAS = 16
COLORS = 15
ROTATE = -45        # 시계 방향 45도: 위를 향하던 검 끝이 오른쪽 위로 간다
UPSCALE = 4         # 돌릴 때 번지지 않게 먼저 키운다


def load_names():
    with open(NAMES_HEADER, encoding='utf-8') as handle:
        text = handle.read()
    body = text.split('sword_names[level_count] = {')[1].split('};')[0]
    return re.findall(r'"([^"]+)"', body)


def load_sword(path):
    image = Image.open(path)
    palette_raw = image.getpalette()
    palette = [tuple(palette_raw[i * 3:i * 3 + 3]) for i in range(len(palette_raw) // 3)]
    rgba = Image.new('RGBA', image.size, (0, 0, 0, 0))
    for y in range(image.height):
        for x in range(image.width):
            index = image.getpixel((x, y))
            if index != 0:
                rgba.putpixel((x, y), palette[index] + (255,))
    return rgba


def make_icon(sword):
    big = sword.resize((sword.width * UPSCALE, sword.height * UPSCALE), Image.NEAREST)
    rotated = big.rotate(ROTATE, resample=Image.BICUBIC, expand=True)
    rotated = rotated.crop(rotated.getbbox())
    scale = ART / max(rotated.size)
    size = (max(1, round(rotated.width * scale)), max(1, round(rotated.height * scale)))
    small = rotated.resize(size, Image.LANCZOS)

    # 14x14 안 가운데에 놓고, 반투명한 가장자리는 잘라 낸다
    placed = Image.new('RGBA', (ART, ART), (0, 0, 0, 0))
    placed.paste(small, ((ART - size[0]) // 2, (ART - size[1]) // 2))
    mask = placed.getchannel('A').point(lambda v: 255 if v >= 110 else 0)

    opaque = Image.new('RGB', (ART, ART), (0, 0, 0))
    opaque.paste(placed.convert('RGB'), (0, 0), mask)
    quantized = opaque.quantize(colors=COLORS, method=Image.Quantize.MAXCOVERAGE, kmeans=3, dither=Image.Dither.NONE)
    rgb = quantized.convert('RGB')

    canvas = Image.new('RGBA', (CANVAS, CANVAS), (0, 0, 0, 0))
    offset = (CANVAS - ART) // 2
    for y in range(ART):
        for x in range(ART):
            if mask.getpixel((x, y)):
                color = tuple(channel & 0xF8 for channel in rgb.getpixel((x, y)))
                canvas.putpixel((x + offset, y + offset), color + (255,))
    return canvas


def main():
    names = load_names()
    paths = {}
    for path in glob.glob(os.path.join(SOURCE_DIR, 'sword_*.png')):
        match = re.match(r'sword_(\d\d)_', os.path.basename(path))
        if match:
            paths[int(match.group(1))] = path

    os.makedirs(OUT_DIR, exist_ok=True)
    icons = [make_icon(load_sword(paths[level])) for level in range(len(names))]
    for level, icon in enumerate(icons):
        icon.save(os.path.join(OUT_DIR, 'sword_%02d_icon.png' % level))

    if '--write' in sys.argv:
        from icon_export import export_icon
        for level, icon in enumerate(icons):
            export_icon('ico_sword_%02d' % level, icon)
        print('graphics/ico_sword_*.bmp 작성:', len(icons))

    # 미리보기 1: 30개 시트 (6열 x 5행, 8배)
    cols, scale = 6, 6
    cell = CANVAS * scale + 10
    sheet = Image.new('RGB', (cols * cell + 10, 5 * cell + 10), (0, 0, 0))
    for level, icon in enumerate(icons):
        big = icon.resize((CANVAS * scale, CANVAS * scale), Image.NEAREST)
        sheet.paste(big, (10 + (level % cols) * cell, 10 + (level // cols) * cell), big)
    sheet.save(os.path.join(OUT_DIR, 'preview_sheet.png'))

    # 미리보기 2: 창고 목록 흉내 (줄 간격 14, 8줄, 240x160의 4배)
    try:
        font = ImageFont.truetype(r'C:\Windows\Fonts\gulim.ttc', 12, index=0)
    except OSError:
        font = ImageFont.load_default()

    mock = Image.new('RGB', (240, 160), (28, 20, 22))
    draw = ImageDraw.Draw(mock)
    draw.text((10, 4), '창고', font=font, fill=(255, 255, 255))
    draw.text((140, 4), '지금 +0', font=font, fill=(255, 255, 255))
    for row, level in enumerate((19, 21, 21, 22, 13, 16, 5, 27)):
        y = 20 + row * 14 - 2
        mock.paste(icons[level], (14, y), icons[level])
        draw.text((32, y + 2), '+%d %s' % (level, names[level]), font=font, fill=(255, 255, 255))
    mock.resize((960, 640), Image.NEAREST).save(os.path.join(OUT_DIR, 'preview_storage_list_4x.png'))

    print('icons:', len(icons), '->', OUT_DIR)
    return 0


if __name__ == '__main__':
    sys.exit(main())
