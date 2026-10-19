"""OPTION 메뉴 화면 시안 4장 (build/option_mock/): OPTION 목록, Record, Gallery, Language. 게임 글꼴 모양으로 그린다.
게임에는 아직 적용하지 않은 시안이다. 사용: python -B tools/preview_option_screens.py
"""
import os
import sys

from PIL import Image, ImageDraw, ImageEnhance

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
from make_recipe_paper import GameFont

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
OUT = os.path.join(ROOT, 'build', 'option_mock')
WHITE = (255, 255, 255)
BLACK = (0, 0, 0)
GRAY = (150, 150, 150)
RED = (255, 60, 60)
GOLD = (255, 214, 96)


def background():
    bg = Image.open(os.path.join(ROOT, 'graphics', 'forge_bg_night.bmp')).convert('RGB').crop((0, 0, 240, 160))
    bg = ImageEnhance.Color(bg).enhance(0.0)
    bg = ImageEnhance.Brightness(bg).enhance(0.45)
    return bg


def centered(font, image, text, y, body=WHITE, shadow=BLACK, cx=120):
    font.draw(image, text, cx - font.width(text) // 2, y, body, shadow)


def screen_option(font, selected=0):
    img = background()
    centered(font, img, 'OPTION', 36)
    items = ['Record', 'Gallery', 'Language']
    for i, name in enumerate(items):
        y = 72 + i * 20
        centered(font, img, name, y - (2 if i == selected else 0), WHITE if i == selected else (200, 200, 200))
        if i == selected:
            font.draw(img, '▶', 120 - font.width(name) // 2 - 18, y - 2, GOLD, BLACK)
    font.draw(img, 'A 선택  B 돌아가기', 6, 148, WHITE, BLACK)
    return img


def screen_record(font, small, empty=False):
    img = background()
    centered(font, img, 'RECORD', 14)
    d = ImageDraw.Draw(img)
    d.line((14, 26, 226, 26), fill=(120, 120, 120))
    centered(small, img, 'HARD 최고 도달 기록', 36, GOLD)
    if empty:
        centered(font, img, '아직 기록이 없습니다.', 82, GRAY)
    else:
        centered(font, img, '+27  오딧세이 소드', 56, (255, 90, 90))
        rows = [('강화', '212'), ('성공', '98'), ('실패', '114'), ('깨뜨린 검', '14'), ('방지권', '9'), ('번 돈', '$64K')]
        for i, (k, v) in enumerate(rows):
            col, row = i % 2, i // 2
            x0 = 20 + col * 108
            y = 84 + row * 18
            font.draw(img, k, x0, y, (200, 200, 200), BLACK)
            font.draw(img, v, x0 + 96, y, WHITE, BLACK, 'right')
        small.draw(img, '새 최고 단계에 닿으면 자동 기록', 120 - small.width('새 최고 단계에 닿으면 자동 기록') // 2, 132, GRAY, BLACK)
    font.draw(img, 'B 돌아가기', 6, 150, WHITE, BLACK)
    return img


def screen_gallery(font, small):
    img = background()
    centered(font, img, 'GALLERY', 14)
    ImageDraw.Draw(img).line((14, 26, 226, 26), fill=(120, 120, 120))
    cutdir = os.path.join(ROOT, 'assets', 'graphics', 'cutscene')
    thumbs = {(0, 0): 'scene7_forge_door.bmp', (1, 2): 'scene5_decree.bmp'}
    for r, mode in enumerate(('STORY', 'HARD')):
        y0 = 34 + r * 44
        font.draw(img, mode, 14, y0 + 1, GOLD if r == 0 else RED, BLACK)
        for c in range(3):
            x0 = 14 + c * 72
            box = (x0, y0 + 10, x0 + 66, y0 + 10 + 28)
            if (r, c) in thumbs:
                t = Image.open(os.path.join(cutdir, thumbs[(r, c)])).convert('RGB').resize((66, 28), Image.NEAREST)
                img.paste(t, (box[0], box[1]))
                ImageDraw.Draw(img).rectangle(box, outline=GOLD)
            else:
                ImageDraw.Draw(img).rectangle(box, fill=(24, 24, 28), outline=(90, 90, 96))
                small.draw(img, '?', (box[0] + box[2]) // 2 - 3, (box[1] + box[3]) // 2, (120, 120, 126), BLACK)
    # 맨 아래: 시간대 아이콘이 열 이름 (낮 / 노을 / 밤)
    for c, name in enumerate(('day', 'sunset', 'night')):
        icon = Image.open(os.path.join(ROOT, 'assets', 'graphics', 'forge', 'time_icon_%s.png' % name)).convert('RGBA')
        x0 = 14 + c * 72 + 33 - 9
        base = img.convert('RGBA')
        base.alpha_composite(icon, (x0, 124))
        img.paste(base.convert('RGB'))
    font.draw(img, '2 / 6   A 크게 보기  B 돌아가기', 6, 151, WHITE, BLACK)
    return img


def screen_language(font, small):
    img = background()
    centered(font, img, 'LANGUAGE', 36)
    centered(font, img, '한국어', 74, WHITE)
    font.draw(img, '▶', 120 - font.width('한국어') // 2 - 18, 74, GOLD, BLACK)
    centered(font, img, 'ENGLISH', 94, GRAY)
    centered(small, img, '(준비 중)', 112, GRAY)
    font.draw(img, 'A 선택  B 돌아가기', 6, 148, WHITE, BLACK)
    return img


def main():
    os.makedirs(OUT, exist_ok=True)
    font = GameFont('hs_font.bmp', 'hs_font.h', False)
    small = GameFont('hs_font_small.bmp', 'hs_font_small.h', True)
    screens = [('option', screen_option(font)), ('record', screen_record(font, small)), ('record_empty', screen_record(font, small, True)),
               ('gallery', screen_gallery(font, small)), ('language', screen_language(font, small))]
    sheet = Image.new('RGB', (3 * 490 - 10, 2 * 330 - 10), (30, 30, 34))
    for i, (name, img) in enumerate(screens):
        img.save(os.path.join(OUT, name + '.png'))
        sheet.paste(img.resize((480, 320), Image.NEAREST), ((i % 3) * 490, (i // 3) * 330))
    sheet.save(os.path.join(OUT, 'sheet.png'))
    print('ok')


if __name__ == '__main__':
    main()
