"""고양이 이벤트 그림 변환기 (시안 단계: 미리보기만 만들고 게임에는 적용하지 않는다).

assets/graphics/events/cat_source.webp(사용자가 준 사진)에서 고양이 얼굴을 잘라 GBA 도트 그림 후보를 만든다.
15색 + 투명(0번). 색 줄이기는 MAXCOVERAGE + kmeans (MEDIANCUT은 개수가 적은 색을 합쳐 버린다).
후보: 얼굴만(가로 4:3) 세 크기, 손까지 포함한 정사각형 한 크기.

수동 실행: python -B tools/make_cat.py
출력: build/cat_preview/ (후보 시트, 강화 화면에 얹은 모의 화면)
"""

import os
import sys

from PIL import Image, ImageDraw, ImageEnhance

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import preview_sell_fx as base

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
SOURCE = os.path.join(ROOT, 'assets', 'graphics', 'events', 'cat_source.webp')
OUT = os.path.join(ROOT, 'build', 'cat_preview')

FACE_BOX = (70, 40, 1130, 835)      # 얼굴 + 턱 털까지 (손은 빼기)
SQUARE_BOX = (60, 40, 1160, 1140)   # 손까지 포함한 정사각형

FRAME_DARK = (40, 24, 24)
FRAME_LIGHT = (200, 168, 120)


def to_pixel(image, size, colors=15, contrast=1.15):
    image = ImageEnhance.Contrast(image).enhance(contrast)
    small = image.resize(size, Image.Resampling.LANCZOS)
    quantized = small.quantize(colors=colors, method=Image.Quantize.MAXCOVERAGE, kmeans=5, dither=Image.Dither.NONE)
    return quantized.convert('RGB')


def framed(picture):
    """그림 둘레에 어두운 테두리 + 밝은 안쪽 선을 두른다"""
    w, h = picture.size
    out = Image.new('RGB', (w + 6, h + 6), FRAME_DARK)
    draw = ImageDraw.Draw(out)
    draw.rectangle((1, 1, w + 4, h + 4), outline=FRAME_LIGHT)
    out.paste(picture, (3, 3))
    return out


def main():
    os.makedirs(OUT, exist_ok=True)
    source = Image.open(SOURCE).convert('RGB')
    face = source.crop(FACE_BOX)
    square = source.crop(SQUARE_BOX)

    variants = [
        ('A 얼굴 64x48', to_pixel(face, (64, 48))),
        ('B 얼굴 80x60', to_pixel(face, (80, 60))),
        ('C 얼굴 96x72', to_pixel(face, (96, 72))),
        ('D 손포함 72x72', to_pixel(square, (72, 72))),
    ]

    scale = 5
    sheet = Image.new('RGB', (4 * 100 * scale // 2 + 20, 90 * scale), (30, 30, 36))
    x = 10

    for title, picture in variants:
        big = picture.resize((picture.width * scale // 2, picture.height * scale // 2), Image.NEAREST)
        sheet.paste(big, (x, 20))
        ImageDraw.Draw(sheet).text((x, 4), title, fill=(255, 255, 255))
        x += big.width + 10

    sheet = sheet.crop((0, 0, x, 90 * scale))
    sheet.save(os.path.join(OUT, 'candidates.png'))

    # 모의 화면: 강화 화면 위에 B안을 얹고 메시지를 아래에 둔다
    mock = base.base_scene().convert('RGB')
    picture = framed(variants[1][1])
    mock.paste(picture, ((240 - picture.width) // 2, 14))
    draw = ImageDraw.Draw(mock)
    draw.text((92, 108), 'cat!', fill=(255, 255, 255))
    mock.resize((240 * 4, 160 * 4), Image.NEAREST).save(os.path.join(OUT, 'mock_B.png'))

    for title, picture in variants:
        name = title[0]
        picture.save(os.path.join(OUT, 'cat_%s.png' % name))

    print('출력:', OUT)
    return 0


if __name__ == '__main__':
    sys.exit(main())
