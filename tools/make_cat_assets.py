"""고양이 이벤트 그림을 graphics/에 쓴다 (수동 실행).

- graphics/cat.bmp (+json): 도트 고양이 32x32 x 7장 (걷기 4 + 앉아 꼬리 흔들기 3), 팔레트 8색
- graphics/cat_room.bmp (+json): 창 틀 + 어두운 방 (86x70, 화면 정가운데 위쪽), 4bpp 배경
- graphics/cat_photo.bmp (+json): 같은 자리의 창 틀 + 고양이 사진 (15색), 4bpp 배경
원본 사진은 assets/graphics/events/cat_source.webp. 그림 모양은 tools/make_cat_sprite.py·make_cat.py의 시안과 같다.

수동 실행: python -B tools/make_cat_assets.py
"""

import os
import sys

from PIL import Image, ImageDraw

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import make_cat as photo
import make_cat_sprite as cat
from make_font import write_bmp_4bpp, write_if_changed

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
GRAPHICS = os.path.join(ROOT, 'graphics')

PHOTO_BOX = (90, 30, 1110, 845)       # 얼굴 (가로:세로 = 5:4)
PHOTO_SIZE = (80, 64)
FRAME = 3                              # 창 틀 두께 (바깥 어두운 선 + 안쪽 밝은 선 + 한 칸)
WINDOW_SIZE = (PHOTO_SIZE[0] + FRAME * 2, PHOTO_SIZE[1] + FRAME * 2)      # 86x70
WINDOW_POS = (77, 41)                  # 화면 왼쪽 위 기준 (main.cpp의 cat_window 좌표와 맞춘다)
FLOOR_TOP = 46                         # 방 바닥이 시작하는 줄 (창 안쪽 기준)


def gba(color):
    return tuple(c & 0xF8 for c in color)


def write_sprite():
    frames = [cat.frame_walk_index(i) for i in range(4)] + [cat.frame_sit_index(i) for i in range(3)]
    sheet = Image.new('P', (cat.SIZE, cat.SIZE * len(frames)), 0)
    for i, frame in enumerate(frames):
        sheet.paste(frame, (0, i * cat.SIZE))

    palette = [(255, 0, 255)] + [gba(cat.PALETTE[i][:3]) for i in range(1, 8)] + [(0, 0, 0)] * 8
    write_if_changed(os.path.join(GRAPHICS, 'cat.bmp'), write_bmp_4bpp(None, sheet, palette))
    write_if_changed(os.path.join(GRAPHICS, 'cat.json'), '{\n    "type": "sprite",\n    "height": 32\n}\n')


def room_image():
    """창 안쪽(80x64): 어두운 방 + 나무 바닥. 색은 6가지 안쪽 팔레트로 줄인다"""
    interior = Image.new('RGB', PHOTO_SIZE, (34, 24, 30))
    draw = ImageDraw.Draw(interior)
    for y in range(PHOTO_SIZE[1]):
        shade = 34 + y // 3
        draw.line((0, y, PHOTO_SIZE[0], y), fill=(shade, shade - 8, shade - 4))
    draw.rectangle((0, FLOOR_TOP, PHOTO_SIZE[0], PHOTO_SIZE[1]), fill=(92, 62, 48))
    draw.line((0, FLOOR_TOP, PHOTO_SIZE[0], FLOOR_TOP), fill=(140, 100, 70))
    for x in range(-6, PHOTO_SIZE[0], 16):
        draw.line((x, FLOOR_TOP + 1, x + 8, PHOTO_SIZE[1]), fill=(70, 46, 36))
    return interior


def write_background(name, content, colors):
    """창(틀 + 내용)을 256x256 4bpp 배경의 WINDOW_POS에 놓는다. 색 0번은 투명."""
    framed = photo.framed(content)
    quantized = framed.quantize(colors=colors, method=Image.Quantize.MAXCOVERAGE, kmeans=5, dither=Image.Dither.NONE)
    raw = quantized.getpalette()
    palette = [(255, 0, 255)] + [gba(tuple(raw[i * 3:i * 3 + 3])) for i in range(colors)]
    palette += [(0, 0, 0)] * (16 - len(palette))

    canvas = Image.new('P', (256, 256), 0)
    pixels = quantized.load()
    for y in range(framed.height):
        for x in range(framed.width):
            canvas.putpixel((WINDOW_POS[0] + x, WINDOW_POS[1] + y), pixels[x, y] + 1)

    write_if_changed(os.path.join(GRAPHICS, name + '.bmp'), write_bmp_4bpp(None, canvas, palette))
    write_if_changed(os.path.join(GRAPHICS, name + '.json'), '{\n    "type": "regular_bg",\n    "bpp_mode": "bpp_4"\n}\n')


def main():
    write_sprite()
    write_background('cat_room', room_image(), 15)

    source = Image.open(photo.SOURCE).convert('RGB').crop(PHOTO_BOX)
    write_background('cat_photo', photo.to_pixel(source, PHOTO_SIZE, 13), 15)

    print('graphics/cat.bmp, cat_room.bmp, cat_photo.bmp 작성 (창 위치 %s, 안쪽 바닥 줄 %d)' % (WINDOW_POS, FLOOR_TOP))
    return 0


if __name__ == '__main__':
    sys.exit(main())
