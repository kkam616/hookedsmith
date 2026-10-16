"""아이템 아이콘 변환기 (시안 단계: 미리보기만 만들고 게임에는 적용하지 않는다).

assets/graphics/items/item_icons_source.webp(2행 5열, 아래에 글자가 붙은 AI 일러스트)에서 아이콘을 잘라
14x14로 줄여 16x16 칸 가운데에 놓는다. 줄이면 뭉개지는 4개(낡은 고철, 불꽃 자루, 악령 혼, 도끼 가루)는
아래 HAND_DRAWN의 점 그림(14x14)을 쓴다. 아이콘마다 자기 15색 팔레트를 쓴다 (목록 화면에는 팔레트가 많이 필요 없다).
검은 배경은 투명으로 바꾼다.

수동 실행: python -B tools/make_item_icons.py
출력: build/items_preview/ (미리보기). graphics/에는 아직 아무것도 쓰지 않는다.
"""

import os
import sys

from PIL import Image, ImageDraw, ImageFont

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
SOURCE = os.path.join(ROOT, 'assets', 'graphics', 'items', 'item_icons_source.webp')
OUT_DIR = os.path.join(ROOT, 'build', 'items_preview')

# 시트의 칸: 열 가운데 x, 행 (위, 아래) y 범위. 글자가 붙은 아랫부분은 뺀다.
COLUMN_CENTERS = [163, 435, 700, 968, 1250]
COLUMN_HALF = 135
ROW_BANDS = [(190, 455), (585, 870)]
NAMES = [
    ('ticket', '깨짐 방지권'), ('scrap', '낡은 고철'), ('tau_bone', '타우 뼈'), ('faded_glow', '바랜 빛'),
    ('swiss_iron', '스위스 철'), ('flame_hilt', '불꽃 자루'), ('evil_soul', '악령 혼'), ('axe_dust', '도끼 가루'),
    ('clear_matter', '투명 물질'), ('warp', '워프권'),
]

ART = 14            # 그림이 차지하는 크기
CANVAS = 16         # 아이콘 한 칸
COLORS = 15         # 투명 제외
BLACK_CUT = 46      # 그림 바깥 상자를 찾을 때 배경으로 보는 밝기
GLOW_CUT = 70       # 줄인 뒤 이보다 어두운 점(번짐)은 투명 처리

# 직접 그린 점 그림 (14x14). '.'은 투명, 나머지 글자는 아래 색 표.
HAND_DRAWN = {
    # 불꽃 자루: 검 손잡이. 칼날 자리가 불꽃 빛으로 타오른다
    'flame_hilt': (
        {'o': (56, 26, 20), 'Y': (255, 238, 130), 'O': (255, 164, 54), 'R': (226, 64, 38), 'G': (238, 192, 72),
         'g': (176, 124, 42), 'b': (150, 84, 48), 'd': (98, 52, 30)},
        [
            '......oo......',
            '.....oYYo.....',
            '..R..oYYo..O..',
            '.....oOYo.....',
            '...R.oOYo.....',
            '.....oOOo..R..',
            '.....oORo.....',
            '.oooooRRooooo.',
            '.oGGGGGGGGGGo.',
            '..ooogGGgooo..',
            '......obbo....',
            '......obdo....',
            '.....oGGGGo...',
            '......oooo....',
        ]),
    # 악령 혼: 보랏빛 불꽃에 싸인 해골
    'evil_soul': (
        {'o': (40, 24, 70), 'p': (150, 90, 230), 'q': (206, 160, 255), 'W': (246, 244, 250), 'w': (206, 200, 224),
         'd': (36, 18, 60), 'n': (120, 100, 150)},
        [
            '.q..........q.',
            '.pq.oooooo.qp.',
            '..poWWWWWWopp.',
            '.poWWWWWWWWop.',
            '.poWddWWddWop.',
            '..oWddWWddWo..',
            '..oWWWnnWWWo..',
            '..poWWWWWWop..',
            '...oWwWWwWo...',
            '...oWdWWdWo...',
            '..ppoWdWdWopp.',
            '.pq.ooooooo.qp',
            '..q.pp..pp..q.',
            '.....p....p...',
        ]),
}


OUTLINE = (40, 36, 44)


def outline_around(image, mask):
    """불투명한 점 둘레(위·아래·좌·우)에 어두운 윤곽선을 두른다"""
    out_image = image.copy()
    out_mask = mask.copy()
    for y in range(ART):
        for x in range(ART):
            if not mask.getpixel((x, y)):
                for dx, dy in ((1, 0), (-1, 0), (0, 1), (0, -1)):
                    nx, ny = x + dx, y + dy
                    if 0 <= nx < ART and 0 <= ny < ART and mask.getpixel((nx, ny)):
                        out_image.putpixel((x, y), OUTLINE)
                        out_mask.putpixel((x, y), 255)
                        break
    return out_image, out_mask


def draw_scrap():
    """낡은 고철: 회색 철판 위에 녹슨 철판이 엇갈려 놓인 모양, 볼트 하나"""
    image = Image.new('RGB', (ART, ART), (0, 0, 0))
    mask = Image.new('L', (ART, ART), 0)
    d = ImageDraw.Draw(image)
    m = ImageDraw.Draw(mask)

    def shape(points, fill):
        d.polygon(points, fill=fill)
        m.polygon(points, fill=255)

    shape([(1, 4), (8, 1), (12, 4), (5, 7)], (150, 150, 164))                 # 회색 철판
    d.line([(1, 4), (8, 1), (12, 4)], fill=(214, 214, 226))                   # 윗 모서리 하이라이트
    d.line([(5, 7), (12, 4)], fill=(96, 94, 110))                             # 아랫 모서리 그림자
    shape([(2, 8), (10, 5), (12, 9), (5, 12)], (190, 100, 52))                # 녹슨 철판
    d.line([(2, 8), (10, 5)], fill=(236, 150, 74))
    d.line([(5, 12), (12, 9)], fill=(118, 62, 34))
    for point, color in (((7, 3), (92, 90, 104)), ((5, 9), (108, 56, 32)), ((9, 8), (236, 150, 74))):
        d.point(point, fill=color)
    return outline_around(image, mask)


def draw_axe_dust():
    """도끼 가루: 부서진 도끼 머리가 회색 가루 더미에 꽂혀 있는 모양"""
    image = Image.new('RGB', (ART, ART), (0, 0, 0))
    mask = Image.new('L', (ART, ART), 0)
    d = ImageDraw.Draw(image)
    m = ImageDraw.Draw(mask)

    def shape(points, fill):
        d.polygon(points, fill=fill)
        m.polygon(points, fill=255)

    shape([(1, 12), (12, 12), (10, 9), (7, 8), (4, 9)], (150, 150, 162))      # 가루 더미
    for point, color in (((3, 11), (200, 200, 212)), ((6, 10), (226, 226, 236)), ((9, 11), (112, 112, 126)),
                         ((5, 12), (96, 96, 110)), ((11, 12), (96, 96, 110)), ((8, 9), (200, 200, 212))):
        d.point(point, fill=color)

    d.line([(5, 9), (9, 3)], fill=(150, 84, 48), width=2)                     # 자루
    m.line([(5, 9), (9, 3)], fill=255, width=2)
    shape([(8, 1), (12, 2), (12, 6), (9, 6), (8, 4)], (168, 168, 182))        # 도끼 머리
    d.line([(12, 2), (12, 6)], fill=(236, 236, 246))                          # 날
    d.line([(8, 1), (11, 1)], fill=(214, 214, 226))
    d.point((9, 5), fill=(100, 100, 116))

    # 날리는 가루 몇 알
    for point in ((1, 8), (12, 9), (3, 6)):
        d.point(point, fill=(200, 200, 212))
        m.point(point, fill=255)

    return outline_around(image, mask)


DRAWN = {'scrap': draw_scrap, 'axe_dust': draw_axe_dust}


def cut_icons(sheet):
    icons = []
    for row in range(2):
        for col in range(5):
            x0 = COLUMN_CENTERS[col] - COLUMN_HALF
            cell = sheet.crop((x0, ROW_BANDS[row][0], x0 + COLUMN_HALF * 2, ROW_BANDS[row][1]))
            gray = cell.convert('L').point(lambda v: 255 if v > BLACK_CUT else 0)
            icons.append(cell.crop(gray.getbbox()))
    return icons


def to_square(icon):
    side = max(icon.size)
    canvas = Image.new('RGB', (side, side), (0, 0, 0))
    canvas.paste(icon, ((side - icon.width) // 2, (side - icon.height) // 2))
    return canvas


def shrink(icon):
    """ART x ART로 줄이고, 거의 검은 점은 투명으로 한다 (그림, 마스크)"""
    small = to_square(icon).resize((ART, ART), Image.LANCZOS)
    mask = small.convert('L').point(lambda v: 255 if v > GLOW_CUT else 0)
    return small, mask


def from_hand(name):
    colors, rows = HAND_DRAWN[name]
    assert len(rows) == ART and all(len(row) == ART for row in rows), name
    image = Image.new('RGB', (ART, ART), (0, 0, 0))
    mask = Image.new('L', (ART, ART), 0)
    for y, row in enumerate(rows):
        for x, char in enumerate(row):
            if char != '.':
                image.putpixel((x, y), colors[char])
                mask.putpixel((x, y), 255)
    return image, mask


def to_gba(color):
    return tuple(channel & 0xF8 for channel in color)


def make_icon(small, mask):
    """아이콘 하나만의 15색 팔레트로 줄여 16x16 RGBA로 만든다"""
    opaque = Image.new('RGB', (ART, ART), (0, 0, 0))
    opaque.paste(small, (0, 0), mask)
    quantized = opaque.quantize(colors=COLORS, method=Image.Quantize.MAXCOVERAGE, kmeans=3, dither=Image.Dither.NONE)
    raw = quantized.getpalette()
    rgb = quantized.convert('RGB')
    canvas = Image.new('RGBA', (CANVAS, CANVAS), (0, 0, 0, 0))
    offset = (CANVAS - ART) // 2
    used = set()

    for y in range(ART):
        for x in range(ART):
            if mask.getpixel((x, y)):
                color = to_gba(rgb.getpixel((x, y)))
                used.add(color)
                canvas.putpixel((x + offset, y + offset), color + (255,))

    return canvas, len(used)


def main():
    sheet = Image.open(SOURCE).convert('RGB')
    cut = cut_icons(sheet)
    os.makedirs(OUT_DIR, exist_ok=True)
    icons16 = []

    for (name, label), icon in zip(NAMES, cut):
        small, mask = DRAWN[name]() if name in DRAWN else from_hand(name) if name in HAND_DRAWN else shrink(icon)
        canvas, color_count = make_icon(small, mask)
        icons16.append(canvas)
        canvas.save(os.path.join(OUT_DIR, name + '_16.png'))
        print('%-13s %s colors=%d' % (name, '손그림' if name in HAND_DRAWN or name in DRAWN else '변환', color_count))

    if '--write' in sys.argv:
        from icon_export import export_icon
        for (name, label), icon in zip(NAMES, icons16):
            export_icon('ico_' + name, icon)
        print('graphics/ico_*.bmp 작성:', len(icons16))

    # 미리보기 1: 확대 시트 (검정 배경, 8배)
    scale = 8
    cols = 5
    sheet_out = Image.new('RGB', (cols * (CANVAS * scale + 16) + 16, 2 * (CANVAS * scale + 16) + 16), (0, 0, 0))
    for i, icon in enumerate(icons16):
        big = icon.resize((CANVAS * scale, CANVAS * scale), Image.NEAREST)
        sheet_out.paste(big, (16 + (i % cols) * (CANVAS * scale + 16), 16 + (i // cols) * (CANVAS * scale + 16)), big)
    sheet_out.save(os.path.join(OUT_DIR, 'preview_sheet_8x.png'))

    try:
        font = ImageFont.truetype(r'C:\Windows\Fonts\gulim.ttc', 12, index=0)
    except OSError:
        font = ImageFont.load_default()

    # 미리보기 2: 잡화 목록 흉내 (줄 간격 14, 240x160의 4배)
    mock = Image.new('RGB', (240, 160), (28, 20, 22))
    draw = ImageDraw.Draw(mock)
    draw.text((10, 4), '잡화', font=font, fill=(255, 255, 255))
    draw.text((170, 4), '방지권 10장', font=font, fill=(255, 255, 255))
    for i in range(8):
        y = 20 + i * 14 - 2
        mock.paste(icons16[1 + i], (20, y), icons16[1 + i])
        draw.text((40, y + 2), NAMES[1 + i][1], font=font, fill=(255, 255, 255))
        draw.text((170, y + 2), 'x10', font=font, fill=(255, 255, 255))
    mock.resize((960, 640), Image.NEAREST).save(os.path.join(OUT_DIR, 'preview_bag_list_4x.png'))

    # 미리보기 3: 화구 목록 흉내 (줄 간격 13, 9줄)
    mock = Image.new('RGB', (240, 160), (28, 20, 22))
    draw = ImageDraw.Draw(mock)
    draw.text((10, 4), '화구', font=font, fill=(255, 255, 255))
    recipes = [(1, '0/5', ticket := 0, 'x1'), (2, '0/3', 0, 'x1'), (3, '0/2', None, '+13 검'), (4, '0/3', 0, 'x2'),
               (5, '0/2', None, '+16 검'), (6, '0/4', 0, 'x4'), (6, '0/6', None, '+19 검'), (7, '0/6', 0, 'x10'),
               (8, '0/3', 0, 'x9')]
    for i, (material, count, out_icon, out_text) in enumerate(recipes):
        y = 17 + i * 13 - 2
        mock.paste(icons16[material], (6, y), icons16[material])
        draw.text((22, y + 2), NAMES[material][1], font=font, fill=(255, 255, 255))
        draw.text((100, y + 2), count, font=font, fill=(255, 255, 255))
        if out_icon is not None:
            mock.paste(icons16[0], (132, y), icons16[0])
            draw.text((150, y + 2), out_text, font=font, fill=(255, 255, 255))
        else:
            draw.text((134, y + 2), out_text, font=font, fill=(255, 255, 255))
    mock.resize((960, 640), Image.NEAREST).save(os.path.join(OUT_DIR, 'preview_craft_list_4x.png'))

    # 미리보기 4: 상점 목록 흉내 (줄 간격 16)
    mock = Image.new('RGB', (240, 160), (28, 20, 22))
    draw = ImageDraw.Draw(mock)
    draw.text((10, 4), '상점', font=font, fill=(255, 255, 255))
    rows = [(0, '방지권 1장', '$10,000'), (0, '방지권 3장', '$25,000'), (9, '워프 +9', '$10,000'),
            (9, '워프 +13', '$70,000'), (9, '워프 +14', '$100,000'), (9, '워프 +15', '$150,000')]
    for i, (icon, label, price) in enumerate(rows):
        y = 22 + i * 16 - 2
        mock.paste(icons16[icon], (16, y), icons16[icon])
        draw.text((36, y + 2), label, font=font, fill=(255, 255, 255))
        draw.text((150, y + 2), price, font=font, fill=(255, 255, 255))
    mock.resize((960, 640), Image.NEAREST).save(os.path.join(OUT_DIR, 'preview_shop_list_4x.png'))

    print('preview ->', OUT_DIR)
    return 0


if __name__ == '__main__':
    sys.exit(main())
