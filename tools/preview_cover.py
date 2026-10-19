"""GBA 박스 앞면 시안 (build/cover/). 약 1:1 비율 + 왼쪽 검은 띠. 게임 그림(타이틀 배경·컷씬)과 로고를 재사용한다.
실제 인증 마크(닌텐도·ESRB 등)는 흉내 내지 않는다. 사용: python -B tools/preview_cover.py
"""
import os
from PIL import Image, ImageDraw, ImageFont, ImageFilter, ImageEnhance

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
T = os.path.join(ROOT, 'assets', 'graphics', 'title')
C = os.path.join(ROOT, 'assets', 'graphics', 'cutscene')
OUT = os.path.join(ROOT, 'build', 'cover')
W, H = 1050, 1040
SPINE = 96
ART_W = W - SPINE
FONT = 'C:/Windows/Fonts/arialbd.ttf'
KFONT = 'C:/Windows/Fonts/malgunbd.ttf'


def key_logo():
    from preview_new_title import key_out_black
    logo = key_out_black(Image.open(os.path.join(T, 'logo_source.png')))
    return logo.crop(logo.getbbox())


def vignette(img, strength=0.55):
    w, h = img.size
    mask = Image.new('L', (w, h), 0)
    d = ImageDraw.Draw(mask)
    d.ellipse((-w * 0.25, -h * 0.2, w * 1.25, h * 1.2), fill=255)
    mask = mask.filter(ImageFilter.GaussianBlur(w * 0.12))
    dark = Image.new('RGB', (w, h), (6, 4, 8))
    return Image.composite(img, Image.blend(img, dark, strength), mask)


def fit_cover(img, size, focus_x=0.5, focus_y=0.5):
    tw, th = size
    r = max(tw / img.width, th / img.height)
    img = img.resize((round(img.width * r), round(img.height * r)), Image.LANCZOS)
    x = min(max(round(img.width * focus_x - tw / 2), 0), img.width - tw)
    y = min(max(round(img.height * focus_y - th / 2), 0), img.height - th)
    return img.crop((x, y, x + tw, y + th))


def make(art_path, focus, name, tagline=None, logo_y=40, art_crop=None):
    art = Image.open(art_path).convert('RGB')
    if os.path.basename(art_path).startswith('bg_'):      # 오른쪽 아래 Gemini 마크를 왼쪽 그림으로 덮는다 (1264x848 원본 기준)
        box = (1105, 700, 1185, 765)
        art.paste(art.crop((box[0] - 160, box[1], box[2] - 160, box[3])), (box[0], box[1]))
    if art_crop:
        art = art.crop(art_crop)
    art = fit_cover(art, (ART_W, H), *focus)
    art = vignette(ImageEnhance.Contrast(art).enhance(1.08))
    cover = Image.new('RGB', (W, H), (8, 8, 12))
    cover.paste(art, (SPINE, 0))

    # 왼쪽 검은 띠: 세로 글자
    d = ImageDraw.Draw(cover)
    for y in range(H):
        shade = 14 + int(10 * (y / H))
        d.line((0, y, SPINE - 1, y), fill=(shade, shade, shade + 6))
    d.line((SPINE - 3, 0, SPINE - 3, H), fill=(150, 110, 50), width=3)
    spine = Image.new('RGBA', (H, SPINE), (0, 0, 0, 0))
    sd = ImageDraw.Draw(spine)
    sd.text((H // 2, SPINE // 2), 'HOOKED SMITH   -   FOR GBA', font=ImageFont.truetype(FONT, 46), fill=(216, 170, 96, 255), anchor='mm')
    cover.paste(spine.rotate(90, expand=True), (0, 0), spine.rotate(90, expand=True))

    # 로고
    logo = key_logo()
    lw = round(ART_W * 0.84)
    logo = logo.resize((lw, round(logo.height * lw / logo.width)), Image.LANCZOS)
    glow = Image.new('RGBA', logo.size, (0, 0, 0, 0))
    shadow = Image.new('RGBA', logo.size, (0, 0, 0, 200))
    glow.paste(shadow, (0, 0), logo.getchannel('A'))
    glow = glow.filter(ImageFilter.GaussianBlur(14))
    pos = (SPINE + (ART_W - lw) // 2, logo_y)
    cover.paste(glow, (pos[0] + 4, pos[1] + 10), glow)
    cover.paste(logo, pos, logo)

    # 아래: 문구 띠
    bar = Image.new('RGBA', (ART_W, 84), (8, 6, 10, 200))
    cover.paste(bar, (SPINE, H - 84), bar)
    d = ImageDraw.Draw(cover)
    if tagline:
        d.text((SPINE + ART_W // 2, H - 56), tagline, font=ImageFont.truetype(KFONT, 40), fill=(255, 214, 140), anchor='mm')
    d.text((SPINE + ART_W // 2, H - 22), 'Unofficial fan game.  Original concept (c) NBS.  A Jay\'s Loft production.', font=ImageFont.truetype(FONT, 20), fill=(190, 180, 170), anchor='mm')

    cover.save(os.path.join(OUT, name + '.png'))
    return cover


def main():
    os.makedirs(OUT, exist_ok=True)
    import sys
    sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
    a = make(os.path.join(T, 'bg_sunset_src.png'), (0.62, 0.55), 'cover_a_sunset', '한 번만 더...', art_crop=(0, 70, 1264, 848))
    b = make(os.path.join(T, 'bg_night_src.png'), (0.68, 0.55), 'cover_b_night', '한 번만 더...', art_crop=(0, 70, 1264, 848))
    c = make(os.path.join(C, 'Gemini_Generated_Image_mbmxafmbmxafmbmx.png'), (0.5, 0.62), 'cover_c_door', '한 번만 더...')
    sheet = Image.new('RGB', (3 * 520 + 20, 515), (30, 30, 34))
    for i, im in enumerate((a, b, c)):
        sheet.paste(im.resize((520, round(H * 520 / W))), (i * 530, 0))
    sheet.save(os.path.join(OUT, 'sheet.png'))


if __name__ == '__main__':
    main()
