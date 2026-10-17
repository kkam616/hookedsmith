"""강화 화면 배경의 시간대 변형 (노을·밤).

낮(지금 그림)은 그대로 두고, 왼쪽 위 열린 문 밖 풍경과 바닥에 드리운 빛줄기만 바꿔 노을·밤 배경을 만든다.
- 노을: 하늘이 보라→주황으로 물들고, 길과 빛줄기는 주황빛, 집·울타리는 실루엣처럼 어두워진다.
- 밤: 문 밖은 남색 하늘에 달과 별, 빛줄기는 푸른 달빛, 방 안은 아주 약하게 푸른 기운이 돈다. 화로 불빛은 그대로 따뜻하다.
출력: build/forge_bg_time/ (낮·노을·밤 비교, 문 부분 확대)
수동 실행: python -B tools/make_forge_bg_time.py            (미리보기만)
          python -B tools/make_forge_bg_time.py --write    (graphics/forge_bg_sunset·forge_bg_night.bmp 작성 — 낮 그림 graphics/forge_bg.bmp를 바꾸면 다시)
"""

import os
import random
import sys

from PIL import Image, ImageDraw

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
from make_font import write_if_changed

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
SOURCE = os.path.join(ROOT, 'graphics', 'forge_bg.bmp')
OUT = os.path.join(ROOT, 'build', 'forge_bg_time')
WIDTH, HEIGHT = 240, 160

# 바뀌는 영역: 열린 문(왼쪽 위)과 거기서 바닥으로 드리운 빛줄기 (240x160 기준)
REGION = [(8, 0), (60, 0), (60, 40), (84, 56), (86, 70), (30, 67), (8, 58)]
SKY_BOTTOM = 34         # 이 줄 위쪽의 파란 픽셀만 하늘로 본다
HOUSE = [(33, 26), (34, 17), (40, 12), (45, 13), (50, 19), (50, 27), (40, 28)]      # 문 밖 멀리 보이는 집: 노을·밤에는 실루엣이 된다


def lerp(a, b, t):
    return tuple(int(a[i] + (b[i] - a[i]) * t) for i in range(3))


def gradient(stops, t):
    t = max(0.0, min(1.0, t))
    for (t0, c0), (t1, c1) in zip(stops, stops[1:]):
        if t0 <= t <= t1:
            return lerp(c0, c1, (t - t0) / max(1e-9, t1 - t0))
    return stops[-1][1]


def luminance(rgb):
    r, g, b = rgb
    return (0.299 * r + 0.587 * g + 0.114 * b) / 255


def in_region(mask_image):
    draw = ImageDraw.Draw(mask_image)
    draw.polygon(REGION, fill=255)
    return mask_image


# 노을: 하늘 위쪽 보라 → 지평선 주황, 땅과 빛줄기는 주황·금빛
SUNSET_SKY = [(0.0, (92, 52, 128)), (0.45, (206, 92, 112)), (0.8, (255, 150, 74)), (1.0, (255, 196, 110))]
SUNSET_LIGHT = [(0.0, (38, 18, 40)), (0.35, (128, 44, 60)), (0.6, (226, 104, 48)), (0.85, (255, 170, 80)), (1.0, (255, 214, 140))]

# 밤: 하늘 남색, 땅과 빛줄기는 푸른 달빛
NIGHT_SKY = [(0.0, (8, 12, 38)), (0.6, (22, 32, 78)), (1.0, (44, 60, 112))]
SUNSET_HOUSE = [(0.0, (44, 22, 58)), (0.55, (96, 48, 86)), (1.0, (178, 96, 100))]
NIGHT_HOUSE = [(0.0, (8, 10, 28)), (1.0, (46, 58, 100))]
NIGHT_LIGHT = [(0.0, (8, 10, 26)), (0.4, (30, 40, 84)), (0.75, (74, 96, 152)), (1.0, (150, 172, 220))]


def is_sky(rgb, y):
    r, g, b = rgb
    return y < SKY_BOTTOM and b > r + 4


def smoothstep(edge0, edge1, x):
    t = max(0.0, min(1.0, (x - edge0) / (edge1 - edge0)))
    return t * t * (3 - 2 * t)


def mix(a, b, t):
    return tuple(a[i] + (b[i] - a[i]) * t for i in range(3))


def recolor(image, mode):
    """영역 가장자리를 부드럽게 풀고(깃털 처리), 밝은 낮 픽셀만 시간대 색으로 바꾸며, 나머지는 같은 분위기의 약한 색조만 입힌다."""
    from PIL import ImageFilter

    out = image.copy()
    px = out.load()
    src = image.load()
    rng = random.Random(7)

    feather = in_region(Image.new('L', image.size, 0)).filter(ImageFilter.GaussianBlur(2.2))
    fp = feather.load()
    house_mask = Image.new('L', image.size, 0)
    ImageDraw.Draw(house_mask).polygon(HOUSE, fill=255)
    hp = house_mask.load()

    if mode == 'sunset':
        sky_stops, light_stops, house_stops = SUNSET_SKY, SUNSET_LIGHT, SUNSET_HOUSE
        ambient = (1.06, 0.95, 0.96)
    else:
        sky_stops, light_stops, house_stops = NIGHT_SKY, NIGHT_LIGHT, NIGHT_HOUSE
        ambient = (0.88, 0.93, 1.07)

    for y in range(HEIGHT):
        for x in range(WIDTH):
            rgb = src[x, y]
            r, g, b = rgb
            lum = luminance(rgb)
            f = fp[x, y] / 255.0
            fire = r > 150 and r > b + 70

            # 방 안 분위기: 영역 안은 강하게, 영역 밖(화로 쪽은 제외)은 아주 약하게 같은 색조를 입힌다
            tint_weight = f * 0.85       # 영역 밖은 낮 그림 그대로 둔다 (색 수가 빠듯해서 바닥 질감이 뭉개지는 것을 막는다)
            tinted = tuple(max(0, min(255, c * ambient[i] + (3 if (mode == 'night' and i == 2) else 0))) for i, c in enumerate(rgb))
            base = mix(rgb, tinted, tint_weight)

            if f > 0:
                day = max(smoothstep(0.16, 0.36, lum), 1.0 if b > r + 6 else 0.0)
                if hp[x, y]:
                    shade = (lum - 0.38) * 1.7 + 0.32        # 지붕과 벽의 밝기 차이를 키워 실루엣 안에도 모양이 남게 한다
                    rc = gradient(house_stops, shade)
                elif y < 23 or (y < SKY_BOTTOM and b > r + 4):
                    horizon = y / SKY_BOTTOM
                    rc = gradient(sky_stops, horizon)
                    cloud = max(0.0, lum - 0.62)                 # 구름처럼 밝았던 곳은 하늘색을 살짝 밝게
                    rc = tuple(max(0, min(255, c * (0.94 + 0.12 * (lum - 0.55)) + 90 * cloud * (0.6 if mode == 'sunset' else 0.25))) for c in rc)
                else:
                    t = lum ** (0.9 if mode == 'sunset' else 1.15)
                    rc = gradient(light_stops, t)
                base = mix(base, rc, f * day)

            px[x, y] = tuple(int(round(c)) for c in base)

    if mode == 'night':
        draw = ImageDraw.Draw(out)
        moon = (42, 10)
        for radius, color in ((9, (22, 32, 78)), (7, (40, 56, 108))):
            draw.ellipse((moon[0] - radius, moon[1] - radius, moon[0] + radius, moon[1] + radius), fill=color)
        draw.ellipse((moon[0] - 4, moon[1] - 4, moon[0] + 4, moon[1] + 4), fill=(238, 234, 196))
        draw.ellipse((moon[0] - 1, moon[1] - 4, moon[0] + 6, moon[1] + 3), fill=(48, 62, 114))         # 초승달: 한쪽을 하늘색으로 덮는다
        px[38, 21] = px[39, 21] = px[38, 22] = px[39, 22] = (255, 206, 96)                                  # 집 창문에 불이 켜져 있다
        for _ in range(16):
            x, y = rng.randint(24, 52), rng.randint(1, 24)
            if fp[x, y] > 200 and y < SKY_BOTTOM and src[x, y][2] > src[x, y][0] + 4 and not hp[x, y]:
                px[x, y] = (232, 238, 255) if rng.random() < 0.5 else (150, 168, 220)

    return out


def build_indexed(image):
    """낮 그림(graphics/forge_bg.bmp)의 팔레트·색 번호를 최대한 그대로 쓴다.
    - 바뀌지 않는 영역(문 밖 영역 바깥)은 낮 그림과 똑같은 색 번호
    - 영역 안에서만 쓰이던 팔레트 칸(낮 그림의 하늘·빛줄기 색)을 새 색으로 바꿔 쓰고, 나머지 픽셀은 가장 가까운 기존 색을 쓴다
    반환: (색 번호 이미지 240x160, 팔레트 64색)"""
    from PIL import ImageFilter

    base = Image.open(SOURCE)
    raw = base.getpalette()
    palette = [tuple(raw[i * 3:i * 3 + 3]) for i in range(65)]          # 0번은 안 쓰는 검정
    indices = base.crop((0, 0, WIDTH, HEIGHT))
    ip = indices.load()

    feather = in_region(Image.new('L', (WIDTH, HEIGHT), 0)).filter(ImageFilter.GaussianBlur(2.2))
    fp = feather.load()

    inside, outside = set(), set()
    for y in range(HEIGHT):
        for x in range(WIDTH):
            (inside if fp[x, y] > 0 else outside).add(ip[x, y])
    free = sorted(inside - outside)                                      # 새 색으로 바꿔 쓸 수 있는 칸
    keep = sorted(outside)

    def dist(a, b):
        return sum((a[i] - b[i]) ** 2 for i in range(3)) ** 0.5

    def nearest(color, candidates):
        best = min(candidates, key=lambda i: dist(color, palette[i]))
        return best, dist(color, palette[best])

    # 영역 안 픽셀 중 기존 색으로는 안 되는 것(하늘·빛줄기·달 등)만 새 색 후보로 모은다
    special = []
    for y in range(HEIGHT):
        for x in range(WIDTH):
            if fp[x, y] > 0:
                color = image.getpixel((x, y))
                if nearest(color, keep)[1] > 26:
                    special.append(color)

    if special and free:
        strip = Image.new('RGB', (len(special), 1))
        strip.putdata(special)
        quantized = strip.quantize(colors=len(free), method=Image.Quantize.MAXCOVERAGE, kmeans=5, dither=Image.Dither.NONE)
        qraw = quantized.getpalette()
        for slot, i in zip(free, range(len(free))):
            palette[slot] = tuple(c & 0xF8 for c in qraw[i * 3:i * 3 + 3])

    result = indices.copy()
    rp = result.load()
    candidates = keep + free
    for y in range(HEIGHT):
        for x in range(WIDTH):
            if fp[x, y] > 0:
                rp[x, y] = nearest(image.getpixel((x, y)), candidates)[0]

    return result, palette, len(free), len(special)


def write_variant(name, image):
    indices, palette, free_count, special_count = build_indexed(image)

    canvas = Image.new('P', (256, 256), 0)
    canvas.paste(indices, (0, 0))
    canvas.putpalette([channel for color in palette for channel in color])
    path = os.path.join(ROOT, 'graphics', name + '.bmp')
    canvas.save(path + '.tmp', format='BMP')
    with open(path + '.tmp', 'rb') as handle:
        write_if_changed(path, handle.read())
    os.remove(path + '.tmp')
    write_if_changed(os.path.join(ROOT, 'graphics', name + '.json'), '{\n    "type": "regular_bg",\n    "bpp_mode": "bpp_8"\n}\n')
    print('%s: 새 색 칸 %d개, 새 색이 필요한 픽셀 %d개' % (name, free_count, special_count))


def main():
    os.makedirs(OUT, exist_ok=True)
    day = Image.open(SOURCE).convert('RGB').crop((0, 0, WIDTH, HEIGHT))
    variants = [('낮 (지금)', day), ('노을', recolor(day, 'sunset')), ('밤', recolor(day, 'night'))]

    scale = 3
    sheet = Image.new('RGB', (WIDTH * scale * 3 + 40, HEIGHT * scale + 30), (30, 30, 36))
    for i, (title, image) in enumerate(variants):
        big = image.resize((WIDTH * scale, HEIGHT * scale), Image.NEAREST)
        sheet.paste(big, (10 + i * (WIDTH * scale + 10), 24))
        ImageDraw.Draw(sheet).text((10 + i * (WIDTH * scale + 10), 6), title, fill=(255, 255, 255))
    sheet.save(os.path.join(OUT, 'compare.png'))

    # 문 부분만 8배 확대 (낮 / 노을 / 밤)
    crop_box = (0, 0, 100, 80)
    zoom = Image.new('RGB', (800 * 3 + 20, 640), (30, 30, 36))
    for i, (title, image) in enumerate(variants):
        zoom.paste(image.crop(crop_box).resize((800, 640), Image.NEAREST), (i * 810, 0))
    zoom.resize((zoom.width // 2, zoom.height // 2), Image.NEAREST).save(os.path.join(OUT, 'door_zoom.png'))

    for name, (title, image) in zip(('day', 'sunset', 'night'), variants):
        image.save(os.path.join(OUT, 'bg_%s.png' % name))

    if '--write' in sys.argv:
        write_variant('forge_bg_sunset', variants[1][1])
        write_variant('forge_bg_night', variants[2][1])
        print('graphics/forge_bg_sunset.bmp, forge_bg_night.bmp 작성')

    print('출력:', OUT)
    return 0


if __name__ == '__main__':
    sys.exit(main())
