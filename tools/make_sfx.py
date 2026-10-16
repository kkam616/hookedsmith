"""칩튠 효과음 생성기. 만든 wav는 Butano가 효과음(bn::sound_items::이름)으로 읽는다.

네모파·삼각파·잡음만으로 8bit 모노 18157Hz wav(타이틀 음악과 같은 형식)를 만든다.
출력: audio/*.wav (+ 이어 듣기용 build/sfx_preview_all.wav)
  sfx_menu_move.wav   메뉴·슬롯 이동 (짧은 삑)
  sfx_hammer.wav      강화 버튼 후 망치질 "땅! 땅!" (0.32초, 소리는 0·0.17초)
  sfx_select.wav      선택·확정 (밝게 올라가는 두 음 "띠딩")
  sfx_success_0~3.wav 단계별 강화 성공 (0: +1~5 / 1: +6~16 / 2: +17~28 / 3: +29)
  sfx_glow.wav        +28→+29 시도와 +29 성공이 공유하는 빛 차오르는 소리 (3.45초, 결과 직전에 정점)
  sfx_sell_0~2.wav    판매 (0: +1~5 동전 몇 닢 / 1: +6~16 쏟아지는 동전 / 2: +17~28 금화 소나기 + 팡파르)
  sfx_event.wav       이벤트 발생 알림 (반짝, 모든 이벤트 공용)
  sfx_cat.wav         고양이 야옹 야옹 (고양이 이벤트)
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


# ── 5) 선택·확정: 밝게 올라가는 두 음 (이동 소리보다 길고 또렷하다) ───────────────────────
def select():
    first = render(0.045, lambda t, p: square(988, t, 0.25) * envelope(p, 0.045, 0.0, 1.0) * 0.6)         # B5
    second = render(0.11, lambda t, p: (square(1319, t, 0.25) * 0.55 + square(1319 * 2, t, 0.5) * 0.15) * envelope(p, 0.11, 0.0, 1.6))  # E6
    click = render(0.012, lambda t, p: Noise(3).next(1) * (1 - p) * 0.25)
    total = 0.16
    return mix(place(click, 0.0, total), place(first, 0.0, total), place(second, 0.045, total))


# ── 6) 단계별 성공음: 연출 4단계(흰색 → 파랑 → 빨강 → +29 황금)와 같은 결로 점점 커진다 ───────────
def arp(notes, step, seconds, wave_duty=0.25, volume=0.5, total=None):
    """음 목록을 step초 간격으로 쌓는다. 마지막 음은 seconds 길이"""
    total = total or (step * (len(notes) - 1) + seconds)
    parts = []

    for i, freq_ in enumerate(notes):
        length = seconds if i == len(notes) - 1 else step * 1.6
        parts.append(place(render(length, lambda t, p, f=freq_: square(f, t, wave_duty) * envelope(p, length, 0.03, 0.9) * volume), step * i, total))

    return mix(*parts)


def chord(freqs, seconds, volume=0.3, vibrato=0.0):
    return render(seconds, lambda t, p: sum(square(f * (1 + vibrato * math.sin(t * 38)), t, 0.25 if i % 2 else 0.5) for i, f in enumerate(freqs)) / len(freqs) * envelope(p, seconds, 0.02, 1.2) * volume * 2)


def crash(seconds, volume):
    noise = Noise(5)
    return render(seconds, lambda t, p: noise.next(1) * math.exp(-t * (5 if seconds > 0.5 else 14)) * volume)


def success_0():       # +1~+5: 작고 가벼운 세 음 "또로롱"
    return mix(arp([784, 988, 1175], 0.055, 0.14, 0.25, 0.5, 0.40))


def success_1():       # +6~+16: 도미솔도 + 반짝이는 높은 음 (기존 성공음을 조금 더 키움)
    run = arp([523, 659, 784, 1047, 1319], 0.06, 0.22, 0.25, 0.5, 0.95)
    tail = place(chord([1047, 1319, 1568], 0.5, 0.28, 0.004), 0.30, 0.95)
    spark = place(render(0.08, lambda t, p: square(2093, t, 0.125) * envelope(p, 0.08, 0.0, 1.0) * 0.28), 0.34, 0.95)
    return mix(run, tail, spark)


def success_2():       # +17~+28: 힘찬 팡파르 — 으뜸화음 위로 솟는 선율, 마지막은 길게 끈 화음 + 심벌
    melody = arp([392, 523, 659, 784, 1047], 0.07, 0.5, 0.25, 0.5, 1.4)
    hit = place(chord([262, 330, 392, 523], 0.9, 0.36), 0.28, 1.4)
    clash = place(crash(0.5, 0.30), 0.28, 1.4)
    bass = place(render(0.5, lambda t, p: triangle(131, t) * envelope(p, 0.5, 0.0, 0.8) * 0.6), 0.28, 1.4)
    return mix(melody, hit, clash, bass)


def success_3():       # +29: 대폭발 — 올라가는 쓸어올림 → 거대한 화음 + 긴 심벌 + 떨리는 마지막 음
    total = 3.0
    sweep = render(0.45, lambda t, p: square(260 * 2 ** (p * 3), t, 0.25) * envelope(p, 0.45, 0.4, 0.5) * 0.35)
    run = arp([523, 659, 784, 1047, 1319, 1568, 2093], 0.055, 1.1, 0.25, 0.4, total)
    run = place(run, 0.40, total)
    boom = place(chord([131, 196, 262, 330, 392, 523, 659], 2.4, 0.42, 0.003), 0.55, total)
    low = place(render(1.6, lambda t, p: triangle(65, t) * envelope(p, 1.6, 0.0, 0.7) * 0.7), 0.55, total)
    shimmer = place(crash(1.6, 0.34), 0.55, total)
    return mix(place(sweep, 0.0, total), run, boom, low, shimmer)


def glow():            # +28→+29 시도·+29 성공 공통: 빛이 점점 차오르는 3.45초 (결과가 나오기 직전에 정점)
    total = 3.45
    rng = random.Random(21)

    def swell(t, p):
        f = 300 * 2 ** (p * 3.2)
        trem = 0.75 + 0.25 * math.sin(t * (14 + 30 * p))
        body = square(f, t, 0.125) * 0.5 + square(f * 1.004, t, 0.25) * 0.5
        return body * trem * (p ** 1.6) * 0.42 * (1 - max(0.0, (p - 0.97) / 0.03))

    out = render(total, swell)

    time_ = 0.25
    while time_ < total - 0.12:            # 반짝이는 높은 음: 갈수록 촘촘해진다
        p = time_ / total
        f = rng.choice((1568, 1976, 2349, 2637, 3136))
        ping = render(0.05, lambda t, q, f=f: square(f, t, 0.125) * (1 - q) ** 1.5 * (0.10 + 0.2 * p))
        out = mix(out, place(ping, time_, total))
        time_ += 0.30 - 0.24 * p
    return out


# ── 7) 판매음: 단계(+1~5 / +6~16 / +17~28)에 따라 동전 소리가 커진다 ──────────────────────────
def clink(freq, volume=0.5, length=0.09):
    """동전 부딪히는 "짤랑": 높은 음 두 개를 짧게 겹친다"""
    return render(length, lambda t, p: (square(freq, t, 0.125) * 0.6 + square(freq * 1.5, t, 0.25) * 0.4) * envelope(p, length, 0.0, 1.8) * volume)


def sell_1():       # +1~5: 동전 두세 닢 "짤랑"
    total = 0.30
    parts = [place(clink(2093, 0.45), 0.00, total), place(clink(2637, 0.42), 0.07, total), place(clink(3136, 0.36, 0.12), 0.15, total)]
    return mix(*parts)


def sell_2():       # +6~16: 동전이 쏟아지는 소리 + 반짝이는 상승음
    total = 0.80
    rng = random.Random(2)
    parts = []
    for i in range(9):
        freq = rng.choice((1760, 2093, 2349, 2637, 3136))
        parts.append(place(clink(freq, 0.34), i * 0.045 + rng.uniform(0, 0.015), total))
    parts.append(place(arp([1047, 1319, 1568, 2093], 0.05, 0.28, 0.125, 0.30, 0.5), 0.24, total))
    return mix(*parts)


def sell_3():       # +17~28: 금화 소나기 + 팡파르 화음 + 심벌
    total = 1.50
    rng = random.Random(3)
    parts = []
    for i in range(24):
        freq = rng.choice((1568, 1760, 2093, 2349, 2637, 3136, 3520))
        parts.append(place(clink(freq, 0.26, 0.08), 0.02 + i * 0.045 + rng.uniform(0, 0.02), total))
    parts.append(place(chord([262, 330, 392, 523], 0.9, 0.32), 0.0, total))
    parts.append(place(arp([523, 659, 784, 1047, 1319], 0.07, 0.5, 0.25, 0.4, 0.9), 0.30, total))
    parts.append(place(crash(0.45, 0.22), 0.0, total))
    return mix(*parts)


# ── 8) 고양이 울음 "야옹": 짧게 올라가는 "야" + 길게 내려가는 "옹" (네모파의 음높이를 미끄러뜨린다) ─────────────
def glide(start_hz, end_hz, seconds, duty, volume, vibrato=0.0, attack=0.06):
    phase = [0.0]

    def generator(t, p):
        freq = start_hz + (end_hz - start_hz) * p
        freq *= 1 + vibrato * math.sin(t * 46)
        phase[0] = (phase[0] + freq / RATE) % 1.0
        value = 1.0 if phase[0] < duty else -1.0
        return value * envelope(p, seconds, attack, 0.9) * volume

    return render(seconds, generator)


def pwm_glide(points, seconds, volume=0.5, vibrato=0.0, vib_rate=40, duty_lo=0.15, duty_hi=0.5, attack=0.05, decay=0.8):
    """points [(위치 0~1, Hz)] 사이를 미끄러지며 듀티를 바꿔 콧소리 같은 음색 변화를 준다"""
    phase = [0.0]

    def freq_at(p):
        for (p0, f0), (p1, f1) in zip(points, points[1:]):
            if p0 <= p <= p1:
                return f0 + (f1 - f0) * (p - p0) / max(1e-9, p1 - p0)
        return points[-1][1]

    def generator(t, p):
        f = freq_at(p) * (1 + vibrato * math.sin(t * vib_rate))
        phase[0] = (phase[0] + f / RATE) % 1.0
        duty = duty_lo + (duty_hi - duty_lo) * math.sin(math.pi * p)
        return (1.0 if phase[0] < duty else -1.0) * envelope(p, seconds, attack, decay) * volume

    return render(seconds, generator)


def cat_meow():         # 고양이 "야옹 야옹": 짧게 두 번 (0.8초)
    total = 0.8

    def one(base):
        ya = pwm_glide([(0, base), (1, base * 1.7)], 0.07, 0.5, 0, 40, 0.25, 0.25, 0.1, 0.9)
        ong = pwm_glide([(0, base * 1.7), (1, base * 0.8)], 0.2, 0.5, 0.03, 44, 0.375, 0.375, 0.04, 0.9)
        return mix(place(ya, 0, 0.3), place(ong, 0.06, 0.3))

    return mix(place(one(560), 0, total), place(one(620), 0.4, total))


def bell(freq_hz, seconds, volume):
    """종소리: 기본음 + 3배음, 빨리 줄어든다"""
    return render(seconds, lambda t, p: (square(freq_hz, t, 0.25) * 0.6 + square(freq_hz * 3, t, 0.125) * 0.25) *
                  envelope(p, seconds, 0.0, 1.6) * volume)


def event_sparkle():    # 이벤트 발생 (고양이 말고 다른 이벤트에서도 쓴다): 높은 종소리가 위로 흩뿌려지는 반짝임 (0.85초)
    total = 0.85
    notes = [1047, 1319, 1568, 2093, 2637, 3136]
    parts = [place(bell(f, 0.22, 0.34), 0.06 * i, total) for i, f in enumerate(notes)]
    parts.append(place(bell(3136, 0.5, 0.25), 0.36, total))
    return mix(*parts)


def main():
    sounds = [
        ('sfx_menu_move', menu_move()),
        ('sfx_hammer', hammer()),
        ('sfx_select', select()),
        ('sfx_success_0', success_0()),
        ('sfx_success_1', success_1()),
        ('sfx_success_2', success_2()),
        ('sfx_success_3', success_3()),
        ('sfx_glow', glow()),
        ('sfx_fail', fail()),
        ('sfx_sell_0', sell_1()),
        ('sfx_sell_1', sell_2()),
        ('sfx_sell_2', sell_3()),
        ('sfx_event', event_sparkle()),
        ('sfx_cat', cat_meow()),
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
