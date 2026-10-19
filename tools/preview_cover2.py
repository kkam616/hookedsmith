"""GBA 박스 앞면 시안 2 (build/cover/cover2.png): 새로 그린(코드로 그린 임시) 배경 + 로고 + 왼쪽 GAME BOY ADVANCE 띠 + JAYS LOFT 로고.
배경은 자리 잡기용 임시 그림이다 (같은 구도의 AI 일러스트로 바꿀 수 있다). 인증 마크(닌텐도 품질 인증, ESRB)는 넣지 않는다.
사용: python -B tools/preview_cover2.py
"""
import math
import os
import random
import sys

from PIL import Image, ImageChops, ImageDraw, ImageFilter, ImageFont

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
from preview_new_title import key_out_black

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
T = os.path.join(ROOT, 'assets', 'graphics', 'title')
OUT = os.path.join(ROOT, 'build', 'cover')
W, H = 1050, 1040
SPINE = 150      # 공식 표지의 왼쪽 띠 폭 비율(약 14%)
AW = W - SPINE
ARIAL = 'C:/Windows/Fonts/arialbd.ttf'
IMPACT = 'C:/Windows/Fonts/impact.ttf'


def lerp(a, b, t):
    return tuple(round(a[i] + (b[i] - a[i]) * t) for i in range(3))


def background():
    random.seed(7)
    img = Image.new('RGB', (AW, H))
    d = ImageDraw.Draw(img)
    for y in range(H):
        t = y / H
        d.line((0, y, AW, y), fill=lerp((10, 12, 38), (74, 22, 30), min(1, t * 1.25)))
    for _ in range(90):
        d.point((random.randrange(AW), random.randrange(int(H * 0.45))), fill=(220, 226, 255))

    # 뒤에서 오는 화로 빛
    glow = Image.new('RGB', (AW, H), (0, 0, 0))
    gd = ImageDraw.Draw(glow)
    cx, cy = AW // 2, int(H * 0.60)
    for r, col in ((520, (80, 20, 6)), (380, (150, 50, 10)), (240, (240, 120, 30)), (120, (255, 210, 120))):
        gd.ellipse((cx - r, cy - r, cx + r, cy + r), fill=col)
    img = ImageChops.screen(img, glow.filter(ImageFilter.GaussianBlur(70)))

    # 멀리 마을 실루엣
    sil = Image.new('RGBA', (AW, H), (0, 0, 0, 0))
    sd = ImageDraw.Draw(sil)
    base = H - 250
    x = -20
    while x < AW:
        w = random.randrange(70, 140)
        h = random.randrange(50, 120)
        sd.rectangle((x, base - h, x + w, H), fill=(14, 8, 16, 255))
        sd.polygon([(x - 8, base - h), (x + w / 2, base - h - random.randrange(40, 80)), (x + w + 8, base - h)], fill=(14, 8, 16, 255))
        for _ in range(random.randrange(0, 3)):
            wx = x + random.randrange(10, max(11, w - 20))
            wy = base - h + random.randrange(15, max(16, h - 20))
            sd.rectangle((wx, wy, wx + 10, wy + 14), fill=(255, 190, 80, 255))
        x += w + random.randrange(-10, 18)
    img.paste(sil, (0, 0), sil)

    # 솟아오르는 달궈진 검과 모루
    mid = AW // 2
    ground = H - 150
    blade_top, blade_bottom = 230, ground - 250
    hw = 34
    mask = Image.new('RGBA', (AW, H), (0, 0, 0, 0))
    ImageDraw.Draw(mask).polygon([(mid, blade_top), (mid + hw, blade_top + 90), (mid + hw, blade_bottom), (mid - hw, blade_bottom), (mid - hw, blade_top + 90)],
                                 fill=(255, 150, 50, 255))
    blade_glow = mask.filter(ImageFilter.GaussianBlur(26))
    img.paste(blade_glow, (0, 0), blade_glow)

    blade = Image.new('RGBA', (AW, H), (0, 0, 0, 0))
    bd = ImageDraw.Draw(blade)
    n = blade_bottom - blade_top
    for y in range(blade_top, blade_bottom):
        t = (y - blade_top) / n
        half = hw if y > blade_top + 90 else hw * (y - blade_top) / 90
        bd.line((mid - half, y, mid + half, y), fill=lerp((255, 250, 220), (255, 110, 30), t ** 0.8) + (255,))
    bd.line((mid, blade_top + 8, mid, blade_bottom), fill=(255, 255, 240, 220), width=4)
    bd.rectangle((mid - 110, blade_bottom, mid + 110, blade_bottom + 30), fill=(60, 40, 40, 255))
    bd.rectangle((mid - 110, blade_bottom, mid + 110, blade_bottom + 8), fill=(150, 110, 90, 255))
    bd.rectangle((mid - 20, blade_bottom + 30, mid + 20, blade_bottom + 140), fill=(70, 44, 36, 255))
    bd.ellipse((mid - 34, blade_bottom + 130, mid + 34, blade_bottom + 190), fill=(110, 76, 60, 255))
    img.paste(blade, (0, 0), blade)

    anv = ImageDraw.Draw(img)
    top = ground - 90
    anv.polygon([(mid - 330, top), (mid + 290, top), (mid + 380, top + 26), (mid + 240, top + 56), (mid + 130, top + 70),
                 (mid + 150, ground), (mid - 150, ground), (mid - 130, top + 70), (mid - 250, top + 50), (mid - 360, top + 34)], fill=(10, 8, 14))
    anv.line((mid - 320, top + 3, mid + 280, top + 3), fill=(120, 70, 50), width=3)

    # 불씨
    sparks = Image.new('RGBA', (AW, H), (0, 0, 0, 0))
    sp = ImageDraw.Draw(sparks)
    for _ in range(170):
        dist = random.uniform(30, 520)
        x = mid + math.cos(random.uniform(0, math.pi)) * dist * random.uniform(0.3, 1.1) * random.choice((-1, 1))
        y = ground - 130 - random.uniform(0, 700) * random.random()
        r = random.choice((2, 2, 3, 4))
        sp.ellipse((x - r, y - r, x + r, y + r), fill=(255, random.randrange(150, 230), 70, random.randrange(160, 255)))
    sparks = sparks.filter(ImageFilter.GaussianBlur(1))
    img.paste(sparks, (0, 0), sparks)

    # 도트처럼 단순화
    small = img.resize((AW // 3, H // 3), Image.BILINEAR).quantize(colors=48, method=Image.Quantize.MAXCOVERAGE, kmeans=2,
                                                                  dither=Image.Dither.NONE).convert('RGB')
    return small.resize((AW, H), Image.NEAREST)


def spine_strip():
    """공식 GBA 박스 왼쪽 띠를 따라 한다: 은색 금속 그라데이션, 위쪽 왼쪽 보라 모서리의 ONLY FOR,
    아래에서 위로 읽는 흰색 GAME BOY (둥근 굵은 글꼴, 부드러운 그림자)와 그 위의 진한 회색 ADVANCE (넓고 기울어진 굵은 글꼴)"""
    random.seed(3)
    strip = Image.new('RGB', (SPINE, H))
    d = ImageDraw.Draw(strip)
    for y in range(H):
        t = y / H
        # 위는 밝은 은색, 아래로 갈수록 회청색
        base = lerp((226, 228, 234), (150, 154, 168), min(1, t * 1.05))
        for x in range(SPINE):
            shade = 1.0 - 0.10 * (x / SPINE)                  # 오른쪽으로 살짝 어두워짐
            n = random.randint(-5, 5)
            d.point((x, y), fill=tuple(max(0, min(255, round(c * shade) + n)) for c in base))
    strip = strip.filter(ImageFilter.GaussianBlur(0.6))

    # 보라색 모서리 + ONLY FOR
    d = ImageDraw.Draw(strip)
    d.polygon([(0, 0), (SPINE * 0.98, 0), (0, SPINE * 1.2)], fill=(66, 44, 160))
    tag = Image.new('RGBA', (240, 80), (0, 0, 0, 0))
    td = ImageDraw.Draw(tag)
    f = ImageFont.truetype(ARIAL, 36)
    td.text((120, 24), 'ONLY', font=f, fill=(255, 255, 255, 255), anchor='mm')
    td.text((120, 58), 'FOR', font=f, fill=(255, 255, 255, 255), anchor='mm')
    tag = tag.rotate(45, expand=True, resample=Image.BICUBIC)
    strip.paste(tag.resize((tag.width * 3 // 5, tag.height * 3 // 5), Image.LANCZOS), (-8, 4), tag.resize((tag.width * 3 // 5, tag.height * 3 // 5), Image.LANCZOS))

    # 글자: 가로로 그린 뒤 반시계 90도 회전 (아래에서 위로 읽음)
    txt = Image.new('RGBA', (H, SPINE), (0, 0, 0, 0))
    t1 = ImageFont.truetype('C:/Windows/Fonts/seguibl.ttf', 70)        # GAME BOY: 둥글고 굵은 글꼴
    t2 = ImageFont.truetype('C:/Windows/Fonts/ariblk.ttf', 78)         # ADVANCE: 넓고 굵은 글꼴
    cy = SPINE // 2 + 6
    # GAME BOY (흰색) + 부드러운 그림자
    x1 = 70
    shadow = Image.new('RGBA', txt.size, (0, 0, 0, 0))
    ImageDraw.Draw(shadow).text((x1 + 4, cy + 5), 'GAME BOY', font=t1, fill=(70, 74, 96, 200), anchor='lm')
    shadow = shadow.filter(ImageFilter.GaussianBlur(3))
    txt.alpha_composite(shadow)
    ImageDraw.Draw(txt).text((x1, cy), 'GAME BOY', font=t1, fill=(250, 250, 252, 255), anchor='lm')
    w1 = ImageDraw.Draw(txt).textlength('GAME BOY', font=t1)
    ImageDraw.Draw(txt).text((x1 + w1 + 4, cy - 34), '(R)', font=ImageFont.truetype(ARIAL, 18), fill=(110, 112, 128, 255), anchor='lm')
    # ADVANCE (진한 회색) : 기울여서
    adv = Image.new('RGBA', (560, SPINE + 40), (0, 0, 0, 0))
    ImageDraw.Draw(adv).text((20, adv.height // 2), 'ADVANCE', font=t2, fill=(40, 42, 54, 255), anchor='lm')
    adv = adv.transform(adv.size, Image.AFFINE, (1, 0.22, -0.22 * adv.height / 2, 0, 1, 0), resample=Image.BICUBIC)
    hl = Image.new('RGBA', adv.size, (0, 0, 0, 0))
    ImageDraw.Draw(hl).text((22, adv.height // 2 - 3), 'ADVANCE', font=t2, fill=(120, 124, 140, 140), anchor='lm')
    txt.alpha_composite(hl.transform(hl.size, Image.AFFINE, (1, 0.22, -0.22 * hl.height / 2, 0, 1, 0), resample=Image.BICUBIC), (x1 + int(w1) + 40, cy - adv.height // 2))
    txt.alpha_composite(adv, (x1 + int(w1) + 40, cy - adv.height // 2))
    rot = txt.transpose(Image.ROTATE_90)
    # ROTATE_90은 반시계: 왼쪽이 아래가 된다
    strip.paste(rot, (0, 0), rot)
    return strip


def main():
    os.makedirs(OUT, exist_ok=True)
    cover = Image.new('RGB', (W, H), (8, 8, 12))
    cover.paste(background(), (SPINE, 0))
    cover.paste(spine_strip(), (0, 0))

    logo = key_out_black(Image.open(os.path.join(T, 'logo_source.png')))
    logo = logo.crop(logo.getbbox())
    lw = round(AW * 0.84)
    logo = logo.resize((lw, round(logo.height * lw / logo.width)), Image.LANCZOS)
    shadow = Image.new('RGBA', logo.size, (0, 0, 0, 0))
    shadow.paste(Image.new('RGBA', logo.size, (0, 0, 0, 210)), (0, 0), logo.getchannel('A'))
    shadow = shadow.filter(ImageFilter.GaussianBlur(14))
    pos = (SPINE + (AW - lw) // 2, 36)
    cover.paste(shadow, (pos[0] + 4, pos[1] + 10), shadow)
    cover.paste(logo, pos, logo)

    # 아래 띠: JAYS LOFT 로고 + 문구
    bar = Image.new('RGBA', (AW, 120), (6, 4, 8, 215))
    cover.paste(bar, (SPINE, H - 120), bar)
    jl = key_out_black(Image.open(os.path.join(ROOT, 'assets', 'graphics', 'splash', 'jays_loft_logo.webp')).convert('RGBA'))
    jl = jl.crop(jl.getbbox())
    jh = 96
    jl = jl.resize((round(jl.width * jh / jl.height), jh), Image.LANCZOS)
    cover.paste(jl, (SPINE + 28, H - 110), jl)
    d = ImageDraw.Draw(cover)
    d.text((SPINE + AW - 28, H - 70), 'Unofficial fan game.', font=ImageFont.truetype(ARIAL, 26), fill=(214, 204, 192), anchor='rm')
    d.text((SPINE + AW - 28, H - 36), 'Original concept (c) NBS.', font=ImageFont.truetype(ARIAL, 22), fill=(170, 160, 150), anchor='rm')
    cover.save(os.path.join(OUT, 'cover2.png'))
    cover.resize((W // 2, H // 2), Image.LANCZOS).save(os.path.join(OUT, 'cover2_small.png'))


if __name__ == '__main__':
    main()
