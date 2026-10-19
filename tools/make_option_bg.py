"""OPTION 화면 배경 (수동 실행): assets/graphics/menu_bg/option_bg_source.png (작업대 위 검 설계도) -> graphics/option_bg.bmp (8bpp, 흑백 32단계)

3:2로 자르고 240x160으로 줄인 뒤 흑백으로 바꾸고, 글자가 잘 보이도록 어둡게 한다 (BRIGHTNESS).
팔레트 0번은 투명이라 비우고 1~32번에 회색 32단계를 둔다.
사용: python -B tools/make_option_bg.py
"""
import os
import sys

from PIL import Image, ImageEnhance, ImageOps

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
from make_font import write_if_changed

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
SOURCE = os.path.join(ROOT, 'assets', 'graphics', 'menu_bg', 'option_bg_source.png')
OUT = os.path.join(ROOT, 'graphics', 'option_bg.bmp')
BRIGHTNESS = 0.62
LEVELS = 32


def main():
    image = Image.open(SOURCE).convert('RGB')
    w, h = image.size
    new_h = round(w / 1.5)
    if new_h < h:
        top = (h - new_h) // 2
        image = image.crop((0, top, w, top + new_h))
    image = image.resize((240, 160), Image.LANCZOS)
    gray = ImageOps.grayscale(image)
    gray = ImageOps.autocontrast(gray, cutoff=1)
    gray = ImageEnhance.Brightness(gray).enhance(BRIGHTNESS)

    indices = Image.new('P', (240, 160), 0)
    px = indices.load()
    src = gray.load()
    for y in range(160):
        for x in range(240):
            px[x, y] = 1 + min(LEVELS - 1, round(src[x, y] / 255 * (LEVELS - 1)))

    palette = [(0, 0, 0)] + [(l * 255 // (LEVELS - 1) & 0xF8,) * 3 for l in range(LEVELS)]
    canvas = Image.new('P', (256, 256), 0)
    canvas.paste(indices, (0, 0))
    canvas.putpalette([c for color in palette for c in color])
    canvas.save(OUT + '.tmp', format='BMP')
    with open(OUT + '.tmp', 'rb') as handle:
        write_if_changed(OUT, handle.read())
    os.remove(OUT + '.tmp')
    write_if_changed(OUT.replace('.bmp', '.json'), '{\n    "type": "regular_bg",\n    "bpp_mode": "bpp_8"\n}\n'.encode())
    gray.save(os.path.join(ROOT, 'build', 'option_bg_preview.png'))
    print('ok')


if __name__ == '__main__':
    main()
