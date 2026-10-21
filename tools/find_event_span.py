"""녹화 폴더에서 이벤트 구간(번쩍임 시작 ~ 강화 화면 복귀 직전 페이드)을 찾아 (시작초, 끝초)를 출력한다.
이벤트 창은 화면 가운데(77,41)~(163,111)에 금빛 틀이 있고 위쪽 L/R 아이콘이 사라지는 것으로 구분한다: 위쪽 왼쪽 구석(0..40, 0..40)의 평균 밝기와 가운데 창 틀 색으로 판정.
사용: python -B tools/find_event_span.py <폴더>
"""
import os
import sys
from PIL import Image

d = sys.argv[1]
times = [float(t) for t in open(os.path.join(d, 'times.txt')).read().split()]

def has_window(im):
    # 이벤트 창 윗변: y=42에 밝은 베이지 선(206,173,123)이 x 80..160에 이어진다 (창 틀은 창 작은 크기로 열릴 때도 가운데 부근에 있다)
    px = im.load()
    hits = 0
    for y in (41, 42, 43, 44, 45, 46, 47, 48, 49, 50):
        row = [px[x, y] for x in range(96, 144, 4)]
        if sum(1 for (r, g, b) in row if abs(r - 206) < 30 and abs(g - 173) < 30 and abs(b - 123) < 30) >= 10:
            hits += 1
    return hits >= 1


flags = []
for i, t in enumerate(times):
    im = Image.open(os.path.join(d, 'f%04d.png' % i)).convert('RGB')
    flags.append(has_window(im))
first = next((i for i, f in enumerate(flags) if f), None)
last = max((i for i, f in enumerate(flags) if f), default=None)
print('start', times[max(0, first - 12)] / 1000, 'end', times[min(len(times) - 1, last + 25)] / 1000, 'window_frames', sum(flags))
