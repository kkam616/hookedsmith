"""레시피 화면의 낡은 종이 배경 (graphics/recipe_paper.bmp, 8bpp 64색).

고정 글자(제목 "레시피", 머리글 "단계 / 필요한 것 / 성공 / 판매가", 아래 안내)는 스프라이트 타일 VRAM을 아끼려고 게임 글꼴 모양 그대로
배경 그림에 미리 그려 넣는다 (바뀌는 글자 — 모드 이름, 줄 내용, 방지권 아이콘, ▲▼ — 는 게임에서 스프라이트로 얹는다).

종이 질감, 얼룩, 가장자리 그을림, 접힌 자국은 tools/preview_recipe_note.py의 paper_background()와 같고,
제목 밑줄(y 21)을 더해 그린다. 글자는 게임에서 스프라이트로 얹는다.
수동 실행: python -B tools/make_recipe_paper.py
"""

import os
import re
import sys

from PIL import Image, ImageDraw, ImageFont

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import preview_recipe_note as note
from make_font import write_if_changed

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
COLORS = 64


INK = (56, 32, 16)            # main.cpp의 ink_palette (7,4,2)*8
INK_SHADOW = (152, 112, 72)   # (19,14,9)*8
FAINT = (144, 104, 64)        # faint_palette 본체 (18,13,8)*8
FAINT_SHADOW = (192, 152, 104)    # (24,19,13)*8


class GameFont:
    """게임 글꼴(hs::font / hs::font_small)과 같은 모양으로 글자를 그린다: 영문·숫자는 graphics/*.bmp 글리프, 한글은 굴림 + make_font.py와 같은 방식"""

    def __init__(self, bmp_name, header_name, small):
        self.small = small
        self.bmp = Image.open(os.path.join(ROOT, 'graphics', bmp_name))
        raw = self.bmp.getpalette()
        self.palette = [tuple(raw[i * 3:i * 3 + 3]) for i in range(16)]
        text = open(os.path.join(ROOT, 'include', header_name), encoding='utf-8').read()
        self.widths = [int(x) for x in re.findall(r'(\d+),', text.split('font_character_widths')[1].split('};')[0].split('{')[1])]
        self.gulim = ImageFont.truetype(r'C:\Windows\Fonts\gulim.ttc', 10 if small else 12, index=0)

    def glyph(self, char, body, shadow):
        cell = Image.new('RGBA', (16, 16), (0, 0, 0, 0))
        if char == ' ':
            return cell, self.widths[0]          # 공백은 그림 없이 폭만 (hs 글꼴의 첫 번째 폭)
        if ord(char) < 127:
            index = ord(char) - 33
            for y in range(16):
                for x in range(16):
                    value = self.bmp.getpixel((x, index * 16 + y))
                    if value == 14:
                        cell.putpixel((x, y), body + (255,))
                    elif value:
                        cell.putpixel((x, y), shadow + (255,))
            return cell, self.widths[1 + index]
        mask = Image.new('1', (16, 16), 0)
        draw = ImageDraw.Draw(mask)
        draw.fontmode = '1'
        draw.text((1 if self.small else 0, 3 if self.small else 2), char, font=self.gulim, fill=1)
        for y in range(16):
            for x in range(16):
                if mask.getpixel((x, y)):
                    offsets = [(sx, sy) for sx in (-1, 0, 1) for sy in (-1, 0, 1)] if self.small else [(1, 0), (0, 1), (1, 1)]
                    for sx, sy in offsets:
                        px, py = x + sx, y + sy
                        if 0 <= px < 16 and 0 <= py < 16 and not mask.getpixel((px, py)):
                            cell.putpixel((px, py), shadow + (255,))
        for y in range(16):
            for x in range(16):
                if mask.getpixel((x, y)):
                    cell.putpixel((x, y), body + (255,))
        return cell, (12 if self.small else 13)

    def width(self, text):
        return sum(self.glyph(c, INK, INK_SHADOW)[1] for c in text)

    def draw(self, image, text, x, center_y, body, shadow, align='left'):
        """x는 butano 좌표가 아니라 화면 좌표(왼쪽 위 기준), center_y는 글자 칸의 가운데 높이"""
        glyphs = [self.glyph(c, body, shadow) for c in text]
        total = sum(w for _, w in glyphs)
        cursor = x - total if align == 'right' else x
        base = image.convert('RGBA')
        for cell, advance in glyphs:
            base.alpha_composite(cell, (int(cursor), int(center_y) - 8))
            cursor += advance
        image.paste(base.convert('RGB'))


def main():
    image = note.paper_background()
    ImageDraw.Draw(image).line((12, 21, 230, 21), fill=note.INK_FAINT)       # 제목 밑줄

    # 고정 글자 (좌표는 main.cpp의 run_recipe와 맞춘다: 화면 x = butano x + 120, 화면 y = butano y + 80)
    normal = GameFont('hs_font.bmp', 'hs_font.h', False)
    small = GameFont('hs_font_small.bmp', 'hs_font_small.h', True)
    normal.draw(image, '레시피', 12, 13, INK, INK_SHADOW)
    small.draw(image, '단계', 12, 29, FAINT, FAINT_SHADOW)
    small.draw(image, '필요한 것', 58, 29, FAINT, FAINT_SHADOW)
    small.draw(image, '성공', 184, 29, FAINT, FAINT_SHADOW, 'right')
    small.draw(image, '판매가', 230, 29, FAINT, FAINT_SHADOW, 'right')
    small.draw(image, '↑↓ 넘기기     B 닫기', 12, 149, FAINT, FAINT_SHADOW)

    quantized = image.quantize(colors=COLORS, method=Image.Quantize.MAXCOVERAGE, kmeans=3, dither=Image.Dither.NONE)
    raw = quantized.getpalette()
    palette = [tuple(c & 0xF8 for c in raw[i * 3:i * 3 + 3]) for i in range(COLORS)]

    canvas = Image.new('P', (256, 256), 0)
    pixels = quantized.load()
    for y in range(note.H):
        for x in range(note.W):
            canvas.putpixel((x, y), pixels[x, y] + 1)
    canvas.putpalette([0, 0, 0] + [channel for color in palette for channel in color])

    path = os.path.join(ROOT, 'graphics', 'recipe_paper.bmp')
    canvas.save(path + '.tmp', format='BMP')
    with open(path + '.tmp', 'rb') as handle:
        write_if_changed(path, handle.read())
    os.remove(path + '.tmp')
    write_if_changed(os.path.join(ROOT, 'graphics', 'recipe_paper.json'), '{\n    "type": "regular_bg",\n    "bpp_mode": "bpp_8"\n}\n')

    canvas.convert('RGB').crop((0, 0, note.W, note.H)).resize((note.W * 3, note.H * 3), Image.NEAREST).save(os.path.join(ROOT, 'build', 'recipe_paper_preview.png'))
    print('graphics/recipe_paper.bmp 작성')
    return 0


if __name__ == '__main__':
    sys.exit(main())
