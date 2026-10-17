"""레시피 시안 (게임에는 아직 적용하지 않는다).

강화 화면에서 SELECT(도움말) → START를 누르면 열리는 "낡은 종이 위에 적은 레시피"의 모의 화면.
단계마다 필요한 것(돈 또는 재료) / 성공률 / 판매가 (STORY·HARD 값이 각각 다르다). ↑↓ 한 줄씩 넘기고(스크롤), B/START 닫기. 화구 교환 쪽과 노트 줄은 뺐다.
표 값은 docs/GAME_MANUAL.md의 자동 표(<!-- AUTO:... -->)에서 읽는다.

출력: build/recipe_note_preview/ (stills.png, scroll_story.gif)
수동 실행: python -B tools/preview_recipe_note.py
"""

import os
import random
import re
import sys

from PIL import Image, ImageDraw, ImageFilter, ImageFont

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
MANUAL = os.path.join(ROOT, 'docs', 'GAME_MANUAL.md')
OUT = os.path.join(ROOT, 'build', 'recipe_note_preview')
W, H = 240, 160
SCALE = 4

INK = (62, 36, 20)
INK_FAINT = (150, 112, 72)
RED_INK = (150, 36, 28)
PAPER = (218, 192, 142)

FONT = ImageFont.truetype(r'C:\Windows\Fonts\gulim.ttc', 12, index=0)
SMALL = ImageFont.truetype(r'C:\Windows\Fonts\gulim.ttc', 10, index=0)

ROWS_VISIBLE = 8
ROW_Y0 = 40
ROW_STEP = 13


# ── 표 읽기 ──────────────────────────────────────────────────────────────
def read_block(name):
    text = open(MANUAL, encoding='utf-8').read()
    return text.split('<!-- AUTO:%s -->' % name)[1].split('<!-- /AUTO:%s -->' % name)[0]


def short_money(text):
    """"$1,010,000" → "$1M", "$142,000" → "$142K" (게임의 format_money_short와 같은 규칙)"""
    match = re.match(r'\$([\d,]+)', text.strip())
    if not match:
        return text.strip()
    value = int(match.group(1).replace(',', ''))
    if value >= 1_000_000:
        amount = value / 1_000_000
        return '$%gM' % round(amount, 1)
    if value >= 1_000:
        amount = value / 1_000
        return '$%gK' % round(amount, 1)
    return '$%d' % value


def cost_text(text):
    text = text.strip()
    if text.startswith('$'):
        return short_money(text)
    if text.startswith('보관 '):
        return text.replace('보관 ', '').replace('검 ×', '검 x').replace('×', 'x')
    return text.replace('×', 'x')


def level_rows(mode):
    rows = []
    current = None
    for line in read_block('levels').splitlines():
        cells = [c.strip() for c in line.strip().strip('|').split('|')]
        if len(cells) < 8 or cells[0] in ('단계', '---'):
            continue
        if cells[0].startswith('---'):
            continue
        if cells[0]:
            current = cells[0]
        if cells[2] == mode:
            rows.append((current, cost_text(cells[4]), cells[3], short_money(cells[5]) if cells[5].startswith('$') else '-'))
    return rows


def recipe_rows(mode):
    rows = []
    for line in read_block('recipes').splitlines():
        cells = [c.strip() for c in line.strip().strip('|').split('|')]
        if len(cells) < 5 or cells[0] in ('재료',) or cells[0].startswith('---'):
            continue
        need = cells[1] if mode == 'STORY' else cells[2]
        result = (cells[3] if mode == 'STORY' else cells[4]).replace(' (창고로)', '')
        rows.append((cells[0], need, result))
    return rows


# ── 종이 ──────────────────────────────────────────────────────────────────
def paper_background():
    rng = random.Random(11)
    image = Image.new('RGB', (W, H), PAPER)
    px = image.load()
    for y in range(H):
        for x in range(W):
            n = rng.randint(-9, 9)
            r, g, b = PAPER
            px[x, y] = (r + n, g + n, b + n)

    draw = ImageDraw.Draw(image)
    # 얼룩 (커피 자국, 물 자국)
    for cx, cy, radius, color in ((190, 120, 26, (196, 164, 112)), (40, 20, 18, (206, 176, 124)), (210, 30, 12, (200, 170, 118))):
        draw.ellipse((cx - radius, cy - radius, cx + radius, cy + radius), outline=color, width=2)
    image = image.filter(ImageFilter.GaussianBlur(0.6))
    draw = ImageDraw.Draw(image)


    # 가장자리 그을림 + 접힌 자국
    px = image.load()
    for y in range(H):
        for x in range(W):
            edge = min(x, y, W - 1 - x, H - 1 - y)
            if edge < 14:
                k = (14 - edge) / 14
                r, g, b = px[x, y]
                px[x, y] = (int(r - 70 * k * k), int(g - 82 * k * k), int(b - 76 * k * k))
    draw.line((W // 2, 0, W // 2, H), fill=(196, 168, 120))
    return image


PAPER_IMAGE = paper_background()


# ── 글 ────────────────────────────────────────────────────────────────────
def ink(draw, xy, text, font=FONT, fill=INK, anchor='la'):
    x, y = xy
    draw.text((x + 1, y + 1), text, font=font, fill=INK_FAINT, anchor=anchor)       # 번진 잉크
    draw.text((x, y), text, font=font, fill=fill, anchor=anchor)


def draw_rows(draw, page, mode, offset):
    if page == 0:
        rows = level_rows(mode)
        ink(draw, (30, 24), '단계', SMALL, INK_FAINT)
        ink(draw, (84, 24), '필요한 것', SMALL, INK_FAINT)
        ink(draw, (186, 24), '성공', SMALL, INK_FAINT, 'ra')
        ink(draw, (226, 24), '판매가', SMALL, INK_FAINT, 'ra')
        for i in range(ROWS_VISIBLE):
            index = offset + i
            if index >= len(rows):
                break
            level, need, success, sell = rows[index]
            y = ROW_Y0 + i * ROW_STEP + (index * 7 % 3 - 1) * 0       # 손글씨 느낌: 줄마다 아주 살짝 흔들리게 하려면 여기를 조정
            material = not need.startswith('$') and need != '무료'
            ink(draw, (30, y), level, FONT, RED_INK if material else INK)
            ink(draw, (84, y), need, FONT, RED_INK if material else INK)
            ink(draw, (186, y), success, FONT, INK, 'ra')
            ink(draw, (226, y), sell, FONT, INK, 'ra')
        return len(rows)

    rows = recipe_rows(mode)
    ink(draw, (30, 24), '모을 것', SMALL, INK_FAINT)
    ink(draw, (112, 24), '개수', SMALL, INK_FAINT)
    ink(draw, (144, 24), '바꾸면', SMALL, INK_FAINT)
    for i in range(ROWS_VISIBLE):
        index = offset + i
        if index >= len(rows):
            break
        name, need, result = rows[index]
        y = ROW_Y0 + i * ROW_STEP
        ink(draw, (30, y), name)
        ink(draw, (126, y), 'x' + need, FONT, INK, 'ra')
        ink(draw, (136, y), '→ ' + result, FONT, RED_INK if '검' in result else INK)
    return len(rows)


def screen(page, mode, offset):
    image = PAPER_IMAGE.copy()
    draw = ImageDraw.Draw(image)

    ink(draw, (12, 7), '레시피', FONT)
    ink(draw, (62, 9), '(%s)' % mode, SMALL, RED_INK)
    draw.line((12, 21, W - 12, 21), fill=INK_FAINT)

    total = draw_rows(draw, page, mode, offset)

    # 스크롤 표시: 오른쪽 가장자리에 ▲ ▼ 와 막대
    if offset > 0:
        ink(draw, (231, 31), '▲', SMALL)
    if offset + ROWS_VISIBLE < total:
        ink(draw, (231, 138), '▼', SMALL)
    track_top, track_bottom = 42, 132
    draw.line((234, track_top, 234, track_bottom), fill=INK_FAINT)
    thumb_h = max(8, int((track_bottom - track_top) * ROWS_VISIBLE / max(total, ROWS_VISIBLE)))
    thumb_y = track_top + int((track_bottom - track_top - thumb_h) * (offset / max(1, total - ROWS_VISIBLE)))
    draw.rectangle((233, thumb_y, 235, thumb_y + thumb_h), fill=INK)

    ink(draw, (12, 148), '↑↓ 넘기기     B 닫기', SMALL, INK_FAINT)
    return image


def main():
    os.makedirs(OUT, exist_ok=True)
    story_total = len(level_rows('STORY'))
    print('단계 표 STORY %d줄 / HARD %d줄, 화구 %d줄' % (story_total, len(level_rows('HARD')), len(recipe_rows('STORY'))))

    shots = [
        ('STORY 처음', screen(0, 'STORY', 0)),
        ('STORY 스크롤 (+21~)', screen(0, 'STORY', 21)),
        ('HARD 같은 자리', screen(0, 'HARD', 21)),
    ]

    sheet = Image.new('RGB', (W * 3 * 2 + 30, H * 2 + 50), (24, 24, 30))
    d = ImageDraw.Draw(sheet)
    for i, (title, image) in enumerate(shots):
        x = 10 + i * (W * 2 + 5)
        y = 24
        d.text((x, y - 16), title, fill=(255, 255, 255))
        sheet.paste(image.resize((W * 2, H * 2), Image.NEAREST), (x, y))
    sheet.save(os.path.join(OUT, 'stills.png'))

    frames = []
    for offset in list(range(0, story_total - ROWS_VISIBLE + 1)) + [story_total - ROWS_VISIBLE] * 6 + list(range(story_total - ROWS_VISIBLE, -1, -1)):
        for _ in range(4):
            frames.append(screen(0, 'STORY', offset).resize((W * SCALE, H * SCALE), Image.NEAREST))
    frames[0].save(os.path.join(OUT, 'scroll_story.gif'), save_all=True, append_images=frames[1:], duration=40, loop=0, optimize=False)
    print('출력:', OUT)
    return 0


if __name__ == '__main__':
    sys.exit(main())
