"""새 도둑 시안(preview_thief_new_sprite.py 결과)을 게임 규격 15색으로 줄이고 원본과 비교하는 GIF·보드를 만든다.
쓰는 프레임: 걷기 T1~T8(8장), 검 들기 B4·B5(2장, 마지막 연출), 두리번 T0·B0(서 있는 모습 2장), 검을 집은 모습 B3(검을 쥐고 팔을 내민 모습)
바닥에서 줍는 B1·B2는 쓰지 않는다.
사용: python -B tools/preview_thief_new_sprite.py 다음에 python -B tools/preview_thief_compare.py [--write]
--write: graphics/thief.bmp를 새 프레임 11장(걷기 8 + 서 있음 2 + 집기 1)으로 쓴다 (make_thief_assets.py의 도트 도둑을 대신한다)
결과: build/thief_new_preview/ (new_sheet.png, compare_*.gif, compare_board.png, in_window_compare.png)
"""
import os
import pickle
import sys

import numpy as np
from PIL import Image, ImageDraw

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import make_cat_assets as cat_assets

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
OUT = os.path.join(ROOT, 'build', 'thief_new_preview')
BG = (46, 46, 54, 255)
S = 6


def main():
    frames = pickle.load(open(os.path.join(OUT, 'frames.pkl'), 'rb'))
    T = frames[:10]
    B = frames[10:]
    picks = {'walk': T[1:9], 'stand': [T[0], B[0]], 'pick': [B[3], B[4], B[5]]}
    order = picks['walk'] + picks['stand'] + picks['pick']          # 11장

    flat = Image.new('RGB', (32, 32 * len(order)), (255, 0, 255))
    alpha = np.zeros((32 * len(order), 32), dtype=bool)
    for k, f in enumerate(order):
        px = f.load()
        for y in range(32):
            for x in range(32):
                if px[x, y][3] > 0:
                    flat.putpixel((x, k * 32 + y), px[x, y][:3])
                    alpha[k * 32 + y, x] = True
    # 투명 칸은 가장 가까운 불투명 색으로 채워 색 줄이기에 영향이 없게 한다
    q = flat.quantize(colors=15, method=Image.Quantize.MAXCOVERAGE, kmeans=5, dither=Image.Dither.NONE)
    raw = q.getpalette()
    pal = [cat_assets.gba(tuple(raw[i * 3:i * 3 + 3])) for i in range(15)]
    new = []
    for k in range(len(order)):
        im = Image.new('RGBA', (32, 32), (0, 0, 0, 0))
        for y in range(32):
            for x in range(32):
                if alpha[k * 32 + y, x]:
                    im.putpixel((x, y), tuple(pal[q.getpixel((x, k * 32 + y))]) + (255,))
        new.append(im)
    new_walk, new_stand, new_pick = new[:8], new[8:10], new[10]      # new[11], new[12]는 검을 드는 모습(+반짝임)

    if '--write' in sys.argv:
        from make_font import write_bmp_4bpp, write_if_changed
        psheet = Image.new('P', (32, 32 * len(order)), 0)
        for k in range(len(order)):
            for y in range(32):
                for x in range(32):
                    if alpha[k * 32 + y, x]:
                        psheet.putpixel((x, k * 32 + y), q.getpixel((x, k * 32 + y)) + 1)
        palette = [(255, 0, 255)] + [tuple(c) for c in pal]
        write_if_changed(os.path.join(ROOT, 'graphics', 'thief.bmp'), write_bmp_4bpp(None, psheet, palette))
        print('graphics/thief.bmp 갱신 (프레임 %d장: 걷기 8, 서 있음 2, 집기 1, 검 들기 2)' % len(order))

    sheet = Image.new('RGBA', (32, 32 * len(new)), (0, 0, 0, 0))
    for k, im in enumerate(new):
        sheet.alpha_composite(im, (0, k * 32))
    sheet.save(os.path.join(OUT, 'new_sheet_11frames.png'))

    # 원본
    old = Image.open(os.path.join(ROOT, 'graphics', 'thief.bmp'))
    opal = old.getpalette()
    olds = []
    for f in range(5):
        im = Image.new('RGBA', (32, 32), (0, 0, 0, 0))
        for y in range(32):
            for x in range(32):
                v = old.getpixel((x, f * 32 + y))
                if v:
                    im.putpixel((x, y), tuple(opal[v * 3:v * 3 + 3]) + (255,))
        olds.append(im)

    def show(im, scale=S):
        b = Image.new('RGBA', im.size, BG)
        b.alpha_composite(im)
        return b.resize((im.width * scale, im.height * scale), Image.NEAREST).convert('RGB')

    # 보드: 원본 5장 / 새 11장
    pad = 6
    cell = 32 * 4
    board = Image.new('RGB', (11 * (cell + pad) + pad, 2 * (cell + pad + 14) + pad), (28, 28, 34))
    d = ImageDraw.Draw(board)
    for r, (label, fl) in enumerate((('ORIGINAL walk4 + pick', olds), ('NEW walk8 + stand2 + pick', new))):
        y = pad + r * (cell + pad + 14)
        d.text((pad, y), label, fill=(255, 220, 120))
        for c, im in enumerate(fl):
            board.paste(show(im, 4), (pad + c * (cell + pad), y + 14))
    board.save(os.path.join(OUT, 'compare_board.png'))

    def gif(path, seq_fn, total, step=20):
        imgs = []
        for t in range(0, total, step):
            o, n = seq_fn(t)
            c = Image.new('RGB', (32 * S * 2 + 20, 32 * S + 18), (28, 28, 34))
            c.paste(show(o), (0, 18))
            c.paste(show(n), (32 * S + 20, 18))
            dd = ImageDraw.Draw(c)
            dd.text((4, 2), 'ORIGINAL', fill=(255, 220, 120))
            dd.text((32 * S + 24, 2), 'NEW', fill=(255, 220, 120))
            imgs.append(c.quantize(colors=64, dither=Image.Dither.NONE))
        imgs[0].save(path, save_all=True, append_images=imgs[1:], duration=step, loop=0)

    # 걷기: 원본 6프레임(100ms)마다 4장, 새 것 3프레임(50ms)마다 8장 → 한 바퀴 400ms로 같다
    gif(os.path.join(OUT, 'compare_walk.gif'), lambda t: (olds[(t // 100) % 4], new_walk[(t // 50) % 8]), 2400)
    # 두리번: 원본 8프레임(133ms)마다 1·3번, 새 것 서 있는 2장
    gif(os.path.join(OUT, 'compare_peek.gif'), lambda t: (olds[3 if (t // 133) % 2 else 1], new_stand[(t // 133) % 2]), 2400)

    # 방 안에서의 모습: 받침대 앞에서 검을 집는 순간 (원본 방/빈 방 + 스프라이트, x3)
    def room_view(room_name, sprite, dx):
        screen = Image.new('RGBA', (240, 160), (20, 20, 28, 255))
        room = Image.open(os.path.join(ROOT, 'graphics', room_name))
        rp = room.getpalette()
        rgba = Image.new('RGBA', room.size, (0, 0, 0, 0))
        for y in range(room.height):
            for x in range(room.width):
                v = room.getpixel((x, y))
                if v:
                    rgba.putpixel((x, y), tuple(rp[v * 3:v * 3 + 3]) + (255,))
        screen.alpha_composite(rgba, (0, 0))
        screen.alpha_composite(sprite, (120 + (34 - 27 + dx) - 16, 80 + 2 - 16))
        return screen

    views = [
        ('thief_room.bmp', olds[1], 0),
        ('thief_room_empty.bmp', olds[4], 6),
        ('thief_room.bmp', new_stand[1], 0),
        ('thief_room_empty.bmp', new_pick, 6),
    ]
    sheet2 = Image.new('RGB', (240 * 2 * 2 + 12, 160 * 2 * 2 + 12), (28, 28, 34))
    for k, (rn, sp, dx) in enumerate(views):
        v = room_view(rn, sp, dx).convert('RGB').resize((480, 320), Image.NEAREST)
        sheet2.paste(v, ((k % 2) * 486, (k // 2) * 326))
    # 위 줄 = 원본(두리번 → 집기), 아래 줄 = 새 것
    sheet2 = sheet2
    sheet2.save(os.path.join(OUT, 'in_window_compare.png'))

    # 새 것 단독 GIF(투명, 어두운 배경)
    def alone(fl, ms, prefix):
        shown = [show(im, 8).quantize(colors=32, dither=Image.Dither.NONE) for im in fl]
        shown[0].save(prefix + '_bg.gif', save_all=True, append_images=shown[1:], duration=ms, loop=0)
        clear = []
        for im in fl:
            big = im.resize((256, 256), Image.NEAREST)
            p = big.convert('RGB').quantize(colors=31, dither=Image.Dither.NONE)
            p.paste(31, mask=big.getchannel('A').point(lambda a: 255 if a < 128 else 0))
            clear.append(p)
        clear[0].save(prefix + '.gif', save_all=True, append_images=clear[1:], duration=ms, loop=0, transparency=31, disposal=2)

    alone(new_walk, 50, os.path.join(OUT, 'thief_new_walk'))
    print('colors', len(set(tuple(c) for c in pal)), 'frames', len(new))


if __name__ == '__main__':
    main()
