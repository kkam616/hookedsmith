"""컷씬 그림 변환기 (빌드 전에 EXTTOOL로 실행).

assets/graphics/cutscene/*.bmp (확장자는 bmp지만 PNG여도 됨, 240x112, 16색 이하)를
Butano(grit)가 읽는 4bpp 인덱스 BMP(256x256, 그림은 왼쪽 위)로 바꿔 graphics/에 쓴다.
팔레트 0번은 배경에서 투명이므로 가장 많이 쓰인 색을 0번에 두고, 게임에서 backdrop 색을 그 색으로 맞춘다.

텍스트 박스(graphics/cutscene_textbox.bmp)도 여기서 만든다: 아래 48줄, 맨 윗줄이 구분선.
"""

import glob
import os
import sys

from PIL import Image

from make_font import write_bmp_4bpp, write_if_changed

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
SOURCE_DIR = os.path.join(ROOT, 'assets', 'graphics', 'cutscene')
OUT_DIR = os.path.join(ROOT, 'graphics')

SCENE_WIDTH = 240
SCENE_HEIGHT = 112
CANVAS = 256

TEXTBOX_TOP = 112
TEXTBOX_PALETTE = [
    (0, 0, 0),         # 0 투명
    (24, 20, 22),      # 1 박스 배경
    (200, 188, 168),   # 2 구분선
] + [(0, 0, 0)] * 13

BG_JSON = '{\n    "type": "regular_bg",\n    "bpp_mode": "bpp_4_manual"\n}\n'


def convert_scene(path):
    name = os.path.splitext(os.path.basename(path))[0]
    image = Image.open(path).convert('RGBA')

    if image.size != (SCENE_WIDTH, SCENE_HEIGHT):
        raise SystemExit('%s: %dx%d (240x112이어야 함)' % (path, image.size[0], image.size[1]))

    pixels = image.load()
    counts = {}
    for y in range(SCENE_HEIGHT):
        for x in range(SCENE_WIDTH):
            r, g, b, a = pixels[x, y]
            if a != 255:
                raise SystemExit('%s: 반투명 픽셀이 있음' % path)
            counts[(r, g, b)] = counts.get((r, g, b), 0) + 1

    if len(counts) > 16:
        raise SystemExit('%s: %d색 (16색 이하여야 함)' % (path, len(counts)))

    colors = sorted(counts, key=lambda c: -counts[c])
    index = {c: i for i, c in enumerate(colors)}
    palette = colors + [(0, 0, 0)] * (16 - len(colors))

    canvas = Image.new('P', (CANVAS, CANVAS), 0)
    for y in range(SCENE_HEIGHT):
        for x in range(SCENE_WIDTH):
            canvas.putpixel((x, y), index[pixels[x, y][:3]])

    changed = write_if_changed(os.path.join(OUT_DIR, name + '.bmp'), write_bmp_4bpp(None, canvas, palette))
    changed |= write_if_changed(os.path.join(OUT_DIR, name + '.json'), BG_JSON)
    return changed


def make_textbox():
    canvas = Image.new('P', (CANVAS, CANVAS), 0)
    for y in range(TEXTBOX_TOP, 160):
        for x in range(SCENE_WIDTH):
            canvas.putpixel((x, y), 2 if y == TEXTBOX_TOP else 1)

    changed = write_if_changed(os.path.join(OUT_DIR, 'cutscene_textbox.bmp'),
                               write_bmp_4bpp(None, canvas, TEXTBOX_PALETTE))
    changed |= write_if_changed(os.path.join(OUT_DIR, 'cutscene_textbox.json'), BG_JSON)
    return changed


def main():
    changed = make_textbox()
    sources = sorted(glob.glob(os.path.join(SOURCE_DIR, '*.bmp')))

    for path in sources:
        changed |= convert_scene(path)

    if changed:
        print('cutscenes: %d scenes' % len(sources))
    return 0


if __name__ == '__main__':
    sys.exit(main())
