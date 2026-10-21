"""새 고양이 도트 시트(assets/graphics/events/cat_sprite_new_source2.webp: 걷기 10(앞 8장만 씀) + 앉아 꼬리 흔들기 8)를
게임 규격(32x32, 15색 + 투명)으로 줄여 graphics/cat.bmp(18프레임)로 쓴다. 수동 실행.

 - 프레임 0~7: 걷기 (오른쪽을 본다, 코끝을 같은 칸에 맞춤)
 - 프레임 8~15: 앉아서 꼬리 흔들기 (머리 가운데를 같은 칸에 맞춤, 꼬리가 왼쪽 → 오른쪽으로 넘어감)
발 아랫선은 32칸 중 31번째 줄. 그림이 도트를 약 7.8배로 키운 것이라 SCALE=32/250(nearest)로 되돌린다.
미리보기: build/cat_new_preview/new_cat_board.png (--write 없이 돌리면 graphics는 건드리지 않는다)
사용: python -B tools/make_cat_new_sprite.py [--write]
"""
import os
import sys

import numpy as np
from PIL import Image

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import make_cat_assets as cat_assets
from make_font import write_bmp_4bpp, write_if_changed

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
SRC = os.path.join(ROOT, 'assets', 'graphics', 'events', 'cat_sprite_new_source2.webp')
OUT = os.path.join(ROOT, 'build', 'cat_new_preview')
SCALE = 32.0 / 250.0


def split(mask, expect):
    """가로로 이어진 그림 덩어리를 expect 개로 나눈다 (겹친 곳은 가장 빈 열에서 자른다)"""
    cols = np.where(mask.any(axis=0))[0]
    groups, s, p = [], cols[0], cols[0]
    for c in cols[1:]:
        if c - p > 6:
            groups.append((s, p))
            s = c
        p = c
    groups.append((s, p))
    while len(groups) < expect:
        i = max(range(len(groups)), key=lambda k: groups[k][1] - groups[k][0])
        a, b = groups[i]
        n = round((b - a) / 195.0)
        if n < 2:
            n = 2
        count = mask[:, a:b + 1].sum(axis=0)
        cuts = []
        for k in range(1, n):
            lo = a + int((b - a) * k / n) - 30
            hi = lo + 60
            cuts.append(lo + int(np.argmin(count[lo - a:hi - a])))
        parts, st = [], a
        for c in cuts:
            parts.append((st, c - 1))
            st = c + 1
        parts.append((st, b))
        groups[i:i + 1] = parts
    return groups


def frames_of(arr, y0, y1, expect):
    mask = arr[y0:y1].astype(int).sum(axis=2) > 60
    out = []
    for a, b in split(mask, expect):
        sub = mask[:, a:b + 1]
        ys = np.where(sub.any(axis=1))[0]
        out.append((a, b, y0 + int(ys.min()), y0 + int(ys.max())))
    return out


def to_frame(arr, box, walking):
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
    na = np.array(small)[:, :, 3] > 0
    if walking:
        xs = np.where(na.any(axis=0))[0]
        ox = 29 - int(xs.max())                     # 코끝(맨 오른쪽)을 29칸에
    else:
        head = na[: max(1, int(nh * 0.4))]
        xs = np.where(head.any(axis=0))[0]
        ox = 16 - int(round((xs.min() + xs.max()) / 2.0))     # 머리 가운데를 16칸에
    canvas = Image.new('RGBA', (32, 32), (0, 0, 0, 0))
    canvas.paste(small, (ox, 31 - nh))
    return canvas


def main():
    os.makedirs(OUT, exist_ok=True)
    arr = np.array(Image.open(SRC).convert('RGB'))
    walk = [to_frame(arr, b, True) for b in frames_of(arr, 120, 320, 10)[:8]]      # 9·10번째는 서로 겹치거나 시트 끝에서 잘려 쓰지 않는다
    sit = [to_frame(arr, b, False) for b in frames_of(arr, 380, 600, 8)]
    frames = walk + sit
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
    board = Image.new('RGB', (8 * 32 * S + 9 * 4, 2 * 32 * S + 12), (28, 28, 34))
    for k, im in enumerate(shown):
        r, c = (0, k) if k < 8 else (1, k - 8)
        board.paste(im.resize((32 * S, 32 * S), Image.NEAREST).convert('RGB'), (4 + c * (32 * S + 4), 4 + r * (32 * S + 4)))
    board.save(os.path.join(OUT, 'new_cat_board.png'))

    if '--write' in sys.argv:
        palette = [(255, 0, 255)] + pal
        write_if_changed(os.path.join(ROOT, 'graphics', 'cat.bmp'), write_bmp_4bpp(None, sheet, palette))
        print('graphics/cat.bmp 갱신 (프레임 %d장: 걷기 8, 앉기 8)' % n)


if __name__ == '__main__':
    main()
