"""수상한 상인 이벤트 그림을 graphics/에 쓴다 (수동 실행). 도둑 이벤트(make_thief_assets.py)와 같은 틀.

- graphics/merchant.bmp (+json): 도트 상인 32x32 x 6장 (웅크려 살금살금 걷기 4 + 좌우 살피는 모습 1 + 금화 주머니를 내미는 모습 1), 팔레트 15색
  (지금 도트는 tools/preview_merchant_motion2.py의 임시 그림. 실제 도트가 나오면 같은 규격으로 바꿔 끼운다)
- graphics/merchant_room.bmp / merchant_room_coins.bmp: 노을 방 창(86x70). 받침대가 빈 것 / 금화가 쌓인 것
- graphics/merchant_photo.bmp: 같은 자리의 창 틀 + 상인 사진 (원본 assets/graphics/events/merchant_photo_source.webp, 80x64)
창 위치·틀은 고양이·도둑 이벤트와 같다 (make_cat_assets.write_background).

수동 실행: python -B tools/make_merchant_assets.py
"""

import os
import sys

from PIL import Image, ImageDraw

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import make_cat as photo
import make_cat_assets as cat_assets
import preview_merchant_motion2 as motion
from make_font import write_bmp_4bpp, write_if_changed

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
GRAPHICS = os.path.join(ROOT, 'graphics')
SOURCE = os.path.join(ROOT, 'assets', 'graphics', 'events', 'merchant_photo_source.webp')
INTERIOR = (80, 64)
STAND_X, STAND_Y = 62, 39          # 받침대 위치 (창 안쪽 80x64 기준, 도둑 이벤트와 같음)


def write_sprite():
    frames = [motion.merchant('walk', i, crouch=2) for i in range(4)] + [motion.merchant('look'), motion.merchant('give')]
    sheet = Image.new('RGBA', (32, 32 * len(frames)), (0, 0, 0, 0))
    for i, frame in enumerate(frames):
        sheet.alpha_composite(frame, (0, i * 32))

    flat = Image.new('RGB', sheet.size, (255, 0, 255))
    flat.paste(sheet, (0, 0), sheet)
    quant = flat.quantize(colors=15, method=Image.Quantize.MAXCOVERAGE, kmeans=5, dither=Image.Dither.NONE)
    raw = quant.getpalette()
    palette = [(255, 0, 255)] + [cat_assets.gba(tuple(raw[i * 3:i * 3 + 3])) for i in range(15)]
    indexed = Image.new('P', sheet.size, 0)
    for y in range(sheet.height):
        for x in range(sheet.width):
            if sheet.getpixel((x, y))[3] >= 128:
                indexed.putpixel((x, y), quant.getpixel((x, y)) + 1)
    write_if_changed(os.path.join(GRAPHICS, 'merchant.bmp'), write_bmp_4bpp(None, indexed, palette))
    write_if_changed(os.path.join(GRAPHICS, 'merchant.json'), '{\n    "type": "sprite",\n    "height": 32\n}\n')


def room(kind):
    """창 안(80x64): 노을 하늘, 지는 해, 나무 바닥, 오른쪽 받침대 (kind: empty / coins)"""
    image = Image.new('RGB', INTERIOR)
    draw = ImageDraw.Draw(image)
    floor_top = 46
    for y in range(floor_top):
        t = y / floor_top
        draw.line((0, y, INTERIOR[0], y), fill=(int(70 + 170 * t), int(36 + 90 * t), int(104 - 36 * t)))
    draw.ellipse((6, 22, 24, 40), fill=(255, 214, 120))
    draw.rectangle((0, floor_top, INTERIOR[0], INTERIOR[1]), fill=(70, 44, 38))
    draw.line((0, floor_top, INTERIOR[0], floor_top), fill=(120, 76, 58))
    for x in range(-6, INTERIOR[0], 16):
        draw.line((x, floor_top + 1, x + 8, INTERIOR[1]), fill=(52, 32, 28))
    draw.rectangle((STAND_X, STAND_Y + 3, STAND_X + 11, STAND_Y + 6), fill=(120, 80, 44))
    draw.rectangle((STAND_X + 1, STAND_Y + 6, STAND_X + 10, STAND_Y + 8), fill=(84, 56, 32))

    if kind == 'coins':
        for cx, cy in ((2, 0), (6, -1), (9, 0), (4, -4), (8, -4), (6, -7)):
            draw.ellipse((STAND_X + cx, STAND_Y + cy - 1, STAND_X + cx + 4, STAND_Y + cy + 3), fill=(255, 208, 64), outline=(176, 120, 24))
        draw.point((STAND_X + 7, STAND_Y - 6), fill=(255, 248, 184))
    return image


def main():
    write_sprite()
    cat_assets.write_background('merchant_room', room('empty'), 15)
    cat_assets.write_background('merchant_room_coins', room('coins'), 15)

    source = Image.open(SOURCE).convert('RGB')
    w, h = source.size
    box = (918 - 70, 718 - 50, 918 + 70, 718 + 50)                 # 오른쪽 아래 이미지 생성기 마크를 왼쪽 그림으로 덮는다
    source.paste(source.crop((box[0] - 160, box[1], box[2] - 160, box[3])), (box[0], box[1]))
    new_h = round(w / 1.25)
    top = max(0, (h - new_h) // 2)
    source = source.crop((0, top, w, top + new_h))
    cat_assets.write_background('merchant_photo', photo.to_pixel(source, INTERIOR, 13, 1.1), 15)
    print('graphics/merchant.bmp, merchant_room*.bmp, merchant_photo.bmp 작성')
    return 0


if __name__ == '__main__':
    sys.exit(main())
