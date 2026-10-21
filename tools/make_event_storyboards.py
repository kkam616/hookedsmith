"""tools/record_helper.ps1로 녹화한 이벤트 프레임 폴더에서 스토리보드(컷 시트)를 만든다.
이벤트 구간(시작초~끝초) 안에서 일정한 간격으로 N장을 뽑아 시간 표시와 함께 한 장에 붙인다.
사용: python -B tools/make_event_storyboards.py <폴더> <시작초> <끝초> <컷수> <출력 PNG> [열 수]
"""
import os
import sys

from PIL import Image, ImageDraw


def main():
    folder, start, end, count, out = sys.argv[1], float(sys.argv[2]) * 1000, float(sys.argv[3]) * 1000, int(sys.argv[4]), sys.argv[5]
    cols = int(sys.argv[6]) if len(sys.argv) > 6 else 4
    times = [float(t) for t in open(os.path.join(folder, 'times.txt')).read().split()]
    picks = []
    for k in range(count):
        target = start + (end - start) * k / max(1, count - 1)
        i = min(range(len(times)), key=lambda j: abs(times[j] - target))
        picks.append(i)
    rows = (count + cols - 1) // cols
    cell_w, cell_h = 240 * 2, 160 * 2 + 18
    sheet = Image.new('RGB', (cols * cell_w + (cols + 1) * 6, rows * cell_h + (rows + 1) * 6), (24, 24, 28))
    d = ImageDraw.Draw(sheet)
    for n, i in enumerate(picks):
        im = Image.open(os.path.join(folder, 'f%04d.png' % i)).convert('RGB').resize((480, 320), Image.NEAREST)
        x = 6 + (n % cols) * (cell_w + 6)
        y = 6 + (n // cols) * (cell_h + 6)
        sheet.paste(im, (x, y + 18))
        d.text((x + 2, y + 3), 'CUT %02d   %.1fs' % (n + 1, (times[i] - start) / 1000), fill=(255, 220, 120))
    sheet.save(out)
    print(out, sheet.size)


if __name__ == '__main__':
    main()
