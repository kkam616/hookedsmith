"""레시피 버튼 아이콘 변환기.

assets/graphics/icons/recipe_icon_source.webp(원형 아이콘 6개 시트)의 둘째 아이콘(종이 위에 검이 그려진 것)을
다른 버튼 아이콘과 같은 방식(make_icons.py)으로 40px 원형 4bpp 스프라이트로 만든다.
수동 실행: python -B tools/make_recipe_icon.py            (build/recipe_icon_preview.png 미리보기만)
          python -B tools/make_recipe_icon.py --write    (graphics/icon_recipe.bmp·json 작성)
"""

import os
import sys

from PIL import Image

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import make_icons
from make_font import write_bmp_4bpp, write_if_changed

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
SOURCE = os.path.join(ROOT, 'assets', 'graphics', 'icons', 'recipe_icon_source.webp')
BOX = (395, 86, 631, 320)         # 둘째 원형 아이콘의 안쪽 그림 (링에서 6px 안쪽)
SIZE = 40


def main():
    sheet = Image.open(SOURCE).convert('RGB')
    image, palette = make_icons.convert(sheet, BOX, 'circle', SIZE)

    # 미리보기: 새 아이콘과 이미 있는 아이콘 둘을 나란히 (4배)
    def to_rgb(img, pal):
        out = Image.new('RGB', img.size, (60, 60, 70))
        for y in range(img.height):
            for x in range(img.width):
                index = img.getpixel((x, y))
                if index:
                    out.putpixel((x, y), pal[index])
        return out

    preview = Image.new('RGB', (64 * 4 * 3 + 20, 64 * 4), (60, 60, 70))
    preview.paste(to_rgb(image, palette).resize((256, 256), Image.NEAREST), (0, 0))
    for i, name in enumerate(('icon_bag', 'icon_sell')):
        existing = Image.open(os.path.join(ROOT, 'graphics', name + '.bmp'))
        raw = existing.getpalette()
        pal = [tuple(raw[j * 3:j * 3 + 3]) for j in range(16)]
        preview.paste(to_rgb(existing, pal).resize((256, 256), Image.NEAREST), ((i + 1) * 266, 0))
    os.makedirs(os.path.join(ROOT, 'build'), exist_ok=True)
    preview.save(os.path.join(ROOT, 'build', 'recipe_icon_preview.png'))

    if '--write' in sys.argv:
        write_if_changed(os.path.join(ROOT, 'graphics', 'icon_recipe.bmp'), write_bmp_4bpp(None, image, palette))
        write_if_changed(os.path.join(ROOT, 'graphics', 'icon_recipe.json'), '{\n    "type": "sprite",\n    "height": 64\n}\n')
        print('graphics/icon_recipe.bmp 작성')
    return 0


if __name__ == '__main__':
    sys.exit(main())
