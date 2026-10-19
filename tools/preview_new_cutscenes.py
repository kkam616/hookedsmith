"""새 오프닝 컷씬 그림(Gemini 원본 PNG) → 240x112, 16색 시안. build/cut_preview/ 에 저장한다 (게임에는 아직 적용하지 않음).

원본은 가로로 긴 PNG라 가운데 기준으로 비율(240:112)에 맞춰 자르고, 오른쪽 아래 Gemini 반짝이 마크를 주변 그림으로 덮은 뒤
LANCZOS로 줄이고 MAXCOVERAGE + kmeans로 16색으로 줄인다.
"""

import os
import sys

from PIL import Image

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
SRC = os.path.join(ROOT, 'assets', 'graphics', 'cutscene')
OUT = os.path.join(ROOT, 'build', 'cut_preview')

# (이름, 원본 파일 끝부분)
SCENES = [
    ('s1_bedside', 'vft6k9vft6k9vft6'),
    ('s2_anvil', '5fbhbv5fbhbv5fbh'),
    ('s3_father_close', 'vat81fvat81fvat8'),
    ('s4_tax_collector', 'xcdawjxcdawjxcda'),
    ('s5_decree', '7mvq2t7mvq2t7mvq'),
    ('s6_dagger', '7xoktk7xoktk7xok'),
    ('s7_forge_door', 'mbmxafmbmxafmbmx'),
    ('s8_bankrupt', 'oqxrecoqxrecoqxr'),
]


def load(tail):
    return Image.open(os.path.join(SRC, 'Gemini_Generated_Image_%s.png' % tail)).convert('RGB')


def crop_to_ratio(image):
    w, h = image.size
    want = 240 / 112
    if w / h > want:
        nw = round(h * want)
        left = (w - nw) // 2
        return image.crop((left, 0, left + nw, h))
    nh = round(w / want)
    top = (h - nh) // 2
    return image.crop((0, top, w, top + nh))


def remove_sparkle(image):
    """오른쪽 아래 반짝이: 그 자리를 왼쪽 같은 높이의 그림으로 덮는다 (자른 뒤 기준 좌표)."""
    w, h = image.size
    box = (int(w * 0.915), int(h * 0.76), int(w * 0.985), int(h * 0.9))
    patch = image.crop((box[0] - 120, box[1], box[2] - 120, box[3]))
    image.paste(patch, (box[0], box[1]))
    return image


def to_gba(image):
    small = image.resize((240, 112), Image.LANCZOS)
    return small.quantize(colors=16, method=Image.Quantize.MAXCOVERAGE, kmeans=3, dither=Image.Dither.NONE).convert('RGB')


def main():
    os.makedirs(OUT, exist_ok=True)
    sheet = Image.new('RGB', (240 * 2 * 2 + 10, 112 * 2 * 4 + 30), (30, 30, 30))
    for index, (name, tail) in enumerate(SCENES):
        image = remove_sparkle(crop_to_ratio(load(tail)))
        gba = to_gba(image)
        gba.save(os.path.join(OUT, name + '.png'))
        big = gba.resize((480, 224), Image.NEAREST)
        big.save(os.path.join(OUT, name + '_x2.png'))
        sheet.paste(big, ((index % 2) * 490, (index // 2) * 234))
    sheet.save(os.path.join(OUT, 'sheet.png'))
    print('ok', OUT)


if __name__ == '__main__':
    main()
