"""칩튠 효과음 생성기. 만든 wav는 Butano가 효과음(bn::sound_items::이름)으로 읽는다.

네모파·삼각파·잡음만으로 8bit 모노 18157Hz wav(타이틀 음악과 같은 형식)를 만든다.
출력: audio/*.wav (+ 이어 듣기용 build/sfx_preview_all.wav)
  sfx_menu_move.wav   메뉴·슬롯 이동 (짧은 삑)
  sfx_hammer.wav      강화 버튼 후 망치질 "땅! 땅!" (0.32초, 소리는 0·0.17초)
  sfx_success.wav     강화 성공 (올라가는 팡파르)
  sfx_fail.wav        강화 실패·파괴 (가벼운 빠직: 짧은 잡음 두 번 + 작은 하강 삑, 약 0.15초)
수동 실행: python -B tools/make_sfx.py
(음악은 assets/audio/*.wav → make_music.py → audio/*.xm. 효과음은 이 스크립트가 audio/에 wav로 바로 만든다.)
"""

import math
import os
import random
import sys
import wave

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
OUT_DIR = os.path.join(ROOT, 'audio')
PREVIEW = os.path.join(ROOT, 'build', 'sfx_preview_all.wav')
RATE = 18157


def samples(seconds):
    return int(seconds * RATE)


def square(freq, t, duty=0.5):
    return 1.0 if (freq * t) % 1.0 < duty else -1.0


def triangle(freq, t):
    phase = (freq * t) % 1.0
    return 4 * abs(phase - 0.5) - 1


class Noise:
    """GBA 잡음 채널처럼 거친 잡음 (짧은 주기 LFSR 느낌)"""

    def __init__(self, seed=1):
        self.random = random.Random(seed)
        self.value = 1.0
        self.hold = 0

    def next(self, rate_divider=1):
        if self.hold <= 0:
            self.value = self.random.choice((-1.0, 1.0))
            self.hold = rate_divider
        self.hold -= 1
        return self.value


def envelope(position, length, attack=0.0, decay_power=2.0):
    """position 0~1: 처음에 빠르게 커졌다가 서서히 줄어든다"""
    if position < attack:
        return position / attack if attack else 1.0
    rest = (position - attack) / max(1e-9, 1 - attack)
    return (1 - rest) ** decay_power


def render(length_seconds, generator):
    count = samples(length_seconds)
    return [generator(i / RATE, i / max(1, count - 1)) for i in range(count)]


def mix(*tracks):
    length = max(len(t) for t in tracks)
    out = [0.0] * length
    for track in tracks:
        for i, value in enumerate(track):
            out[i] += value
    return out


def place(track, start_seconds, total_seconds):
    """track을 total_seconds 길이 안의 start_seconds 위치에 놓는다"""
    out = [0.0] * samples(total_seconds)
    offset = samples(start_seconds)
    for i, value in enumerate(track):
        if offset + i < len(out):
            out[offset + i] += value
    return out


def write_wav(path, data, gain=0.8):
    peak = max(1e-9, max(abs(v) for v in data))
    scale = min(1.0, gain / peak) if peak > gain else 1.0
    frames = bytearray()
    for value in data:
        v = max(-1.0, min(1.0, value * scale))
        frames.append(int(round(128 + v * 127)))
    os.makedirs(os.path.dirname(path), exist_ok=True)
    with wave.open(path, 'wb') as handle:
        handle.setnchannels(1)
        handle.setsampwidth(1)
        handle.setframerate(RATE)
        handle.writeframes(bytes(frames))


# ── 1) 메뉴 이동: 높고 짧은 삑 (올라가는 두 음) ──────────────────────────────────
def menu_move():
    # 누르는 즉시 들리도록 처음부터 꽉 찬 소리로 시작하는 한 번의 짧은 "틱" (두 음으로 나누지 않는다)
    return render(0.04, lambda t, p: square(1568, t, 0.5) * math.exp(-t * 55) * 0.8)


# ── 2) 망치질 땅! 땅! 땅!: 0.67초 안에 세 번 ─────────────────────────────────────
def hammer_hit(base):
    noise = Noise(7)

    def metal(t, p):
        ring = (square(base, t, 0.25) * 0.45 + square(base * 1.51, t, 0.5) * 0.3 + triangle(base * 2.37, t) * 0.25)
        thud = triangle(base * 0.16, t) * 0.7 * math.exp(-t * 28)         # 낮은 쿵
        crack = noise.next(2) * 0.8 * math.exp(-t * 90)                    # 맞는 순간의 찰칵
        return (ring * math.exp(-t * 30) + thud + crack) * 0.8

    return render(0.12, metal)


def hammer():
    total = 0.32
    hits = [place(hammer_hit(base), start, total) for base, start in ((1180, 0.0), (1280, 0.17))]
    return mix(*hits)


# ── 3) 성공: 올라가는 팡파르 ─────────────────────────────────────────────────
def success():
    notes = [(523, 0.07), (659, 0.07), (784, 0.07), (1047, 0.09)]       # C5 E5 G5 C6
    parts = []
    clock = 0.0
    for freq, seconds in notes:
        parts.append(place(render(seconds, lambda t, p, f=freq: square(f, t, 0.25) * envelope(p, seconds, 0.05, 0.6) * 0.55), clock, 1.0))
        clock += seconds
    # 마지막 높은 음을 길게 (떨림 + 겹치는 화음)
    tail = render(0.55, lambda t, p: (square(1047, t, 0.25) * 0.45 + square(1319 + 6 * math.sin(t * 38), t, 0.5) * 0.3 +
                                       triangle(523, t) * 0.35) * envelope(p, 0.55, 0.02, 1.4))
    parts.append(place(tail, clock, 1.0))
    return mix(*parts)[:samples(0.95)]


# ── 4) 실패·파괴: 가벼운 "빠직" — 짧은 잡음 두 번 + 작은 하강 삑 (약 0.15초) ─────────────────
def fail():
    noise = Noise(11)

    first = render(0.03, lambda t, p: noise.next(1) * math.exp(-t * 110) * 0.8)          # 빠
    second = render(0.04, lambda t, p: noise.next(2) * math.exp(-t * 80) * 0.6)          # 직
    chirp = render(0.07, lambda t, p: square(1900 - 1200 * p, t, 0.25) * envelope(p, 0.07, 0.0, 1.4) * 0.35)

    total = 0.15
    return mix(place(first, 0.0, total), place(chirp, 0.015, total), place(second, 0.06, total))


def main():
    sounds = [
        ('sfx_menu_move', menu_move()),
        ('sfx_hammer', hammer()),
        ('sfx_success', success()),
        ('sfx_fail', fail()),
    ]

    gap = [0.0] * samples(0.5)
    combined = []

    for name, data in sounds:
        write_wav(os.path.join(OUT_DIR, name + '.wav'), data)
        combined += data + gap
        print('%-16s %.2f초' % (name, len(data) / RATE))

    # 이어 듣기용: 메뉴 이동은 세 번 연속으로 들려 준다
    menu_data = sounds[0][1]
    menu_run = []
    for _ in range(3):
        menu_run += menu_data + [0.0] * samples(0.18)
    combined = menu_run + gap + [v for _, d in sounds[1:] for v in d + gap]
    write_wav(PREVIEW, combined)
    print('이어 듣기:', PREVIEW)
    return 0


if __name__ == '__main__':
    sys.exit(main())
