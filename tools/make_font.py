"""한글 스프라이트 폰트 생성기.

src/, include/ 소스에서 쓰인 비ASCII 글자를 모아 16x16 스프라이트 폰트를 만든다.
- ASCII(33~126): Butano common_variable_8x16_font 글리프를 그대로 복사
- 그 밖의 글자: 굴림 12px로 렌더링 (흰 본체 + 오른쪽/아래 검은 그림자)

출력: graphics/hs_font.bmp, graphics/hs_font.json, include/hs_font.h
같은 글자로 사방에 검은 외곽선을 두른 폰트도 함께 만든다 (일시정지 화면처럼 배경이 복잡한 곳용):
graphics/hs_font_outline.bmp, graphics/hs_font_outline.json, include/hs_font_outline.h (hs::font_outline)
내용이 바뀌었을 때만 파일을 다시 쓰므로 Makefile EXTTOOL로 매 빌드마다 돌려도 된다.
"""

import os
import re
import struct
import sys

from PIL import Image, ImageDraw, ImageFont

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
BUTANO_COMMON = os.path.join(ROOT, '..', 'butano', 'common')
ASCII_BMP = os.path.join(BUTANO_COMMON, 'graphics', 'common_variable_8x16_font.bmp')
ASCII_HEADER = os.path.join(BUTANO_COMMON, 'include', 'common_variable_8x16_sprite_font.h')
KOREAN_TTF = r'C:\Windows\Fonts\gulim.ttc'
KOREAN_SIZE = 12
KOREAN_WIDTH = 13      # 12px 글자 + 그림자 1px
KOREAN_TOP = 2

OUT_BMP = os.path.join(ROOT, 'graphics', 'hs_font.bmp')
OUT_JSON = os.path.join(ROOT, 'graphics', 'hs_font.json')
OUT_HEADER = os.path.join(ROOT, 'include', 'hs_font.h')

SMALL_SIZE = 10        # 작은 글꼴(hs::font_small): 10px 글자 + 사방 외곽선, 전부 굴림으로 그린다
SMALL_TOP = 3

CELL = 16
BODY = 14     # 흰색
SHADOW = 12   # 검은색


def collect_characters():
    chars = set()
    for folder in ('src', 'include'):
        for dirpath, _, filenames in os.walk(os.path.join(ROOT, folder)):
            for filename in filenames:
                if filename in ('hs_font.h', 'hs_font_outline.h', 'hs_font_small.h') or not filename.endswith(('.cpp', '.h', '.hpp')):
                    continue
                with open(os.path.join(dirpath, filename), encoding='utf-8') as f:
                    text = f.read()
                # 주석은 제외하고 문자열 리터럴 안의 글자만 모은다
                for literal in re.findall(r'"((?:[^"\\\n]|\\.)*)"', text):
                    chars.update(c for c in literal if ord(c) > 126)
    return sorted(chars)


def read_ascii_widths():
    with open(ASCII_HEADER, encoding='utf-8') as f:
        text = f.read()
    body = text.split('character_widths[] = {')[1].split('};')[0]
    widths = [int(m) for m in re.findall(r'^\s*(\d+),', body, re.MULTILINE)]
    return widths[:95]   # 32(공백)~126


def write_bmp_4bpp(path, image, palette):
    width, height = image.size
    row_size = ((width * 4 + 31) // 32) * 4
    pixels = bytearray()
    for y in range(height - 1, -1, -1):
        row = bytearray()
        for x in range(0, width, 2):
            hi = image.getpixel((x, y))
            lo = image.getpixel((x + 1, y)) if x + 1 < width else 0
            row.append((hi << 4) | lo)
        row.extend(b'\0' * (row_size - len(row)))
        pixels.extend(row)
    palette_bytes = bytearray()
    for i in range(16):
        r, g, b = palette[i]
        palette_bytes.extend((b, g, r, 0))
    offset = 14 + 40 + len(palette_bytes)
    header = struct.pack('<2sIHHI', b'BM', offset + len(pixels), 0, 0, offset)
    info = struct.pack('<IiiHHIIiiII', 40, width, height, 1, 4, 0, len(pixels), 2835, 2835, 16, 16)
    return bytes(header + info + palette_bytes + pixels)


def write_if_changed(path, data):
    mode = 'rb' if isinstance(data, bytes) else 'r'
    if os.path.exists(path):
        kwargs = {} if mode == 'rb' else {'encoding': 'utf-8', 'newline': ''}
        with open(path, mode, **kwargs) as f:
            if f.read() == data:
                return False
    if isinstance(data, bytes):
        with open(path, 'wb') as f:
            f.write(data)
    else:
        with open(path, 'w', encoding='utf-8', newline='\n') as f:
            f.write(data)
    return True


def draw_outline(image, base_y, mask):
    """mask(1비트, CELL x CELL)의 본체 둘레 8방향에 검은 외곽선을 칠한다 (본체는 호출한 쪽에서 칠함)"""
    for y in range(CELL):
        for x in range(CELL):
            if mask.getpixel((x, y)):
                for sy in (-1, 0, 1):
                    for sx in (-1, 0, 1):
                        px, py = x + sx, y + sy
                        if 0 <= px < CELL and 0 <= py < CELL and not mask.getpixel((px, py)):
                            image.putpixel((px, base_y + py), SHADOW)


def build_image(chars, ascii_image, outline):
    glyph_count = 94 + len(chars)
    image = Image.new('P', (CELL, CELL * glyph_count), 0)

    # ASCII 33~126
    for index in range(94):
        if not outline:
            for y in range(16):
                for x in range(8):
                    image.putpixel((x, index * CELL + y), ascii_image.getpixel((x, index * 16 + y)))
        else:
            # 본체(흰색) 점만 가져와 한 칸 오른쪽으로 옮기고 둘레를 검게 두른다
            mask = Image.new('1', (CELL, CELL), 0)
            for y in range(16):
                for x in range(8):
                    if ascii_image.getpixel((x, index * 16 + y)) == BODY:
                        mask.putpixel((x + 1, y), 1)
            base_y = index * CELL
            draw_outline(image, base_y, mask)
            for y in range(CELL):
                for x in range(CELL):
                    if mask.getpixel((x, y)):
                        image.putpixel((x, base_y + y), BODY)

    # 한글 등
    font = ImageFont.truetype(KOREAN_TTF, KOREAN_SIZE, index=0)
    for i, char in enumerate(chars):
        mask = Image.new('1', (CELL, CELL), 0)
        draw = ImageDraw.Draw(mask)
        draw.fontmode = '1'
        draw.text((1 if outline else 0, KOREAN_TOP), char, font=font, fill=1)
        base_y = (94 + i) * CELL

        if outline:
            draw_outline(image, base_y, mask)
        else:
            for y in range(CELL):
                for x in range(CELL):
                    if mask.getpixel((x, y)):
                        for sx, sy in ((1, 0), (0, 1), (1, 1)):
                            px, py = x + sx, y + sy
                            if px < CELL and py < CELL and not mask.getpixel((px, py)):
                                image.putpixel((px, base_y + py), SHADOW)

        for y in range(CELL):
            for x in range(CELL):
                if mask.getpixel((x, y)):
                    image.putpixel((x, base_y + y), BODY)

    return image, glyph_count


def build_small(chars):
    """작은 외곽선 폰트(hs::font_small): ASCII 33~126과 그 밖의 글자를 모두 굴림 10px로 그린다.
    반환: (이미지, 글자 수, 폭 목록 — 공백(32)부터 ASCII 126까지 95개 + chars)"""
    font = ImageFont.truetype(KOREAN_TTF, SMALL_SIZE, index=0)
    glyphs = [chr(code) for code in range(33, 127)] + list(chars)
    image = Image.new('P', (CELL, CELL * len(glyphs)), 0)

    for index, char in enumerate(glyphs):
        mask = Image.new('1', (CELL, CELL), 0)
        draw = ImageDraw.Draw(mask)
        draw.fontmode = '1'
        draw.text((1, SMALL_TOP), char, font=font, fill=1)
        base_y = index * CELL
        draw_outline(image, base_y, mask)
        for y in range(CELL):
            for x in range(CELL):
                if mask.getpixel((x, y)):
                    image.putpixel((x, base_y + y), BODY)

    widths = [4]     # 공백
    for char in glyphs[:94]:
        widths.append(int(round(font.getlength(char))) + 2)
    widths += [SMALL_SIZE + 2] * len(chars)
    return image, len(glyphs), widths


def header_text(chars, widths, name, suffix):
    """name = 스프라이트 아이템 이름(hs_font), suffix = 변수 이름 꼬리('' 또는 '_outline')"""
    guard = name.upper() + '_H'
    lines = [
        '// 자동 생성 파일: tools/make_font.py가 만든다. 직접 고치지 말 것.',
        '',
        '#ifndef %s' % guard,
        '#define %s' % guard,
        '',
        '#include "bn_sprite_font.h"',
        '#include "bn_utf8_characters_map.h"',
        '#include "bn_sprite_items_%s.h"' % name,
        '',
        'namespace hs',
        '{',
        '',
        'constexpr bn::utf8_character font_utf8_characters%s[] = {' % suffix,
    ]
    for start in range(0, len(chars), 16):
        lines.append('    ' + ' '.join('"%s",' % c for c in chars[start:start + 16]))
    if not chars:
        lines.append('    " ",')
    lines += [
        '};',
        '',
        'constexpr int8_t font_character_widths%s[] = {' % suffix,
    ]
    for start in range(0, len(widths), 24):
        lines.append('    ' + ' '.join('%d,' % w for w in widths[start:start + 24]))
    lines += [
        '};',
        '',
        'constexpr bn::span<const bn::utf8_character> font_utf8_characters_span%s(font_utf8_characters%s);' % (suffix, suffix),
        '',
        'constexpr auto font_utf8_characters_map%s = bn::utf8_characters_map<font_utf8_characters_span%s>();' % (suffix, suffix),
        '',
        'constexpr bn::sprite_font font%s(' % suffix,
        '        bn::sprite_items::%s, font_utf8_characters_map%s.reference(), font_character_widths%s);' % (name, suffix, suffix),
        '',
        '}',
        '',
        '#endif',
        '',
    ]
    return '\n'.join(lines)


def main():
    chars = collect_characters()
    ascii_widths = read_ascii_widths()

    ascii_image = Image.open(ASCII_BMP)
    raw_palette = ascii_image.getpalette()
    palette = [tuple(raw_palette[i * 3:i * 3 + 3]) for i in range(16)]

    changed = False

    for outline in (False, True):
        name = 'hs_font_outline' if outline else 'hs_font'
        suffix = '_outline' if outline else ''
        image, glyph_count = build_image(chars, ascii_image, outline)

        if outline:
            widths = [w + 2 for w in ascii_widths] + [KOREAN_WIDTH + 1] * len(chars)    # 외곽선 두 겹(좌우 1px)만큼 넓다
        else:
            widths = list(ascii_widths) + [KOREAN_WIDTH] * len(chars)

        out_bmp = os.path.join(ROOT, 'graphics', name + '.bmp')
        out_json = os.path.join(ROOT, 'graphics', name + '.json')
        out_header = os.path.join(ROOT, 'include', name + '.h')
        changed |= write_if_changed(out_bmp, write_bmp_4bpp(out_bmp, image, palette))
        changed |= write_if_changed(out_json, '{\n    "type": "sprite",\n    "height": 16\n}\n')
        changed |= write_if_changed(out_header, header_text(chars, widths, name, suffix))

        if changed:
            print('%s: %d glyphs (%d non-ASCII)' % (name, glyph_count, len(chars)))

    # 작은 외곽선 폰트
    image, glyph_count, widths = build_small(chars)
    out_bmp = os.path.join(ROOT, 'graphics', 'hs_font_small.bmp')
    out_json = os.path.join(ROOT, 'graphics', 'hs_font_small.json')
    out_header = os.path.join(ROOT, 'include', 'hs_font_small.h')
    small_changed = write_if_changed(out_bmp, write_bmp_4bpp(out_bmp, image, palette))
    small_changed |= write_if_changed(out_json, '{\n    "type": "sprite",\n    "height": 16\n}\n')
    small_changed |= write_if_changed(out_header, header_text(chars, widths, 'hs_font_small', '_small'))

    if small_changed:
        print('hs_font_small: %d glyphs (%d non-ASCII)' % (glyph_count, len(chars)))

    return 0


if __name__ == '__main__':
    sys.exit(main())
