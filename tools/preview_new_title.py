"""새 타이틀 시안 (AI 원본 PNG → 240x160). build/title_preview/ 에 저장한다 (게임에는 아직 적용하지 않음).

배경 3장(아침·노을·밤)은 위쪽 이음선을 잘라 3:2로 맞추고 오른쪽 아래 Gemini 마크를 지운 뒤 줄인다.
로고는 검은 바탕을 지워 (가장자리에서 이어진 검정만) 얹는다. 메뉴 판자는 자리만 보는 임시 그림.
"""
import os
from collections import deque

from PIL import Image, ImageDraw, ImageFont

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
SRC = os.path.join(ROOT, 'assets', 'graphics', 'title')
OUT = os.path.join(ROOT, 'build', 'title_preview')

BGS = [('morning', 'bg_morning_src.png'), ('sunset', 'bg_sunset_src.png'), ('night', 'bg_night_src.png')]
TOP_CUT = 68          # 위쪽 이음선(밝기 띠) 높이
LEFT_CUT = 94         # 3:2로 맞추려고 왼쪽에서 자르는 폭
LOGO_WIDTH = 140
LOGO_POS = (14, 9)         # 대장간 집(오른쪽)을 가리지 않게 왼쪽 위


def prepare_bg(path):
    im = Image.open(path).convert('RGB')
    w, h = im.size
    # 오른쪽 아래 반짝이 마크: 왼쪽 같은 높이 그림으로 덮는다
    box = (1105, 700, 1185, 765)
    patch = im.crop((box[0] - 160, box[1], box[2] - 160, box[3]))
    im.paste(patch, (box[0], box[1]))
    im = im.crop((LEFT_CUT, TOP_CUT, w, h))
    return im.resize((240, 160), Image.LANCZOS)


def key_out_black(logo):
    """가장자리에서 이어진 거의 검정 픽셀만 투명으로 (글자 안쪽 어두운 부분은 남긴다)"""
    logo = logo.convert('RGBA')
    w, h = logo.size
    px = logo.load()
    seen = [[False] * h for _ in range(w)]
    dq = deque()
    for x in range(w):
        for y in (0, h - 1):
            dq.append((x, y))
    for y in range(h):
        for x in (0, w - 1):
            dq.append((x, y))
    while dq:
        x, y = dq.popleft()
        if not (0 <= x < w and 0 <= y < h) or seen[x][y]:
            continue
        r, g, b, a = px[x, y]
        if r + g + b > 60:
            continue
        seen[x][y] = True
        px[x, y] = (0, 0, 0, 0)
        dq.extend(((x + 1, y), (x - 1, y), (x, y + 1), (x, y - 1)))
    return logo


def menu_mock(img):
    d = ImageDraw.Draw(img)
    font = ImageFont.load_default()
    labels = ['STORY', 'HARD', 'OPTION']
    for i, t in enumerate(labels):
        y = 100 + i * 17
        d.rectangle((150, y, 226, y + 13), fill=(104, 70, 40), outline=(40, 24, 14))
        d.line((151, y + 1, 225, y + 1), fill=(140, 98, 58))
        d.text((188 - len(t) * 3, y + 2), t, fill=(240, 226, 196), font=font)
    return img


def main():
    os.makedirs(OUT, exist_ok=True)
    logo = key_out_black(Image.open(os.path.join(SRC, 'logo_source.png')))
    bbox = logo.getbbox()
    logo = logo.crop(bbox)
    lh = round(logo.size[1] * LOGO_WIDTH / logo.size[0])
    logo = logo.resize((LOGO_WIDTH, lh), Image.LANCZOS)
    print('logo', logo.size)
    sheet = Image.new('RGB', (240 * 3 * 2 + 20, 160 * 2), (30, 30, 30))
    for i, (name, f) in enumerate(BGS):
        bg = prepare_bg(os.path.join(SRC, f))
        bg.save(os.path.join(OUT, 'bg_%s.png' % name))
        comp = bg.convert('RGBA')
        comp.alpha_composite(logo, LOGO_POS)
        comp = comp.convert('RGB')
        comp = menu_mock(comp)
        comp = comp.quantize(colors=64, method=Image.Quantize.MAXCOVERAGE, kmeans=3, dither=Image.Dither.NONE).convert('RGB')
        comp.save(os.path.join(OUT, 'title_%s.png' % name))
        sheet.paste(comp.resize((480, 320), Image.NEAREST), (i * 490, 0))
    sheet = sheet.crop((0, 0, 3 * 490 - 10, 320))
    sheet.save(os.path.join(OUT, 'sheet.png'))


if __name__ == '__main__':
    main()
