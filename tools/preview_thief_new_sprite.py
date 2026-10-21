"""새 도둑 도트 시트(assets/graphics/events/thief_sprite_new_source.webp)를 게임 규격(32x32, 15색)으로 바꿔 보는 시안.
위 줄 걷기 10장, 아래 줄 6장(서 있음, 바닥 줍기 2장, 검 쥠, 검 든 모습 2장). 시안만 만든다 (build/thief_new_preview/).

사용: python -B tools/preview_thief_new_sprite.py            → 전체 프레임 보드(frames_board.png)
"""
import os
import sys

import numpy as np
from PIL import Image, ImageDraw

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
SRC = os.path.join(ROOT, 'assets', 'graphics', 'events', 'thief_sprite_new_source.webp')
OUT = os.path.join(ROOT, 'build', 'thief_new_preview')
SCALE = 28.0 / 190.0        # 서 있는 키 약 190px → 28px


def segments(mask, min_gap=12):
    cols = np.where(mask.any(axis=0))[0]
    segs, start, prev = [], cols[0], cols[0]
    for c in cols[1:]:
        if c - prev > min_gap:
            segs.append((start, prev))
            start = c
        prev = c
    segs.append((start, prev))
    return segs


def extract(arr, y0, y1):
    band = arr[y0:y1]
    mask = band.astype(int).sum(axis=2) > 60
    out = []
    for (a, b) in segments(mask):
        sub = mask[:, a:b + 1]
        ys = np.where(sub.any(axis=1))[0]
        out.append((a, b, y0 + ys.min(), y0 + ys.max()))
    return out


def to_frame(arr, box):
    a, b, top, bottom = box
    crop = Image.fromarray(arr[top:bottom + 1, a:b + 1])
    mask = (arr[top:bottom + 1, a:b + 1].astype(int).sum(axis=2) > 60)
    w, h = crop.size
    nw, nh = max(1, round(w * SCALE)), max(1, round(h * SCALE))
    small = crop.resize((nw, nh), Image.NEAREST)
    m = Image.fromarray((mask * 255).astype(np.uint8)).resize((nw, nh), Image.NEAREST)
    rgba = small.convert('RGBA')
    px, mp = rgba.load(), m.load()
    for y in range(nh):
        for x in range(nw):
            if mp[x, y] < 128:
                px[x, y] = (0, 0, 0, 0)
            else:
                px[x, y] = px[x, y][:3] + (255,)
    # 몸통 중심(위쪽 60%)을 가로 중앙(15)에, 발 아랫선을 31번째 줄에 맞춘다
    arr_small = np.array(rgba)
    upper = arr_small[: max(1, int(nh * 0.6)), :, 3] > 0
    xs = np.where(upper.any(axis=0))[0]
    cx = (xs.min() + xs.max()) / 2.0
    canvas = Image.new('RGBA', (32, 32), (0, 0, 0, 0))
    ox = int(round(15.5 - cx))
    oy = 31 - nh
    canvas.alpha_composite(rgba, (ox, oy)) if (ox >= 0 and oy >= 0 and ox + nw <= 32) else canvas.paste(rgba, (ox, oy))
    return canvas, (nw, nh, ox, oy)


def main():
    os.makedirs(OUT, exist_ok=True)
    arr = np.array(Image.open(SRC).convert('RGB'))
    top = extract(arr, 95, 310)
    bottom = extract(arr, 380, 620)
    print('top', len(top), top)
    print('bottom', len(bottom), bottom)
    frames, info = [], []
    for box in top + bottom:
        f, i = to_frame(arr, box)
        frames.append(f)
        info.append(i)
    print(info)

    S = 5
    cols = 10
    rows = 2
    pad = 6
    board = Image.new('RGB', (cols * (32 * S + pad) + pad, rows * (32 * S + pad + 14) + pad), (28, 28, 34))
    d = ImageDraw.Draw(board)
    for k, f in enumerate(frames):
        r, c = (0, k) if k < len(top) else (1, k - len(top))
        x = pad + c * (32 * S + pad)
        y = pad + r * (32 * S + pad + 14)
        base = Image.new('RGBA', (32, 32), (46, 46, 54, 255))
        base.alpha_composite(f)
        board.paste(base.resize((32 * S, 32 * S), Image.NEAREST).convert('RGB'), (x, y + 14))
        d.text((x + 2, y), '%s%d' % ('T' if r == 0 else 'B', c), fill=(255, 220, 120))
    board.save(os.path.join(OUT, 'frames_board.png'))
    import pickle
    pickle.dump(frames, open(os.path.join(OUT, 'frames.pkl'), 'wb'))


if __name__ == '__main__':
    main()
