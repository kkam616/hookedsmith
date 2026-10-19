"""컷씬 대사가 대사 박스 두 줄(첫 줄 228px, 둘째 줄 212px)에 들어가는지 미리 검사한다 (src/main.cpp의 *_lines 표를 읽는다).
폭은 hs::font와 같은 규칙: 한글 13px, ASCII는 make_font.read_ascii_widths(). wrap은 main.cpp의 wrap_dialogue와 같다.
사용: python -B tools/check_dialogue_width.py
"""
import os
import re
import sys

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
from make_font import read_ascii_widths, KOREAN_WIDTH

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
TEXTBOX = 228
SECOND = TEXTBOX - 16

ascii_widths = read_ascii_widths()


def width(text):
    total = 0
    for ch in text:
        o = ord(ch)
        total += ascii_widths[o - 32] if 32 <= o < 127 else KOREAN_WIDTH
    return total


def wrap(text):
    first, second = '', ''
    line = 'first'
    rest = text.split(' ')
    for word in rest:
        cur = first if line == 'first' else second
        cand = (cur + ' ' + word) if cur else word
        limit = TEXTBOX if line == 'first' else SECOND
        if not cur or width(cand) <= limit:
            if line == 'first':
                first = cand
            else:
                second = cand
        else:
            if line == 'second':
                return first, second, False
            line = 'second'
            second = word
    ok = width(first) <= TEXTBOX and width(second) <= SECOND
    return first, second, ok


src = open(os.path.join(ROOT, 'src', 'main.cpp'), encoding='utf-8').read()
bad = 0
for m in re.finditer(r'\{\s*"([^"]+)",\s*"([^"]+)"\s*\}', src):
    speaker, text = m.groups()
    if not re.search(r'[가-힣]', text) or len(text) < 3:
        continue
    first, second, ok = wrap(text)
    if not ok or width(second) > SECOND:
        bad += 1
        print('TOO LONG  [%s] %s  (%d + %d px)' % (speaker, text, width(first), width(second)))
    else:
        print('ok  %3d/%3d  [%s] %s' % (width(first), width(second), speaker, text))
print('문제 %d개' % bad)
