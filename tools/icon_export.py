"""아이콘 변환 스크립트들이 같이 쓰는 내보내기 도우미.

16x16 RGBA 아이콘(투명 + 15색 이하)을 Butano가 읽는 graphics/ico_NAME.bmp(4bpp, 0번 투명)와 .json으로 쓴다.
"""

import os
from collections import Counter

from PIL import Image

from make_font import write_bmp_4bpp, write_if_changed

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
GRAPHICS_DIR = os.path.join(ROOT, 'graphics')


def export_icon(name, canvas):
    """name 예: 'ico_ticket'. canvas는 16x16 RGBA. 불투명한 색이 15가지를 넘으면 오류."""
    assert canvas.size == (16, 16), name

    counts = Counter()
    for y in range(16):
        for x in range(16):
            r, g, b, a = canvas.getpixel((x, y))
            if a >= 128:
                counts[(r, g, b)] += 1

    colors = [color for color, _ in counts.most_common()]
    assert len(colors) <= 15, '%s: 색이 %d가지 (15가지까지)' % (name, len(colors))
    index_of = {color: i + 1 for i, color in enumerate(colors)}

    image = Image.new('P', (16, 16), 0)
    for y in range(16):
        for x in range(16):
            r, g, b, a = canvas.getpixel((x, y))
            if a >= 128:
                image.putpixel((x, y), index_of[(r, g, b)])

    palette = [(0, 0, 0)] + colors + [(0, 0, 0)] * (15 - len(colors))
    write_if_changed(os.path.join(GRAPHICS_DIR, name + '.bmp'), write_bmp_4bpp(None, image, palette))
    write_if_changed(os.path.join(GRAPHICS_DIR, name + '.json'), '{\n    "type": "sprite",\n    "height": 16\n}\n')
