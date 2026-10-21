"""새 상인 도트 시트(assets/graphics/events/merchant_sprite_new_source.png: 위 걷기 10 + 아래 서기·돈뭉치 집기 9)를
게임 규격(32x32, 15색 + 투명)으로 줄여 graphics/merchant.bmp(17프레임)로 쓴다. 수동 실행.

 - 프레임 0~9: 걷기 (오른쪽을 봄)
 - 프레임 10: 서 있는 모습 (좌우를 살필 때 좌우 반전해서 쓰고, 돈뭉치를 내려놓은 뒤에도 쓴다)
 - 프레임 11~16: 돈뭉치를 내려놓는 순서 (든 모습 → 허리를 숙임 → 바닥에 놓음). 시트의 "집어 드는" 그림을 거꾸로 쓴다 (아래 줄 8·6·4·3·2·1번)
등 뒤 짐(상자) 왼쪽 끝을 4칸에, 발 아랫선을 32칸 중 31번째 줄에 맞춘다.
사용: python -B tools/make_merchant_new_sprite.py [--write]   (미리보기: build/merchant_new_preview/board.png)
"""
import os
import sys

import numpy as np
from PIL import Image

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import make_cat_assets as cat_assets
import make_cat_new_sprite as split_tools
from make_font import write_bmp_4bpp, write_if_changed

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
SRC = os.path.join(ROOT, 'assets', 'graphics', 'events', 'merchant_sprite_new_source.png')
OUT = os.path.join(ROOT, 'build', 'merchant_new_preview')
SCALE = 30.0 / 170.0
DROP_ORDER = [8, 6, 4, 3, 2, 1]


def to_frame(arr, box):
    a, b, top, bottom = box
    rgb = Image.fromarray(arr[top:bottom + 1, a:b + 1])
    mask = Image.fromarray(((arr[top:bottom + 1, a:b + 1].astype(int).sum(axis=2) > 60) * 255).astype(np.uint8))
    nw, nh = max(1, round(rgb.width * SCALE)), max(1, round(rgb.height * SCALE))
    small = rgb.resize((nw, nh), Image.NEAREST).convert('RGBA')
    m = mask.resize((nw, nh), Image.NEAREST)
    px, mp = small.load(), m.load()
    for y in range(nh):
        for x in range(nw):
            px[x, y] = (0, 0, 0, 0) if mp[x, y] < 128 else px[x, y][:3] + (255,)
    canvas = Image.new('RGBA', (32, 32), (0, 0, 0, 0))
    canvas.paste(small, (4, 31 - nh))
    return canvas


def main():
    os.makedirs(OUT, exist_ok=True)
    arr = np.array(Image.open(SRC).convert('RGB'))
    walk = [to_frame(arr, b) for b in split_tools.frames_of(arr, 60, 380, 10)]
    bottom = [to_frame(arr, b) for b in split_tools.frames_of(arr, 400, 700, 9)]
    frames = walk + [bottom[0]] + [bottom[i] for i in DROP_ORDER]
    n = len(frames)

    flat = Image.new('RGB', (32, 32 * n), (255, 0, 255))
    alpha = np.zeros((32 * n, 32), dtype=bool)
    for k, f in enumerate(frames):
        px = f.load()
        for y in range(32):
            for x in range(32):
                if px[x, y][3] > 0:
                    flat.putpixel((x, k * 32 + y), px[x, y][:3])
                    alpha[k * 32 + y, x] = True
    q = flat.quantize(colors=15, method=Image.Quantize.MAXCOVERAGE, kmeans=5, dither=Image.Dither.NONE)
    raw = q.getpalette()
    pal = [tuple(cat_assets.gba(tuple(raw[i * 3:i * 3 + 3]))) for i in range(15)]

    sheet = Image.new('P', (32, 32 * n), 0)
    shown = []
    for k in range(n):
        im = Image.new('RGBA', (32, 32), (46, 46, 54, 255))
        for y in range(32):
            for x in range(32):
                if alpha[k * 32 + y, x]:
                    v = q.getpixel((x, k * 32 + y))
                    sheet.putpixel((x, k * 32 + y), v + 1)
                    im.putpixel((x, y), tuple(pal[v]) + (255,))
        shown.append(im)

    S = 4
    cols = 10
    board = Image.new('RGB', (cols * (32 * S + 4) + 4, 2 * (32 * S + 4) + 4), (28, 28, 34))
    for k, im in enumerate(shown):
        r, c = divmod(k, cols)
        board.paste(im.resize((32 * S, 32 * S), Image.NEAREST).convert('RGB'), (4 + c * (32 * S + 4), 4 + r * (32 * S + 4)))
    board.save(os.path.join(OUT, 'board.png'))

    if '--write' in sys.argv:
        write_if_changed(os.path.join(ROOT, 'graphics', 'merchant.bmp'), write_bmp_4bpp(None, sheet, [(255, 0, 255)] + pal))
        print('graphics/merchant.bmp 갱신 (프레임 %d장)' % n)


if __name__ == '__main__':
    main()
