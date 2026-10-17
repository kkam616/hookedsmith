"""도둑 이벤트 그림을 graphics/에 쓴다 (수동 실행).

- graphics/thief.bmp (+json): 도트 도둑 32x32 x 5장 (살금살금 걷기 4 + 검을 쥔 모습 1), 팔레트 13색
- graphics/thief_room.bmp / thief_room_coins.bmp / thief_room_empty.bmp: 밤의 어두운 방 창(86x70). 받침대에 검이 있는 것 / 금화가 쌓인 것 / 비어 있는 것
- graphics/thief_photo.bmp: 같은 자리의 창 틀 + 도둑 정면 사진 (원본 assets/graphics/events/thief_source.webp, 80x64)
창 위치·방 모양은 고양이 이벤트(make_cat_assets.py)와 같다. 도트 그림은 make_thief_sprite.py, 방은 make_thief_event_mock.py의 시안과 같다.

수동 실행: python -B tools/make_thief_assets.py
"""

import os
import sys

from PIL import Image, ImageDraw

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import make_cat as photo
import make_cat_assets as cat_assets
import make_thief_event_mock as mock
import make_thief_sprite as thief
from make_font import write_bmp_4bpp, write_if_changed

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
GRAPHICS = os.path.join(ROOT, 'graphics')
SOURCE = os.path.join(ROOT, 'assets', 'graphics', 'events', 'thief_source.webp')
STAND_X, STAND_Y = 62, 39          # 받침대 위치 (창 안쪽 80x64 기준)


def write_sprite():
    frames = [thief.side_thief(i).indices() for i in range(4)] + [thief.grab_pose().indices()]
    sheet = Image.new('P', (thief.SIZE, thief.SIZE * len(frames)), 0)
    for i, frame in enumerate(frames):
        sheet.paste(frame, (0, i * thief.SIZE))
    palette = [(255, 0, 255)] + [cat_assets.gba(thief.PALETTE[i][:3]) for i in range(1, 13)] + [(0, 0, 0)] * 3
    write_if_changed(os.path.join(GRAPHICS, 'thief.bmp'), write_bmp_4bpp(None, sheet, palette))
    write_if_changed(os.path.join(GRAPHICS, 'thief.json'), '{\n    "type": "sprite",\n    "height": 32\n}\n')


def room(kind):
    image = mock.night_room()
    draw = ImageDraw.Draw(image)
    # 받침대 (mock.sword_stand는 틀을 포함한 좌표라 안쪽 좌표로 옮겨 그린다)
    draw.rectangle((STAND_X, STAND_Y + 3, STAND_X + 11, STAND_Y + 6), fill=(120, 80, 44))
    draw.rectangle((STAND_X + 1, STAND_Y + 6, STAND_X + 10, STAND_Y + 8), fill=(84, 56, 32))

    if kind == 'sword':
        x = STAND_X
        draw.rectangle((x + 5, STAND_Y - 24, x + 6, STAND_Y + 2), fill=(206, 214, 232))
        draw.rectangle((x + 3, STAND_Y - 1, x + 8, STAND_Y), fill=(255, 208, 64))
        draw.rectangle((x + 5, STAND_Y, x + 6, STAND_Y + 2), fill=(150, 100, 56))
        draw.point((x + 5, STAND_Y - 22), fill=(255, 255, 255))
    elif kind == 'coins':
        # 금화가 쌓인 더미
        for cx, cy in ((2, 0), (6, -1), (9, 0), (4, -4), (8, -4), (6, -7)):
            draw.ellipse((STAND_X + cx, STAND_Y + cy - 1, STAND_X + cx + 4, STAND_Y + cy + 3), fill=(255, 208, 64), outline=(176, 120, 24))
        draw.point((STAND_X + 7, STAND_Y - 6), fill=(255, 248, 184))
    return image


def main():
    write_sprite()
    cat_assets.write_background('thief_room', room('sword'), 15)
    cat_assets.write_background('thief_room_coins', room('coins'), 15)
    cat_assets.write_background('thief_room_empty', room('empty'), 15)

    source = Image.open(SOURCE).convert('RGB')
    cat_assets.write_background('thief_photo', photo.to_pixel(source, (80, 64), 13, 1.1), 15)
    print('graphics/thief.bmp, thief_room*.bmp, thief_photo.bmp 작성')
    return 0


if __name__ == '__main__':
    sys.exit(main())
