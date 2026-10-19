"""OPTION > Gallery 그림 (수동 실행): 엔딩 썸네일 6장 + 잠긴 칸 + 시간대 아이콘 3개 -> graphics/gallery_*.bmp (스프라이트 4bpp)

- gallery_thumb_0~5: 엔딩 그림 (번호 = 모드*3 + 시간대, 모드 0 STORY / 1 HARD, 시간대 0 낮 / 1 노을 / 2 밤). 지금은 임시로 오프닝 장면을 쓴다.
  엔딩 그림이 생기면 THUMBS의 원본 파일만 바꿔 다시 실행한다.
- gallery_locked: 아직 못 본 엔딩 칸 (어두운 판 + ?)
- gallery_icon_0~2: 맨 아래 열 이름으로 쓰는 시간대 아이콘 (낮 / 노을 / 밤)
사용: python -B tools/make_gallery_assets.py
"""
import os
import sys

from PIL import Image, ImageDraw

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
from make_font import write_bmp_4bpp, write_if_changed

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
CUT = os.path.join(ROOT, 'assets', 'graphics', 'cutscene')
ICON = os.path.join(ROOT, 'assets', 'graphics', 'forge')
OUT = os.path.join(ROOT, 'graphics')

# (STORY 낮, 노을, 밤), (HARD 낮, 노을, 밤): 임시 그림
THUMBS = ['scene7_forge_door.bmp', 'scene4_tax_collector.bmp', 'scene5_decree.bmp',
          'scene6_dagger.bmp', 'scene2_anvil.bmp', 'scene_bankrupt.bmp']
JSON32 = '{\n    "type": "sprite",\n    "height": 32\n}\n'

QUESTION = [
    '..XXXX..',
    '.XX..XX.',
    '.....XX.',
    '....XX..',
    '...XX...',
    '........',
    '...XX...',
]


def quantize15(image):
    """RGB -> (색 번호 이미지(0번 비움), 팔레트 16색)"""
    q = image.quantize(colors=15, method=Image.Quantize.MAXCOVERAGE, kmeans=3, dither=Image.Dither.NONE)
    raw = q.getpalette()
    palette = [(0, 0, 0)] + [tuple(c & 0xF8 for c in raw[i * 3:i * 3 + 3]) for i in range(15)]
    return q, palette


def write(name, indices, palette):
    write_if_changed(os.path.join(OUT, name + '.bmp'), write_bmp_4bpp(None, indices, palette))
    write_if_changed(os.path.join(OUT, name + '.json'), JSON32.encode())


def main():
    for index, source in enumerate(THUMBS):
        image = Image.open(os.path.join(CUT, source)).convert('RGB').resize((64, 30), Image.LANCZOS)
        q, palette = quantize15(image)
        indices = Image.new('P', (64, 32), 0)
        indices.putdata([0] * (64 * 32))
        px = indices.load()
        for y in range(30):
            for x in range(64):
                px[x, y + 1] = q.getpixel((x, y)) + 1
        write('gallery_thumb_%d' % index, indices, palette)

    # 잠긴 칸
    palette = [(0, 0, 0), (24, 24, 28), (88, 88, 96), (128, 128, 136)] + [(0, 0, 0)] * 12
    indices = Image.new('P', (64, 32), 0)
    d = ImageDraw.Draw(indices)
    d.rectangle((0, 1, 63, 30), fill=1, outline=2)
    px = indices.load()
    ox, oy = 64 // 2 - 8, 32 // 2 - 7                    # 가운데에 2배 크기 ?
    for y, row in enumerate(QUESTION):
        for x, ch in enumerate(row):
            if ch == 'X':
                for dy in range(2):
                    for dx in range(2):
                        px[ox + x * 2 + dx, oy + y * 2 + dy] = 3
    write('gallery_locked', indices, palette)

    # 시간대 아이콘 (18x18 -> 32x32 가운데)
    for index, name in enumerate(('day', 'sunset', 'night')):
        icon = Image.open(os.path.join(ICON, 'time_icon_%s.png' % name)).convert('RGBA')
        rgb = Image.new('RGB', icon.size, (0, 0, 0))
        rgb.paste(icon, (0, 0), icon)
        q, palette = quantize15(rgb)
        indices = Image.new('P', (32, 32), 0)
        px = indices.load()
        for y in range(icon.size[1]):
            for x in range(icon.size[0]):
                if icon.getpixel((x, y))[3] >= 128:
                    px[x + 7, y + 7] = q.getpixel((x, y)) + 1
        write('gallery_icon_%d' % index, indices, palette)
    print('ok')


if __name__ == '__main__':
    main()
