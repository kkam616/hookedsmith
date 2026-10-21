"""tools/record_helper.ps1로 녹화한 프레임 폴더(f0000.png..., times.txt)를 GIF로 묶는다.

사용: python -B tools/make_event_gifs.py <폴더> [시작초 끝초] [이름]
 - 폴더 안 PNG는 240x160, times.txt는 각 프레임의 캡처 시각(ms)이다.
 - 시작초·끝초를 주면 그 구간만 자른다 (녹화 전체를 훑어본 뒤 이벤트 구간만 쓰려고).
 - 결과: <폴더>/<이름>.gif (240x160 원본) 과 <이름>_x3.gif (3배 확대, 도트가 선명하게 보이도록 NEAREST)
"""
import os
import sys

from PIL import Image


def main():
    folder = sys.argv[1]
    start = float(sys.argv[2]) * 1000 if len(sys.argv) > 3 else 0.0
    end = float(sys.argv[3]) * 1000 if len(sys.argv) > 3 else 1e12
    name = sys.argv[4] if len(sys.argv) > 4 else 'event'

    times = [float(t) for t in open(os.path.join(folder, 'times.txt')).read().split()]
    frames = []
    for i, t in enumerate(times):
        if start <= t <= end:
            path = os.path.join(folder, 'f%04d.png' % i)
            if os.path.exists(path):
                frames.append((t, Image.open(path).convert('RGB')))
    if len(frames) < 2:
        print('프레임이 부족하다')
        return 1

    durations = []
    for i, (t, _) in enumerate(frames):
        nxt = frames[i + 1][0] if i + 1 < len(frames) else t + 600
        durations.append(max(20, int(round((nxt - t) / 10.0)) * 10))      # GIF는 10ms 단위

    for suffix, scale in (('', 1), ('_x3', 3)):
        imgs = []
        for _, im in frames:
            if scale != 1:
                im = im.resize((im.width * scale, im.height * scale), Image.NEAREST)
            imgs.append(im.quantize(colors=128, method=Image.Quantize.MEDIANCUT, dither=Image.Dither.NONE))
        out = os.path.join(folder, '%s%s.gif' % (name, suffix))
        imgs[0].save(out, save_all=True, append_images=imgs[1:], duration=durations, loop=0, optimize=False)
        print(out, len(imgs), '프레임')
    return 0


if __name__ == '__main__':
    sys.exit(main())
