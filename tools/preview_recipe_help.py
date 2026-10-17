"""SELECT 도움말 화면에 레시피 아이콘을 합성한 시안 (게임에는 아직 적용하지 않는다).

build/help_base.png(도움말 화면 실제 스크린샷)에 새 아이콘(tools/make_recipe_icon.py 변환 결과)을 잡화(↓)와 판매(B) 사이에 놓고
"레시피" 라벨과 START 키 표시(↓·B와 같은 줄, 아이콘 가운데)를 그린다.
수동 실행: python -B tools/preview_recipe_help.py
"""

import os
import re
import sys

from PIL import Image, ImageDraw, ImageFont

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import make_icons

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
ICON_CENTER = (120, 128)        # 화면 기준 (잡화 (64,128)와 판매 (176,128)의 한가운데)


def main():
    base = Image.open(os.path.join(ROOT, 'build', 'help_base.png')).convert('RGBA')
    sheet = Image.open(os.path.join(ROOT, 'assets', 'graphics', 'icons', 'recipe_icon_source.webp')).convert('RGB')
    image, palette = make_icons.convert(sheet, (395, 86, 631, 320), 'circle', 40)

    icon = Image.new('RGBA', image.size, (0, 0, 0, 0))
    for y in range(image.height):
        for x in range(image.width):
            index = image.getpixel((x, y))
            if index:
                icon.putpixel((x, y), palette[index] + (255,))
    icon = icon.crop((12, 12, 52, 52))
    base.alpha_composite(icon, (ICON_CENTER[0] - 20, ICON_CENTER[1] - 20))

    # 게임과 같은 글꼴로 그린다: 영문·숫자는 graphics/hs_font.bmp(Butano 8x16 글리프), 한글은 make_font.py와 같은 방식(굴림 12px + 오른쪽·아래 그림자)
    font_bmp = Image.open(os.path.join(ROOT, 'graphics', 'hs_font.bmp'))
    font_raw = font_bmp.getpalette()
    font_palette = [tuple(font_raw[i * 3:i * 3 + 3]) for i in range(16)]
    header = open(os.path.join(ROOT, 'include', 'hs_font.h'), encoding='utf-8').read()
    widths = [int(x) for x in re.findall(r'(\d+),', header.split('font_character_widths[] = {')[1].split('};')[0])]
    gulim = ImageFont.truetype(r'C:\Windows\Fonts\gulim.ttc', 12, index=0)

    def glyph(char):
        """(16x16 RGBA 글리프, 진행 폭)"""
        cell = Image.new('RGBA', (16, 16), (0, 0, 0, 0))
        if ord(char) < 127:
            index = ord(char) - 33
            for y in range(16):
                for x in range(16):
                    value = font_bmp.getpixel((x, index * 16 + y))
                    if value:
                        cell.putpixel((x, y), font_palette[value] + (255,))
            return cell, widths[1 + index]
        mask = Image.new('1', (16, 16), 0)
        d = ImageDraw.Draw(mask)
        d.fontmode = '1'
        d.text((0, 2), char, font=gulim, fill=1)
        for y in range(16):
            for x in range(16):
                if mask.getpixel((x, y)):
                    for sx, sy in ((1, 0), (0, 1), (1, 1)):
                        px, py = x + sx, y + sy
                        if px < 16 and py < 16 and not mask.getpixel((px, py)) and cell.getpixel((px, py))[3] == 0:
                            cell.putpixel((px, py), (0, 0, 0, 255))
        for y in range(16):
            for x in range(16):
                if mask.getpixel((x, y)):
                    cell.putpixel((x, y), (255, 255, 255, 255))
        return cell, 13

    def game_text(text, cx, cy):
        glyphs = [glyph(c) for c in text]
        total = sum(w for _, w in glyphs)
        x = cx - total / 2
        for cell, advance in glyphs:
            base.alpha_composite(cell, (int(round(x)), cy - 8))
            x += advance

    game_text('레시피', ICON_CENTER[0], ICON_CENTER[1])
    game_text('START', ICON_CENTER[0], 113)            # 키 글자 줄: 잡화 ↓, 판매 B와 같은 높이(y 113), 아이콘 가운데

    base.convert('RGB').resize((960, 640), Image.NEAREST).save(os.path.join(ROOT, 'build', 'recipe_help_mock.png'))
    print('출력: build/recipe_help_mock.png')
    return 0


if __name__ == '__main__':
    sys.exit(main())
