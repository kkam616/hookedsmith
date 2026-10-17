"""재료 강화비 표시 시안 (게임에는 아직 적용하지 않는다).

지금은 강화비 값 줄에 "아이콘 + x개수"만 나와서 어떤 검·재료인지 알아보기 어렵다. 세 가지 대안을 같은 장면에서 비교한다.
 현재: 강화비 / [아이콘] x1
 A 이름 + 개수: 라벨 자리에 이름("+19 검", "도끼 가루"), 값 자리에 "x1" (아이콘 없음)
 B 아이콘 + 이름 줄: 강화비 / [아이콘] 이름 / x1   (한 줄 더)
 C 이름 + 아이콘 + 개수: 라벨 "필요", 값 줄에 [아이콘] x1, 그 아래 작은 이름 줄
출력: build/cost_layout_preview/cost_layouts.png
수동 실행: python -B tools/preview_cost_layout.py
"""

import os
import sys

from PIL import Image, ImageDraw, ImageFont

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import preview_sell_fx as base

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
OUT = os.path.join(ROOT, 'build', 'cost_layout_preview')
SCALE = 3

WHITE = (255, 255, 255)
RED = (255, 90, 90)
GOLD = (255, 214, 96)
GRAY = (200, 200, 208)

CENTER_X = 172          # 강화비 줄 가운데 (화면 기준)
LABEL_Y = 64
VALUE_Y = 78

CASES = [
    ('+21 → 22', 'sword', 19, 1, '+19 검', RED),
    ('+23 → 24', 'material', 'evil_soul', 8, '악령 혼', WHITE),
    ('+25 → 26', 'material', 'axe_dust', 15, '도끼 가루', WHITE),
]
FONT12 = ImageFont.truetype(r'C:\Windows\Fonts\gulim.ttc', 12, index=0)
FONT10 = ImageFont.truetype(r'C:\Windows\Fonts\gulim.ttc', 10, index=0)


def outline_text(draw, text, center_x, center_y, font, fill=WHITE):
    width = draw.textlength(text, font=font)
    x = center_x - width / 2
    y = center_y - font.size / 2 - 1
    for dx in (-1, 0, 1):
        for dy in (-1, 0, 1):
            draw.text((x + dx, y + dy), text, font=font, fill=(0, 0, 0))
    draw.text((x, y), text, font=font, fill=fill)
    return width


def icon_for(kind, key):
    name = 'ico_sword_%02d.bmp' % key if kind == 'sword' else 'ico_%s.bmp' % key
    return base.load_rgba(os.path.join(ROOT, 'graphics', name))


def draw_case(scene, option, case):
    image = scene.copy()
    draw = ImageDraw.Draw(image)
    title, kind, key, count, name, name_color = case
    icon = icon_for(kind, key)

    if option == 'now':
        outline_text(draw, '강화비', CENTER_X, LABEL_Y, FONT12)
        text = 'x%d' % count
        width = draw.textlength(text, font=FONT12)
        total = 16 + 3 + width
        start = CENTER_X - total / 2
        image.alpha_composite(icon, (int(start), VALUE_Y - 8))
        outline_text(draw, text, start + 19 + width / 2, VALUE_Y, FONT12)

    elif option == 'A':
        outline_text(draw, name, CENTER_X, LABEL_Y, FONT12, name_color)
        outline_text(draw, 'x%d' % count, CENTER_X, VALUE_Y, FONT12, GOLD)

    elif option == 'B':
        outline_text(draw, '강화비', CENTER_X, LABEL_Y - 2, FONT12)
        image.alpha_composite(icon, (CENTER_X - 28, VALUE_Y - 9))
        outline_text(draw, name, CENTER_X + 12, VALUE_Y - 1, FONT10, name_color)
        outline_text(draw, 'x%d' % count, CENTER_X + 12, VALUE_Y + 10, FONT10, GOLD)

    elif option == 'C':
        outline_text(draw, '필요', CENTER_X, LABEL_Y - 2, FONT12)
        text = 'x%d' % count
        width = draw.textlength(text, font=FONT12)
        total = 16 + 3 + width
        start = CENTER_X - total / 2
        image.alpha_composite(icon, (int(start), VALUE_Y - 8))
        outline_text(draw, text, start + 19 + width / 2, VALUE_Y, FONT12, GOLD)
        outline_text(draw, name, CENTER_X, VALUE_Y + 14, FONT10, name_color)

    return image.convert('RGB').crop((128, 50, 218, 112))


def main():
    os.makedirs(OUT, exist_ok=True)
    scene = base.base_scene()
    sword = base.sword_image(21)
    scene.alpha_composite(sword, (104, 42))

    options = [('now', '지금'), ('A', 'A 이름 + 개수'), ('B', 'B 아이콘 + 이름 줄'), ('C', 'C 필요 + 아이콘 + 이름')]
    cell_w, cell_h = 90 * SCALE, 62 * SCALE
    sheet = Image.new('RGB', (cell_w * len(options) + 10 * (len(options) + 1), cell_h * len(CASES) + 30 + 10 * (len(CASES) + 1)), (24, 24, 30))
    d = ImageDraw.Draw(sheet)

    for col, (option, title) in enumerate(options):
        d.text((10 + col * (cell_w + 10), 8), title, fill=WHITE)
        for row, case in enumerate(CASES):
            cell = draw_case(scene, option, case).resize((cell_w, cell_h), Image.NEAREST)
            sheet.paste(cell, (10 + col * (cell_w + 10), 30 + 10 + row * (cell_h + 10)))

    sheet.save(os.path.join(OUT, 'cost_layouts.png'))
    print('출력:', OUT)
    return 0


if __name__ == '__main__':
    sys.exit(main())
