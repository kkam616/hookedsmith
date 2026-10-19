"""타이틀 배경 3장(낮=아침·노을·밤)을 만든다: AI 원본(assets/graphics/title/bg_*_src.png) + 로고(logo_source.png) → graphics/title_*.bmp (8bpp, 256x256 캔버스 왼쪽 위 240x160).

preview_new_title.py의 자르기·마크 지우기·로고 오리기를 그대로 쓰고, 메뉴(아이콘·글자)는 게임에서 스프라이트로 얹으니 여기서는 넣지 않는다.
팔레트 0번은 투명(=backdrop)이라 쓰지 않는다: 255색으로 줄여 1번부터 채운다.
수동 실행: python -B tools/make_title_bg.py            (build/title_preview/에 미리보기만)
          python -B tools/make_title_bg.py --write    (graphics/title_day·sunset·night.bmp + .json 작성)
"""
import os
import sys

from PIL import Image

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import preview_new_title as pv
from make_font import write_if_changed

ROOT = pv.ROOT
OUT = pv.OUT
NAMES = [('title_day', 'bg_morning_src.png'), ('title_sunset', 'bg_sunset_src.png'), ('title_night', 'bg_night_src.png')]
JSON = '{\n    "type": "regular_bg",\n    "bpp_mode": "bpp_8"\n}\n'


def compose(src_name, logo):
    bg = pv.prepare_bg(os.path.join(pv.SRC, src_name)).convert('RGBA')
    bg.alpha_composite(logo, pv.LOGO_POS)
    return bg.convert('RGB')


def to_indexed(image):
    q = image.quantize(colors=255, method=Image.Quantize.MAXCOVERAGE, kmeans=3, dither=Image.Dither.NONE)
    raw = q.getpalette()
    palette = [(0, 0, 0)] + [tuple(c & 0xF8 for c in raw[i * 3:i * 3 + 3]) for i in range(255)]
    indices = Image.new('P', (240, 160), 0)
    indices.putdata([v + 1 for v in q.getdata()])
    return indices, palette


def main():
    write = '--write' in sys.argv
    os.makedirs(OUT, exist_ok=True)
    logo = pv.key_out_black(Image.open(os.path.join(pv.SRC, 'logo_source.png')))
    logo = logo.crop(logo.getbbox())
    logo = logo.resize((pv.LOGO_WIDTH, round(logo.size[1] * pv.LOGO_WIDTH / logo.size[0])), Image.LANCZOS)

    for name, src in NAMES:
        indices, palette = to_indexed(compose(src, logo))
        preview = Image.new('RGB', (240, 160))
        preview.putdata([palette[v] for v in indices.getdata()])
        preview.save(os.path.join(OUT, name + '_final.png'))

        if write:
            canvas = Image.new('P', (256, 256), 0)
            canvas.paste(indices, (0, 0))
            canvas.putpalette([c for color in palette for c in color])
            path = os.path.join(ROOT, 'graphics', name + '.bmp')
            canvas.save(path + '.tmp', format='BMP')
            with open(path + '.tmp', 'rb') as handle:
                write_if_changed(path, handle.read())
            os.remove(path + '.tmp')
            write_if_changed(os.path.join(ROOT, 'graphics', name + '.json'), JSON.encode())
    print('ok', 'written' if write else 'preview only')


if __name__ == '__main__':
    main()
