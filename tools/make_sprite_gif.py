"""게임 스프라이트 시트(graphics/<이름>.bmp, 32x32 프레임을 세로로 이어 붙인 4bpp)에서 프레임을 꺼내 GIF·PNG로 만든다.
화면 녹화가 아니라 원본 그림 그대로다 (0번 색 = 투명).
사용: python -B tools/make_sprite_gif.py <이름> <첫 프레임> <프레임 수> <프레임당 ms> <출력 접두어> [확대 배수]
예: python -B tools/make_sprite_gif.py cat 0 4 83 docs/event_gifs/cat_sprite/cat_walk 8
결과: <접두어>.gif (투명 배경), <접두어>_bg.gif (어두운 배경), <접두어>_strip.png (프레임 나열), <접두어>_NN.png (프레임별, 투명)
"""
import os
import sys

from PIL import Image

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))


def main():
    name, first, count, ms, prefix = sys.argv[1], int(sys.argv[2]), int(sys.argv[3]), int(sys.argv[4]), sys.argv[5]
    scale = int(sys.argv[6]) if len(sys.argv) > 6 else 8
    sheet = Image.open(os.path.join(ROOT, 'graphics', name + '.bmp'))
    raw = sheet.getpalette()
    palette = [tuple(raw[i * 3:i * 3 + 3]) for i in range(16)]
    px = sheet.load()

    frames = []
    for f in range(first, first + count):
        im = Image.new('RGBA', (32, 32), (0, 0, 0, 0))
        for y in range(32):
            for x in range(32):
                v = px[x, f * 32 + y]
                if v != 0:
                    im.putpixel((x, y), palette[v] + (255,))
        frames.append(im.resize((32 * scale, 32 * scale), Image.NEAREST))

    out_dir = os.path.dirname(prefix)
    os.makedirs(out_dir, exist_ok=True)

    # 프레임별 PNG (투명)
    for i, im in enumerate(frames):
        im.save('%s_%02d.png' % (prefix, i))

    # 나열한 한 장
    strip = Image.new('RGBA', (32 * scale * len(frames), 32 * scale), (0, 0, 0, 0))
    for i, im in enumerate(frames):
        strip.alpha_composite(im, (i * 32 * scale, 0))
    strip.save(prefix + '_strip.png')

    # 투명 배경 GIF
    def to_gif_frame(im, bg=None):
        if bg is not None:
            base = Image.new('RGBA', im.size, bg)
            base.alpha_composite(im)
            return base.convert('RGB').quantize(colors=32, dither=Image.Dither.NONE)
        p = im.convert('RGB').quantize(colors=31, dither=Image.Dither.NONE)
        mask = im.getchannel('A').point(lambda a: 255 if a < 128 else 0)
        p.paste(31, mask=mask)                                   # 31번을 투명 색으로
        return p

    clear = [to_gif_frame(im) for im in frames]
    clear[0].save(prefix + '.gif', save_all=True, append_images=clear[1:], duration=ms, loop=0, transparency=31, disposal=2)
    shown = [to_gif_frame(im, (46, 46, 54, 255)) for im in frames]
    shown[0].save(prefix + '_bg.gif', save_all=True, append_images=shown[1:], duration=ms, loop=0)
    print(prefix, len(frames), '프레임', 'x%d' % scale)


if __name__ == '__main__':
    main()
