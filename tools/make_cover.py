"""GBA 박스 앞면 합성 (build/cover/hookedsmith_cover.png, 600x600).

- 왼쪽 띠: 사용자가 준 공식 표지 이미지(assets/graphics/cover/gba_box_reference.webp, 600x600)의 왼쪽 띠(약 80px)를 그대로 오려 쓴다.
  띠 바깥(퍼블리셔 로고·등급·인증 마크·일러스트)은 쓰지 않는다.
- 나머지: tools/preview_cover2.py가 만든 표지 본문(배경 + 로고 + JAYS LOFT + 문구, 띠 제외)을 520x600으로 줄여 붙인다.
  배경이 임시 그림이면 최종 일러스트(assets/graphics/cover/cover_art.png)로 바꾼 뒤 다시 돌린다.
사용: python -B tools/preview_cover2.py && python -B tools/make_cover.py
"""
import os

from PIL import Image, ImageDraw, ImageFilter

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
REF = os.path.join(ROOT, 'assets', 'graphics', 'cover', 'gba_box_reference.webp')
BODY = os.path.join(ROOT, 'build', 'cover', 'cover2.png')
OUT = os.path.join(ROOT, 'build', 'cover', 'hookedsmith_cover.png')
SPINE_W = 76          # 공식 이미지에서 오려 낼 띠 폭 (600px 중, 76열부터는 흰 배경이라 포함하지 않는다)
SIZE = 600


def main():
    ref = Image.open(REF).convert('RGB')
    spine = ref.crop((0, 0, SPINE_W, ref.height))

    body = Image.open(BODY).convert('RGB')
    body = body.crop((150, 0, body.width, body.height))                  # 시안 2의 띠(150px)를 뺀 본문
    body = body.resize((SIZE - SPINE_W, SIZE), Image.LANCZOS)

    cover = Image.new('RGB', (SIZE, SIZE))
    cover.paste(body, (SPINE_W, 0))
    # 띠가 본문 위에 살짝 그림자를 드리운 것처럼 (공식 표지도 경계가 부드럽다)
    shadow = Image.new('RGBA', (10, SIZE), (0, 0, 0, 0))
    sd = ImageDraw.Draw(shadow)
    for x in range(10):
        sd.line((x, 0, x, SIZE), fill=(0, 0, 0, round(90 * (1 - x / 10) ** 2)))
    cover.paste(shadow, (SPINE_W, 0), shadow)
    cover.paste(spine, (0, 0))
    cover.save(OUT)
    cover.resize((SIZE * 2, SIZE * 2), Image.NEAREST).save(OUT.replace('.png', '_x2.png'))
    print('ok', OUT)


if __name__ == '__main__':
    main()
