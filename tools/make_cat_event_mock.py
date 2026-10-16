"""고양이 이벤트 연출 시안 (게임에는 아직 적용하지 않는다).

강화 화면 배경(컬러) 위에: 도트 고양이가 왼쪽에서 걸어 들어와 앉아 꼬리를 흔들고 →
가운데에 사진 창이 열리며 야옹! → 창 아래에 메시지.
출력: build/cat_event_preview/cat_event.gif, cat_event_sound.wav (GIF와 같은 길이, 야옹이 열리는 순간에 맞춰 있다), stills.png
수동 실행: python -B tools/make_cat_event_mock.py
"""

import os
import sys

from PIL import Image, ImageDraw, ImageFont

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import make_cat as photo
import make_cat_sprite as cat
import make_sfx as sfx
import preview_sell_fx as base

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
OUT = os.path.join(ROOT, 'build', 'cat_event_preview')
FPS = 50                                  # GIF 20ms 한 장
PHOTO_BOX = (90, 30, 1110, 845)           # 얼굴 (가로:세로 = 5:4)
PHOTO_SIZE = (80, 64)
SCALE = 4


def text_with_outline(draw, text, center_x, y, font):
    width = draw.textlength(text, font=font)
    x = center_x - width / 2
    for dx in (-1, 0, 1):
        for dy in (-1, 0, 1):
            draw.text((x + dx, y + dy), text, font=font, fill=(0, 0, 0, 255))
    draw.text((x, y), text, font=font, fill=(255, 255, 255, 255))


def main():
    os.makedirs(OUT, exist_ok=True)

    source = Image.open(photo.SOURCE).convert('RGB').crop(PHOTO_BOX)
    window = photo.framed(photo.to_pixel(source, PHOTO_SIZE)).convert('RGBA')       # 86x70
    window_x = (240 - window.width) // 2
    window_y = (160 - window.height) // 2 - 4

    scene = base.base_scene()
    walk = [cat.frame_walk(i) for i in range(4)]
    sit = [cat.frame_sit(i) for i in range(3)]
    font = ImageFont.truetype(r'C:\Windows\Fonts\gulim.ttc', 12, index=0)

    # 창 안쪽 (80x64): 어두운 방 + 바닥. 고양이는 이 안에서만 움직인다.
    interior = Image.new('RGBA', PHOTO_SIZE, (34, 24, 30, 255))
    ip = ImageDraw.Draw(interior)
    for y in range(PHOTO_SIZE[1]):
        shade = 34 + y // 3
        ip.line((0, y, PHOTO_SIZE[0], y), fill=(shade, shade - 8, shade - 4, 255))
    floor_top = 46
    ip.rectangle((0, floor_top, PHOTO_SIZE[0], PHOTO_SIZE[1]), fill=(92, 62, 48, 255))
    ip.line((0, floor_top, PHOTO_SIZE[0], floor_top), fill=(140, 100, 70, 255))
    for x in range(-6, PHOTO_SIZE[0], 16):
        ip.line((x, floor_top + 1, x + 8, PHOTO_SIZE[1]), fill=(70, 46, 36, 255))
    interior = photo.framed(interior.convert('RGB')).convert('RGBA')        # 테두리 포함 86x70 (프레임 안쪽 시작 3,3)

    cat_floor = 3 + floor_top - 22                         # 고양이 발이 바닥 위에 오도록
    sit_x = 3 + (PHOTO_SIZE[0] - 32) // 2
    speed = 1.4
    open_frames = 8
    walk_start = open_frames + 6
    arrive = walk_start + int((sit_x + 32) / speed)       # 앉는 프레임
    photo_frame = arrive + 64                              # 사진이 나오고 야옹
    messages_first = photo_frame + 14
    messages_both = messages_first + 36
    total = messages_both + 110

    frames = []
    position = -32.0

    for f in range(total):
        image = scene.copy()

        if f < open_frames:
            content = interior
            grow = (f + 1) / open_frames
        else:
            grow = 1.0
            content = interior.copy()

            if f < photo_frame:
                if f < arrive:
                    position = -32.0 + (f - walk_start) * speed if f >= walk_start else -32.0
                    sprite = walk[((f - walk_start) // 5) % 4] if f >= walk_start else walk[0]
                else:
                    position = sit_x
                    u = f - arrive
                    sprite = sit[1] if u < 10 else sit[(u // 8) % 3]

                # 창 안쪽 영역 밖으로는 그려지지 않게 잘라서 붙인다
                clip = Image.new('RGBA', content.size, (0, 0, 0, 0))
                clip.alpha_composite(sprite, (int(position), cat_floor))
                mask = Image.new('L', content.size, 0)
                ImageDraw.Draw(mask).rectangle((3, 3, content.width - 4, content.height - 4), fill=255)
                clipped = Image.new('RGBA', content.size, (0, 0, 0, 0))
                clipped.paste(clip, (0, 0), mask)
                content.alpha_composite(clipped)
            else:
                content = window.copy()
                if f - photo_frame < 3:
                    flash = Image.new('RGBA', content.size, (255, 255, 255, int(190 - (f - photo_frame) * 60)))
                    content.alpha_composite(flash)

        height = max(2, int(content.height * grow))
        shown = content.resize((content.width, height), Image.NEAREST)
        image.alpha_composite(shown, (window_x, window_y + (content.height - height) // 2))

        draw = ImageDraw.Draw(image)
        if f >= messages_both:
            text_with_outline(draw, '고양이다!', 120, window_y + window.height + 8, font)
            text_with_outline(draw, '보은이다!', 120, window_y + window.height + 22, font)
        elif f >= messages_first:
            text_with_outline(draw, '고양이다!', 120, window_y + window.height + 15, font)

        frames.append(image.convert('RGB').resize((240 * SCALE, 160 * SCALE), Image.NEAREST))

    frames[0].save(os.path.join(OUT, 'cat_event.gif'), save_all=True, append_images=frames[1:] + [frames[-1]] * 40,
                   duration=1000 // FPS, loop=0, optimize=False, disposal=1)

    # GIF와 같이 틀면 맞도록: 사진이 나오는 프레임에 야옹
    length = total / FPS + 0.8
    track = sfx.place(sfx.cat_meow(), photo_frame / FPS, length)
    sfx.write_wav(os.path.join(OUT, 'cat_event_sound.wav'), track)

    stills = Image.new('RGB', (480 * 3, 320 * 2))
    picks = (open_frames // 2, walk_start + 20, arrive + 24, photo_frame - 1, photo_frame + 2, messages_both + 10)
    for i, f in enumerate(picks):
        stills.paste(frames[f].resize((480, 320)), ((i % 3) * 480, (i // 3) * 320))
    stills.save(os.path.join(OUT, 'stills.png'))

    print('출력:', OUT, ' 야옹 프레임', photo_frame, '(%.2f초)' % (photo_frame / FPS), ' 전체 %.1f초' % (total / FPS))
    return 0


if __name__ == '__main__':
    sys.exit(main())
