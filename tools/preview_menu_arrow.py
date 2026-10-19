"""메뉴 노란 화살표 시안 (build/menu_arrow_mock/): 화살표는 고정, 고른 메뉴 글자만 위아래로 흔들린다 (정지 화면에서는 위로 올라간 순간을 보여 준다).
대상: 시작 화면(STORY / HARD), 일시정지(STORY / HARD), OPTION. 사용: python -B tools/preview_menu_arrow.py
"""
import os
import sys

from PIL import Image, ImageDraw, ImageEnhance

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
from make_recipe_paper import GameFont

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
OUT = os.path.join(ROOT, 'build', 'menu_arrow_mock')
WHITE = (255, 255, 255)
BLACK = (0, 0, 0)
GRAY = (150, 150, 150)
RED = (255, 60, 60)
GOLD = (255, 214, 96)
BOB = 2


def dark_bg():
    return Image.new('RGB', (240, 160), (30, 24, 26))


def gray_bg():
    bg = Image.open(os.path.join(ROOT, 'graphics', 'forge_bg_night.bmp')).convert('RGB').crop((0, 0, 240, 160))
    bg = ImageEnhance.Color(bg).enhance(0.0)
    return ImageEnhance.Brightness(bg).enhance(0.45)


def centered(font, img, text, y, color=WHITE):
    font.draw(img, text, 120 - font.width(text) // 2, y, color, BLACK)


# 도트 화살표 (본문 7칸 + 외곽선 = 가로 6 x 세로 9). 글자 줄의 가운데(영문 대문자 높이 5~11행의 중심)에 정확히 맞춘다.
ARROW_ROWS = [1, 2, 3, 4, 3, 2, 1]


def arrow_pixels(left, center_y):
    """(x, y, 색) 목록: 본문 + 1px 검은 외곽선"""
    top = center_y - 4
    body = {}
    for r, w in enumerate(ARROW_ROWS):
        for x in range(w):
            body[(left + x, top + r)] = (255, 244, 170) if (r < 3 and x == 0) else GOLD
    result = dict(body)
    for (x, y) in list(body):
        for dx, dy in ((-1, 0), (1, 0), (0, -1), (0, 1), (-1, -1), (1, -1), (-1, 1), (1, 1)):
            if (x + dx, y + dy) not in body:
                result.setdefault((x + dx, y + dy), BLACK)
    return result


def menu(font, img, items, top_y, step, selected):
    """items: (글자, 색). 화살표는 고른 줄의 왼쪽에 고정, 글자만 BOB만큼 올라간다."""
    for i, (text, color) in enumerate(items):
        y = top_y + i * step
        if i == selected:
            for (ax, ay), arrow_color in arrow_pixels(120 - font.width(text) // 2 - 14, y).items():      # 화살표: 고정
                img.putpixel((ax, ay), arrow_color)
            y -= BOB                                                                    # 글자: 흔들리는 순간
        centered(font, img, text, y, color)


def screen_start(font, hard):
    img = dark_bg()
    centered(font, img, 'START', 26)
    if hard:
        menu(font, img, [('NEW GAME', WHITE)], 50, 18, 0)
        centered(font, img, 'HARD 모드는 저장 할 수 없습니다.', 86, RED)
    else:
        menu(font, img, [('NEW GAME', WHITE), ('SLOT 1  비어 있음', WHITE), ('SLOT 2  STORY +3  $9.9K', WHITE), ('SLOT 3  비어 있음', WHITE)], 50, 18, 0)
    font.draw(img, 'A 선택  B 돌아가기', 6, 148, WHITE, BLACK)
    return img


def screen_pause(font, hard):
    img = gray_bg()
    centered(font, img, 'PAUSE', 36)
    if hard:
        centered(font, img, 'HARD - 저장 불가', 53, RED)
    menu(font, img, [('Resume', WHITE), ('Save', GRAY if hard else WHITE), ('Quit', WHITE)], 72, 20, 0)
    font.draw(img, 'A 선택  B 돌아가기', 6, 148, WHITE, BLACK)
    return img


def screen_option(font):
    img = gray_bg()
    centered(font, img, 'OPTION', 36)
    menu(font, img, [('Record', WHITE), ('Gallery', WHITE), ('Language', WHITE)], 72, 20, 1)
    font.draw(img, 'A 선택  B 돌아가기', 6, 148, WHITE, BLACK)
    return img


def main():
    os.makedirs(OUT, exist_ok=True)
    font = GameFont('hs_font.bmp', 'hs_font.h', False)
    screens = [('start_story', screen_start(font, False)), ('start_hard', screen_start(font, True)),
               ('pause_story', screen_pause(font, False)), ('pause_hard', screen_pause(font, True)),
               ('option', screen_option(font))]
    sheet = Image.new('RGB', (3 * 490 - 10, 2 * 330 - 10), (60, 60, 66))
    for i, (name, img) in enumerate(screens):
        img.save(os.path.join(OUT, name + '.png'))
        sheet.paste(img.resize((480, 320), Image.NEAREST), ((i % 3) * 490, (i // 3) * 330))
    sheet.save(os.path.join(OUT, 'sheet.png'))
    print('ok')


if __name__ == '__main__':
    main()
