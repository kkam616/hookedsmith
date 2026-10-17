"""도둑 이벤트 연출 + 웃음소리 시안 (게임에는 아직 적용하지 않는다).

밤 장면: 배경이 푸르게 어두워지고, 창 안(어두운 방)으로 도둑이 살금살금 들어와 → 두리번 → 검을 집으면 → 같은 창에 도둑 정면 사진(assets/graphics/events/thief_source.webp, 80x64)이 번쩍이며 나오고 "히히히" 웃음소리.
출력: build/thief_preview/thief_event.gif, thief_event_sound.wav(GIF와 같은 길이), stills.png,
      laugh_A_quick.wav / laugh_B_sly.wav / laugh_C_excited.wav, laugh_all.wav
수동 실행: python -B tools/make_thief_event_mock.py
"""

import math
import os
import sys

from PIL import Image, ImageDraw, ImageFont

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import make_cat as photo
import make_sfx as sfx
import make_thief_sprite as thief
import preview_sell_fx as base
from make_sfx import RATE, Noise, envelope, mix, place, render, write_wav

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
OUT = os.path.join(ROOT, 'build', 'thief_preview')
FPS = 50
SCALE = 4
INTERIOR = (80, 64)


# ── 웃음소리 "히히히" ──────────────────────────────────────────────────────
def hi(f_start, f_end, seconds, volume=0.5, breath=0.25, vibrato=0.04):
    """"히" 한 번: 짧은 숨소리(ㅎ) + 높은 모음 소리(이). 음높이는 위에서 아래로 떨어진다"""
    noise = Noise(int(f_start) % 97 + 3)
    phase = [0.0]
    breath_len = 0.035

    def generator(t, p):
        f = (f_start + (f_end - f_start) * p) * (1 + vibrato * math.sin(t * 52))
        phase[0] = (phase[0] + f / RATE) % 1.0
        duty = 0.2 + 0.15 * math.sin(math.pi * p)
        voiced = (1.0 if phase[0] < duty else -1.0) * 0.8 + (1.0 if (phase[0] * 2) % 1.0 < 0.25 else -1.0) * 0.25
        puff = noise.next(1) * breath * max(0.0, 1 - t / breath_len)
        return (voiced * envelope(p, seconds, 0.06, 1.1) * min(1.0, t / 0.01) + puff) * volume

    return render(seconds, generator)


def laugh_a():          # 빠르고 높은 "히히히" (0.7초)
    total = 0.7
    parts = [place(hi(820 + 40 * i, 640 + 30 * i, 0.12), 0.17 * i, total) for i in range(3)]
    parts.append(place(hi(900, 560, 0.2, 0.5, 0.15, 0.05), 0.5, total))
    return mix(*parts)


def laugh_b():          # 음흉하게 낮은 "히힛.. 히힛.. 히히히~" (1.3초)
    total = 1.3
    parts = [
        place(hi(560, 470, 0.14), 0.0, total),
        place(hi(540, 450, 0.14), 0.27, total),
        place(hi(620, 480, 0.12), 0.58, total),
        place(hi(600, 470, 0.12), 0.74, total),
        place(hi(580, 360, 0.42, 0.5, 0.12, 0.06), 0.9, total),
    ]
    return mix(*parts)


def laugh_c():          # 들뜬 "히히히히히!" 다섯 번 빠르게 (0.8초)
    total = 0.8
    parts = [place(hi(980 - 25 * i, 760 - 25 * i, 0.09), 0.12 * i, total) for i in range(5)]
    parts.append(place(hi(1000, 640, 0.2, 0.45, 0.1, 0.05), 0.6, total))
    return mix(*parts)


# ── 밤 장면 ────────────────────────────────────────────────────────────────
def night(image, amount=0.5):
    """푸르게 어둡게"""
    px = image.load()
    for y in range(image.height):
        for x in range(image.width):
            r, g, b = px[x, y][:3]
            px[x, y] = (int(r * (1 - amount) * 0.85), int(g * (1 - amount) * 0.95), int(b * (1 - amount * 0.4) + 12))
    return image


def night_room():
    """창 안 (80x64): 달이 비치는 푸른 어둠, 나무 바닥, 오른쪽에 검이 꽂힌 받침대"""
    room = Image.new('RGB', INTERIOR, (14, 18, 40))
    draw = ImageDraw.Draw(room)
    for y in range(INTERIOR[1]):
        shade = 18 + y // 3
        draw.line((0, y, INTERIOR[0], y), fill=(shade - 6, shade, shade + 26))
    draw.ellipse((8, 6, 20, 18), fill=(240, 236, 190))
    draw.ellipse((12, 4, 24, 16), fill=(18, 24, 52))
    floor_top = 46
    draw.rectangle((0, floor_top, INTERIOR[0], INTERIOR[1]), fill=(44, 34, 52))
    draw.line((0, floor_top, INTERIOR[0], floor_top), fill=(84, 72, 104))
    for x in range(-6, INTERIOR[0], 16):
        draw.line((x, floor_top + 1, x + 8, INTERIOR[1]), fill=(32, 24, 40))
    return room


def sword_stand(draw, x, with_sword=True):
    """받침대 + 금빛 검"""
    draw.rectangle((x, 42, x + 11, 45), fill=(120, 80, 44))
    draw.rectangle((x + 1, 45, x + 10, 47), fill=(84, 56, 32))
    if with_sword:
        draw.rectangle((x + 5, 18, x + 6, 41), fill=(206, 214, 232))
        draw.rectangle((x + 3, 38, x + 8, 39), fill=(255, 208, 64))
        draw.rectangle((x + 5, 39, x + 6, 41), fill=(150, 100, 56))
        draw.point((x + 5, 20), fill=(255, 255, 255))


def text_with_outline(draw, text, center_x, y, font):
    width = draw.textlength(text, font=font)
    x = center_x - width / 2
    for dx in (-1, 0, 1):
        for dy in (-1, 0, 1):
            draw.text((x + dx, y + dy), text, font=font, fill=(0, 0, 0, 255))
    draw.text((x, y), text, font=font, fill=(255, 255, 255, 255))


def main():
    os.makedirs(OUT, exist_ok=True)

    # 소리 시안
    sounds = [('laugh_A_quick', laugh_a()), ('laugh_B_sly', laugh_b()), ('laugh_C_excited', laugh_c())]
    combined = []
    for name, data in sounds:
        write_wav(os.path.join(OUT, name + '.wav'), data)
        combined += data + [0.0] * sfx.samples(0.7)
        print('%-16s %.2f초' % (name, len(data) / RATE))
    write_wav(os.path.join(OUT, 'laugh_all.wav'), combined)

    # 장면
    scene = night(base.base_scene().convert('RGB'), 0.45).convert('RGBA')
    window_x, window_y = 77, 41
    interior = photo.framed(night_room()).convert('RGBA')
    thief_source = Image.open(os.path.join(ROOT, 'assets', 'graphics', 'events', 'thief_source.webp')).convert('RGB')
    thief_photo = photo.framed(photo.to_pixel(thief_source, (80, 64), 15, 1.1)).convert('RGBA')
    font = ImageFont.truetype(r'C:\Windows\Fonts\gulim.ttc', 12, index=0)

    side = [thief.to_rgba(thief.side_thief(i)) for i in range(4)]
    grab = thief.to_rgba(thief.grab_pose())
    front = [thief.to_rgba(thief.front_thief(i)) for i in range(2)]

    floor_y = 3 + 46 - 24                  # 도둑 발이 바닥에 닿는 높이 (창 안쪽 기준)
    stand_x = 3 + 62
    open_frames = 8
    walk_start = open_frames + 8
    walk_frames = 56
    peek_frames = 26
    grab_frame = walk_start + walk_frames + peek_frames
    laugh_frame = grab_frame + 10
    laugh_end = laugh_frame + 70
    fade_end = laugh_end + 14
    msg_one = laugh_frame + 12
    msg_two = msg_one + 56
    total = msg_two + 100
    target_x = 34

    frames = []

    for f in range(total):
        image = scene.copy()
        content = interior.copy()
        grow = min(1.0, (f + 1) / open_frames)

        if f >= open_frames:
            cdraw = ImageDraw.Draw(content)
            sword_present = f < grab_frame
            sword_stand(cdraw, stand_x, sword_present)

            layer = Image.new('RGBA', content.size, (0, 0, 0, 0))

            if f < walk_start:
                sprite = None
            elif f < walk_start + walk_frames:
                u = f - walk_start
                x = -30 + (target_x + 30) * u / walk_frames
                sprite = (side[(u // 6) % 4], int(x), floor_y)
            elif f < grab_frame:
                sprite = (side[3] if ((f - grab_frame) // 8) % 2 else side[1], target_x + (2 if ((f // 8) % 2) else 0), floor_y)
            elif f < laugh_frame:
                sprite = (grab, target_x + 6, floor_y)
            else:
                sprite = None

            if sprite is not None:
                art, px, py = sprite
                if laugh_end - 1 <= f < fade_end:
                    pass
                layer.alpha_composite(art, (px, py))

                # 창 안쪽 밖은 가린다
                mask = Image.new('L', content.size, 0)
                ImageDraw.Draw(mask).rectangle((3, 3, content.width - 4, content.height - 4), fill=255)
                clipped = Image.new('RGBA', content.size, (0, 0, 0, 0))
                clipped.paste(layer, (0, 0), mask)
                content.alpha_composite(clipped)

        if f >= laugh_frame:
            content = thief_photo.copy()                       # 같은 창에 도둑 정면 사진 (고양이 사진과 같은 방식)
            if f - laugh_frame < 3:
                flash = Image.new('RGBA', content.size, (255, 255, 255, int(190 - (f - laugh_frame) * 60)))
                content.alpha_composite(flash)

        height = max(2, int(content.height * grow))
        shown = content.resize((content.width, height), Image.NEAREST)
        image.alpha_composite(shown, (window_x, window_y + (content.height - height) // 2))

        draw = ImageDraw.Draw(image)
        base_y = window_y + content.height + 8
        if f >= msg_two:
            text_with_outline(draw, '+21 템페스트 골드를', 120, base_y, font)
            text_with_outline(draw, '도둑맞았다...', 120, base_y + 14, font)
        elif f >= msg_one:
            text_with_outline(draw, '수상한 그림자가', 120, base_y, font)
            text_with_outline(draw, '창고를 뒤졌다!', 120, base_y + 14, font)

        frames.append(image.convert('RGB').resize((240 * SCALE, 160 * SCALE), Image.NEAREST))

    frames[0].save(os.path.join(OUT, 'thief_event.gif'), save_all=True, append_images=frames[1:] + [frames[-1]] * 40,
                   duration=1000 // FPS, loop=0, optimize=False, disposal=1)

    # GIF와 같이 틀면 맞도록: 창이 열릴 때 이벤트 알림(반짝), 웃음은 도둑이 정면을 보는 프레임에
    length = total / FPS + 0.8
    track = mix(place(sfx.event_sparkle(), 0.0, length), place(laugh_b(), laugh_frame / FPS, length))
    write_wav(os.path.join(OUT, 'thief_event_sound.wav'), track)

    stills = Image.new('RGB', (480 * 3, 320 * 2))
    picks = (walk_start + 30, grab_frame - 6, grab_frame + 4, laugh_frame + 6, msg_one + 6, msg_two + 10)
    for i, f in enumerate(picks):
        stills.paste(frames[f].resize((480, 320)), ((i % 3) * 480, (i // 3) * 320))
    stills.save(os.path.join(OUT, 'stills.png'))

    print('출력:', OUT, ' 웃음 프레임', laugh_frame, '(%.2f초)' % (laugh_frame / FPS), ' 전체 %.1f초' % (total / FPS))
    return 0


if __name__ == '__main__':
    sys.exit(main())
