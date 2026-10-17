"""도둑 정면 사진 변환기 (시안 단계: 미리보기만 만들고 게임에는 적용하지 않는다).

assets/graphics/events/thief_source.webp(사용자가 준 도둑 일러스트)를 고양이 사진(make_cat.py)과 같은 방식으로
GBA 도트 그림 후보로 줄이고(15색), 밤 장면의 사진 창 안에 넣은 모의 화면을 만든다.
후보: 전체 / 얼굴 중심으로 조금 더 자른 것, 크기 64x51(작게) / 80x64(창 안쪽 가득).

수동 실행: python -B tools/make_thief_photo.py
출력: build/thief_photo_preview/ (후보 시트, 밤 장면 모의 화면)
"""

import os
import sys

from PIL import Image, ImageDraw, ImageFont

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import make_cat as photo
import make_thief_event_mock as mock
import preview_sell_fx as base

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
SOURCE = os.path.join(ROOT, 'assets', 'graphics', 'events', 'thief_source.webp')
OUT = os.path.join(ROOT, 'build', 'thief_photo_preview')

FULL_BOX = (0, 0, 1024, 825)
FACE_BOX = (60, 40, 1000, 792)        # 가장자리를 조금 덜어내고 얼굴·검·망치를 크게 (가로:세로 5:4)


def main():
    os.makedirs(OUT, exist_ok=True)
    source = Image.open(SOURCE).convert('RGB')
    full = source.crop(FULL_BOX)
    face = source.crop(FACE_BOX)

    variants = [
        ('A 전체 64x51', photo.to_pixel(full, (64, 51), 15, 1.1)),
        ('B 전체 80x64', photo.to_pixel(full, (80, 64), 15, 1.1)),
        ('C 얼굴 80x64', photo.to_pixel(face, (80, 64), 15, 1.1)),
        ('D 얼굴 96x77', photo.to_pixel(face, (96, 77), 15, 1.1)),
    ]

    scale = 5
    sheet = Image.new('RGB', (4 * 100 * scale // 2 + 20, 90 * scale), (30, 30, 36))
    x = 10
    for title, picture in variants:
        big = picture.resize((picture.width * scale // 2, picture.height * scale // 2), Image.NEAREST)
        sheet.paste(big, (x, 20))
        ImageDraw.Draw(sheet).text((x, 4), title, fill=(255, 255, 255))
        x += big.width + 10
    sheet.crop((0, 0, x, 90 * scale)).save(os.path.join(OUT, 'candidates.png'))

    # 밤 장면: 창 안(80x64)에 B·C안을 넣고 아래에 도난 메시지
    font = ImageFont.truetype(r'C:\Windows\Fonts\gulim.ttc', 12, index=0)
    scene = mock.night(base.base_scene().convert('RGB'), 0.45).convert('RGBA')
    panels = []
    for title, picture in (variants[1], variants[2]):
        image = scene.copy()
        window = photo.framed(picture).convert('RGBA')
        wx, wy = 77, 41
        image.alpha_composite(window, (wx, wy))
        draw = ImageDraw.Draw(image)
        mock.text_with_outline(draw, '+21 템페스트 골드를', 120, wy + window.height + 8, font)
        mock.text_with_outline(draw, '도둑맞았다...', 120, wy + window.height + 22, font)
        panels.append(image.convert('RGB').resize((240 * 3, 160 * 3), Image.NEAREST))
    scene_sheet = Image.new('RGB', (240 * 3 * 2 + 10, 160 * 3), (24, 24, 30))
    scene_sheet.paste(panels[0], (0, 0))
    scene_sheet.paste(panels[1], (240 * 3 + 10, 0))
    scene_sheet.save(os.path.join(OUT, 'night_scene.png'))

    for title, picture in variants:
        picture.save(os.path.join(OUT, 'thief_%s.png' % title[0]))
    print('출력:', OUT)
    return 0


if __name__ == '__main__':
    sys.exit(main())
