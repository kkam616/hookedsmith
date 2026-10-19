"""표지 임시 배경 (코드로 그린 그림). tools/preview_cover2.py가 쓴다.
구도: 어두운 밤 대장간, 가운데 달궈진 검이 모루 위에 떠 있고, 바닥에는 부서진 검 조각, 뒤로 잠든 마을의 층층 실루엣과 화로의 붉은 후광.
위쪽 약 28%는 로고 자리, 아래 120px는 문구 띠 자리라 조용하게 둔다.
"""
import math
import random

from PIL import Image, ImageChops, ImageDraw, ImageFilter


def lerp(a, b, t):
    t = max(0.0, min(1.0, t))
    return tuple(round(a[i] + (b[i] - a[i]) * t) for i in range(3))


def stops(stops_list, t):
    for (t0, c0), (t1, c1) in zip(stops_list, stops_list[1:]):
        if t0 <= t <= t1:
            return lerp(c0, c1, (t - t0) / max(1e-9, t1 - t0))
    return stops_list[-1][1]


def background(AW, H):
    random.seed(11)
    mid = AW // 2
    img = Image.new('RGB', (AW, H))
    d = ImageDraw.Draw(img)

    # 1. 하늘
    sky = [(0.0, (5, 7, 24)), (0.35, (30, 16, 44)), (0.62, (96, 30, 36)), (0.8, (140, 52, 30)), (1.0, (30, 12, 14))]
    for y in range(H):
        d.line((0, y, AW, y), fill=stops(sky, y / H))
    for _ in range(140):
        x, y = random.randrange(AW), int(random.random() ** 1.6 * H * 0.5)
        c = random.choice(((200, 210, 255), (255, 235, 200), (150, 160, 220)))
        d.point((x, y), fill=c)
        if random.random() < 0.08:
            for dx, dy in ((1, 0), (-1, 0), (0, 1), (0, -1)):
                d.point((x + dx, y + dy), fill=tuple(v // 2 for v in c))

    # 2. 화로의 후광
    glow = Image.new('RGB', (AW, H), (0, 0, 0))
    gd = ImageDraw.Draw(glow)
    cx, cy = mid, int(H * 0.60)
    for r, col in ((560, (70, 16, 8)), (420, (130, 40, 10)), (290, (210, 90, 24)), (170, (255, 170, 70)), (80, (255, 235, 170))):
        gd.ellipse((cx - r, cy - int(r * 0.9), cx + r, cy + int(r * 0.9)), fill=col)
    img = ImageChops.screen(img, glow.filter(ImageFilter.GaussianBlur(80)))

    # 3. 마을: 먼 층(안개에 가려 밝음) + 가까운 층(어두운 실루엣)
    def village(base, color, hmin, hmax, wmin, wmax, windows, chimneys):
        layer = Image.new('RGBA', (AW, H), (0, 0, 0, 0))
        ld = ImageDraw.Draw(layer)
        x = -30
        while x < AW + 30:
            w = random.randrange(wmin, wmax)
            h = random.randrange(hmin, hmax)
            top = base - h
            ld.rectangle((x, top, x + w, H), fill=color + (255,))
            peak = random.randrange(34, 84)
            kind = random.random()
            if kind < 0.7:
                ld.polygon([(x - 10, top), (x + w / 2, top - peak), (x + w + 10, top)], fill=color + (255,))
            else:
                ld.polygon([(x - 8, top), (x + 10, top - peak * 0.6), (x + w - 10, top - peak * 0.6), (x + w + 8, top)], fill=color + (255,))
            if chimneys and random.random() < 0.5:
                cxp = x + random.randrange(10, max(11, w - 20))
                ld.rectangle((cxp, top - peak * 0.8 - 22, cxp + 14, top - peak * 0.4), fill=color + (255,))
            for _ in range(random.randrange(0, 3) if windows else 0):
                wx = x + random.randrange(8, max(9, w - 22))
                wy = top + random.randrange(14, max(15, h - 26))
                ld.rectangle((wx, wy, wx + 11, wy + 15), fill=(255, 196, 96, 255))
                ld.rectangle((wx + 5, wy, wx + 6, wy + 15), fill=color + (255,))
            x += w + random.randrange(-14, 16)
        return layer

    far = village(H - 330, (84, 40, 56), 40, 110, 60, 120, False, False)
    far = far.filter(ImageFilter.GaussianBlur(1.2))
    img.paste(far, (0, 0), far)
    mist = Image.new('RGBA', (AW, H), (0, 0, 0, 0))
    md = ImageDraw.Draw(mist)
    for i in range(8):
        y = H - 330 + i * 8
        md.rectangle((0, y, AW, y + 10), fill=(170, 80, 70, 14 + i * 3))
    img.paste(mist.filter(ImageFilter.GaussianBlur(10)), (0, 0), mist.filter(ImageFilter.GaussianBlur(10)))
    near = village(H - 215, (16, 9, 20), 60, 150, 80, 150, True, True)
    img.paste(near, (0, 0), near)

    # 4. 바닥
    ground_y = H - 195
    floor = Image.new('RGB', (AW, H - ground_y))
    fd = ImageDraw.Draw(floor)
    for y in range(floor.height):
        fd.line((0, y, AW, y), fill=lerp((34, 16, 16), (7, 5, 9), y / floor.height))
    img.paste(floor, (0, ground_y))
    refl = Image.new('RGBA', (AW, H), (0, 0, 0, 0))
    ImageDraw.Draw(refl).ellipse((mid - 330, ground_y - 4, mid + 330, ground_y + 90), fill=(255, 140, 50, 120))
    refl = refl.filter(ImageFilter.GaussianBlur(24))
    img.paste(refl, (0, 0), refl)
    ld = ImageDraw.Draw(img)
    for i, y in enumerate((ground_y + 18, ground_y + 44, ground_y + 80)):         # 돌바닥 이음선
        ld.line((0, y, AW, y), fill=(52 - i * 10, 26 - i * 4, 22 - i * 3), width=2)
    for y, step in ((ground_y + 18, 150), (ground_y + 44, 200)):
        for x in range((y % 7) * 9, AW, step):
            ld.line((x, y, x + (x - mid) // 9, y + 26), fill=(40, 20, 18), width=2)

    # 5. 모루 (윗면은 검의 빛을 받는다)
    anv_top = ground_y - 56
    anv = [(mid - 250, anv_top), (mid + 170, anv_top), (mid + 270, anv_top + 14), (mid + 160, anv_top + 40),
           (mid + 100, anv_top + 52), (mid + 112, ground_y - 8), (mid + 190, ground_y + 6), (mid - 190, ground_y + 6),
           (mid - 112, ground_y - 8), (mid - 100, anv_top + 52), (mid - 190, anv_top + 38), (mid - 330, anv_top + 26)]
    ld.polygon(anv, fill=(18, 14, 22))
    ld.polygon([(mid - 250, anv_top), (mid + 170, anv_top), (mid + 270, anv_top + 14), (mid - 330, anv_top + 26)], fill=(52, 34, 36))
    ld.line((mid - 250, anv_top + 1, mid + 170, anv_top + 1), fill=(255, 176, 90), width=3)
    ld.line((mid + 170, anv_top + 1, mid + 268, anv_top + 14), fill=(210, 110, 50), width=2)
    ld.line((mid - 330, anv_top + 26, mid - 190, anv_top + 38), fill=(120, 60, 40), width=2)
    ld.line((mid - 112, anv_top + 56, mid - 112, ground_y - 8), fill=(70, 46, 46), width=3)
    ld.ellipse((mid + 40, anv_top + 3, mid + 62, anv_top + 11), fill=(10, 8, 12))                    # 구멍

    # 6. 검 (모루 위에 떠 있다)
    blade_tip, guard_y = 330, anv_top - 168
    glow_layer = Image.new('RGBA', (AW, H), (0, 0, 0, 0))
    gd = ImageDraw.Draw(glow_layer)
    gd.polygon([(mid, blade_tip - 10), (mid + 40, blade_tip + 70), (mid + 40, guard_y), (mid - 40, guard_y), (mid - 40, blade_tip + 70)], fill=(255, 150, 50, 255))
    glow_layer = glow_layer.filter(ImageFilter.GaussianBlur(34))
    img.paste(glow_layer, (0, 0), glow_layer)

    blade = Image.new('RGBA', (AW, H), (0, 0, 0, 0))
    bd = ImageDraw.Draw(blade)
    heat = [(0.0, (255, 252, 230)), (0.25, (255, 226, 130)), (0.6, (255, 150, 52)), (1.0, (214, 70, 30))]
    for y in range(blade_tip, guard_y):
        t = (y - blade_tip) / (guard_y - blade_tip)
        half = 34 * min(1, (y - blade_tip) / 70) ** 0.7
        col = stops(heat, t)
        bd.line((mid - half, y, mid + half, y), fill=col + (255,))
        # 날 가장자리는 조금 어둡게, 가운데 홈(풀러)은 밝게
        edge = lerp(col, (120, 30, 18), 0.45)
        bd.line((mid - half, y, mid - half + 3, y), fill=edge + (255,))
        bd.line((mid + half - 3, y, mid + half, y), fill=edge + (255,))
        bd.line((mid - 3, y, mid + 3, y), fill=lerp(col, (255, 255, 240), 0.65) + (255,))
    # 날끝의 하이라이트
    bd.polygon([(mid, blade_tip), (mid + 7, blade_tip + 34), (mid - 7, blade_tip + 34)], fill=(255, 255, 245, 255))
    # 가드 (끝이 위로 휜 모양)
    bd.polygon([(mid - 150, guard_y - 24), (mid - 120, guard_y - 4), (mid + 120, guard_y - 4), (mid + 150, guard_y - 24),
                (mid + 138, guard_y + 14), (mid + 40, guard_y + 24), (mid - 40, guard_y + 24), (mid - 138, guard_y + 14)], fill=(46, 28, 28, 255))
    bd.line((mid - 146, guard_y - 22, mid - 118, guard_y - 4, mid + 118, guard_y - 4, mid + 146, guard_y - 22), fill=(255, 150, 70, 255), width=3)
    bd.rectangle((mid - 22, guard_y + 24, mid + 22, guard_y + 88), fill=(54, 32, 30, 255))             # 손잡이
    for y in range(guard_y + 30, guard_y + 86, 10):
        bd.line((mid - 22, y, mid + 22, y + 6), fill=(110, 66, 50, 255), width=2)
    bd.ellipse((mid - 28, guard_y + 84, mid + 28, guard_y + 128), fill=(70, 42, 36, 255))                # 꼬리 장식
    bd.ellipse((mid - 14, guard_y + 94, mid + 14, guard_y + 118), fill=(255, 160, 70, 255))
    img.paste(blade, (0, 0), blade)

    # 7. 바닥의 부서진 검 조각 (한쪽 모서리에 불빛)
    frag = ImageDraw.Draw(img)
    for _ in range(18):
        fx = random.randrange(40, AW - 40)
        fy = random.randrange(ground_y + 8, H - 125)
        if abs(fx - mid) < 90 and fy < ground_y + 40:
            continue
        L = random.randrange(26, 70)
        a = random.uniform(-0.5, 0.5) + random.choice((0, math.pi))
        dx, dy = math.cos(a) * L, math.sin(a) * L * 0.45
        wdt = random.randrange(5, 12)
        pts = [(fx - dx, fy - dy), (fx + dx, fy + dy), (fx + dx * 0.6 - dy * 0.3, fy + dy * 0.6 + wdt), (fx - dx * 0.6, fy - dy * 0.6 + wdt)]
        frag.polygon(pts, fill=(34, 24, 30))
        frag.line((pts[0], pts[1]), fill=(255, 150, 70), width=2)

    # 8. 불씨 + 꼬리
    sparks = Image.new('RGBA', (AW, H), (0, 0, 0, 0))
    sp = ImageDraw.Draw(sparks)
    for _ in range(260):
        spread = random.gauss(0, 150)
        x = mid + spread * (1 + random.random())
        y = guard_y + 80 - abs(random.gauss(0, 330)) - random.random() * 120
        r = random.choice((1.5, 2, 2, 3, 3, 4))
        color = (255, random.randrange(140, 240), random.randrange(40, 120), random.randrange(150, 255))
        tail = random.randrange(0, 22)
        sp.line((x, y, x - spread * 0.06, y + tail), fill=color[:3] + (110,), width=max(1, int(r // 2)))
        sp.ellipse((x - r, y - r, x + r, y + r), fill=color)
    glow_s = sparks.filter(ImageFilter.GaussianBlur(5))
    img.paste(glow_s, (0, 0), glow_s)
    img.paste(sparks, (0, 0), sparks)

    # 9. 위쪽(로고 자리)은 살짝 어둡게, 네 모서리 비네팅
    vg = Image.new('L', (AW, H), 0)
    vd = ImageDraw.Draw(vg)
    vd.ellipse((-AW * 0.35, -H * 0.25, AW * 1.35, H * 1.2), fill=255)
    vg = vg.filter(ImageFilter.GaussianBlur(AW * 0.14))
    img = Image.composite(img, Image.blend(img, Image.new('RGB', (AW, H), (4, 3, 6)), 0.6), vg)

    # 10. 도트풍으로 단순화
    scale = 3
    small = img.resize((AW // scale, H // scale), Image.BILINEAR)
    small = small.quantize(colors=96, method=Image.Quantize.MAXCOVERAGE, kmeans=2, dither=Image.Dither.NONE).convert('RGB')
    return small.resize((AW // scale * scale, H // scale * scale), Image.NEAREST).resize((AW, H), Image.NEAREST)
