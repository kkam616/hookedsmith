"""레시피 화면의 낡은 종이 배경 (graphics/recipe_paper.bmp, 8bpp 64색).

종이 질감, 얼룩, 가장자리 그을림, 접힌 자국은 tools/preview_recipe_note.py의 paper_background()와 같고,
제목 밑줄(y 21)을 더해 그린다. 글자는 게임에서 스프라이트로 얹는다.
수동 실행: python -B tools/make_recipe_paper.py
"""

import os
import sys

from PIL import Image, ImageDraw

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import preview_recipe_note as note
from make_font import write_if_changed

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
COLORS = 64


def main():
    image = note.paper_background()
    ImageDraw.Draw(image).line((12, 21, note.W - 12, 21), fill=note.INK_FAINT)       # 제목 밑줄

    quantized = image.quantize(colors=COLORS, method=Image.Quantize.MAXCOVERAGE, kmeans=3, dither=Image.Dither.NONE)
    raw = quantized.getpalette()
    palette = [tuple(c & 0xF8 for c in raw[i * 3:i * 3 + 3]) for i in range(COLORS)]

    canvas = Image.new('P', (256, 256), 0)
    pixels = quantized.load()
    for y in range(note.H):
        for x in range(note.W):
            canvas.putpixel((x, y), pixels[x, y] + 1)
    canvas.putpalette([0, 0, 0] + [channel for color in palette for channel in color])

    path = os.path.join(ROOT, 'graphics', 'recipe_paper.bmp')
    canvas.save(path + '.tmp', format='BMP')
    with open(path + '.tmp', 'rb') as handle:
        write_if_changed(path, handle.read())
    os.remove(path + '.tmp')
    write_if_changed(os.path.join(ROOT, 'graphics', 'recipe_paper.json'), '{\n    "type": "regular_bg",\n    "bpp_mode": "bpp_8"\n}\n')

    canvas.convert('RGB').crop((0, 0, note.W, note.H)).resize((note.W * 3, note.H * 3), Image.NEAREST).save(os.path.join(ROOT, 'build', 'recipe_paper_preview.png'))
    print('graphics/recipe_paper.bmp 작성')
    return 0


if __name__ == '__main__':
    sys.exit(main())
