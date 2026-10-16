"""강화 화면 배경음 생성기 (칩튠, 루프용 wav 3곡).

출력: assets/audio/bgm_forge_normal.wav  (+6~+16: 보통 템포)
      assets/audio/bgm_forge_fast.wav    (+17~+27: 같은 곡, 더 빠른 템포)
      assets/audio/bgm_forge_doom.wav    (+28: 느리고 장중한 새 곡)
      (+ 이어 듣기용 build/bgm_preview_all.wav)
wav는 빌드 때 tools/make_music.py가 루프 XM(audio/*.xm)으로 바꾼다 → bn::music_items::bgm_forge_*.
수동 실행: python -B tools/make_bgm.py
곡 길이가 정확히 8마디라 처음과 끝이 이어진다. 템포·음 높이·악기는 아래 상수와 곡 표에서 고친다.
"""

import math
import os
import sys

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
from make_sfx import RATE, Noise, square, triangle, write_wav  # noqa: E402

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
OUT_DIR = os.path.join(ROOT, 'assets', 'audio')
PREVIEW = os.path.join(ROOT, 'build', 'bgm_preview_all.wav')

NOTE_INDEX = {'C': 0, 'D': 2, 'E': 4, 'F': 5, 'G': 7, 'A': 9, 'B': 11}


def freq(name):
    """'A4', 'G#5' → Hz"""
    semitone = NOTE_INDEX[name[0]]
    rest = name[1:]

    if rest[0] == '#':
        semitone += 1
        rest = rest[1:]

    midi = 12 * (int(rest) + 1) + semitone
    return 440.0 * 2 ** ((midi - 69) / 12)


# 8마디(마디당 8분음표 8칸). '-'은 앞 음을 잇고 '.'은 쉼표.
CHORDS = ['A', 'F', 'C', 'G', 'A', 'F', 'G', 'E']
CHORD_TONES = {                       # 아르페지오(낮은 음 3개)와 베이스 으뜸음
    'A': ('A3', 'C4', 'E4', 'A2'),
    'F': ('F3', 'A3', 'C4', 'F2'),
    'C': ('C4', 'E4', 'G4', 'C3'),
    'G': ('G3', 'B3', 'D4', 'G2'),
    'E': ('E3', 'G#3', 'B3', 'E2'),
}
MELODY = [
    'A4 - C5 - E5 - D5 C5',
    'A4 - C5 - F5 - E5 C5',
    'G4 - C5 - E5 - G5 E5',
    'D5 - B4 - G4 - B4 D5',
    'E5 - A5 - G5 E5 C5 -',
    'F5 - E5 - C5 - A4 C5',
    'D5 - G5 - F5 - D5 B4',
    'E5 - G#5 - B5 - G#5 E5',
]


def melody_events():
    """(시작 칸, 길이 칸, 음 이름) 목록"""
    events = []
    step = 0

    for bar in MELODY:
        for token in bar.split():
            if token == '-' and events:
                start, length, note = events[-1]
                events[-1] = (start, length + 1, note)
            elif token != '.' and token != '-':
                events.append((step, 1, token))
            step += 1

    return events


def add(buffer, start_sample, data):
    for i, value in enumerate(data):
        index = start_sample + i

        if index < len(buffer):
            buffer[index] += value


def note_wave(f, seconds, wave, volume, decay=2.0, vibrato=0.0):
    count = int(seconds * RATE)
    out = []

    for i in range(count):
        t = i / RATE
        p = i / max(1, count - 1)
        env = min(1.0, t / 0.004) * (1 - p) ** decay               # 짧게 올라갔다 서서히 줄어든다
        out.append(wave(f * (1 + vibrato * math.sin(t * 36)), t) * env * volume)

    return out


def kick(volume):
    out = []

    for i in range(int(0.12 * RATE)):
        t = i / RATE
        out.append(math.sin(2 * math.pi * (150 * t - 100 / 0.24 * t * t)) * math.exp(-t * 26) * volume)

    return out


def noise_hit(seconds, volume, decay, seed):
    noise = Noise(seed)
    return [noise.next(1) * math.exp(-i / RATE * decay) * volume for i in range(int(seconds * RATE))]


def render_normal(bpm):
    """보통/빠른 곡: 네모파 주선율 + 네모파 아르페지오 + 삼각파 베이스 + 킥·스네어·하이햇"""
    eighth = 60.0 / bpm / 2
    steps = 64
    total = int(steps * eighth * RATE)
    buffer = [0.0] * total

    # 주선율
    for start, length, note in melody_events():
        add(buffer, int(start * eighth * RATE),
            note_wave(freq(note), length * eighth * 0.92, lambda f, t: square(f, t, 0.25), 0.34, 1.2, 0.004))

    for step in range(steps):
        chord = CHORDS[step // 8]
        a, b, c, root = CHORD_TONES[chord]
        at = int(step * eighth * RATE)

        # 아르페지오: 한 칸을 두 번(16분음표)으로 쪼개 3음을 돈다
        for half in range(2):
            tone = (a, b, c)[(step * 2 + half) % 3]
            add(buffer, at + int(half * eighth / 2 * RATE),
                note_wave(freq(tone), eighth / 2 * 0.8, lambda f, t: square(f, t, 0.5), 0.10, 1.5))

        # 베이스: 으뜸음, 으뜸음, 한 옥타브 위 …
        bass = freq(root) * (2 if step % 4 == 2 else 1)
        add(buffer, at, note_wave(bass, eighth * 0.85, triangle, 0.5, 0.8))

        # 타악기
        if step % 4 == 0:
            add(buffer, at, kick(0.55))
        elif step % 4 == 2:
            add(buffer, at, noise_hit(0.09, 0.30, 38, step + 1))

        add(buffer, at, noise_hit(0.025, 0.09, 140, step + 100))

    return buffer


# +28 곡: 마지막 보스를 맞이하는 곡. 느리지만(84BPM) 뒤에서 쉬지 않고 달려드는 낮은 베이스와 북, 불길한 금관 주제.
# 프리지안 도미넌트(미·파·솔#·라·시·도·레) 느낌. 4/4, 8마디 반복.
DOOM_CHORDS = ['Em', 'Em', 'F', 'Em', 'Am', 'F', 'G', 'B']
DOOM_TONES = {          # (뿌리, 5도, 3도) — 낮은 음
    'Em': ('E2', 'B2', 'G3'),
    'F': ('F2', 'C3', 'A3'),
    'Am': ('A2', 'E3', 'C4'),
    'G': ('G2', 'D3', 'B3'),
    'B': ('B1', 'F#2', 'D#3'),
}
DOOM_MELODY = [
    'E4 . E4 F4 E4 . B4 -',
    'A4 - G4 - F4 - E4 -',
    'F4 . F4 G4 F4 . C5 -',
    'B4 - A4 - G4 - F4 E4',
    'A4 - C5 - E5 - E5 F5',
    'F5 - E5 - C5 - A4 C5',
    'B4 - D5 - G5 - F5 D5',
    'F#5 - - - D#5 - E5 -',
]


def doom_events():
    global MELODY
    saved = MELODY
    MELODY = DOOM_MELODY
    try:
        return melody_events()
    finally:
        MELODY = saved


def render_doom(bpm):
    """보스전 곡: 쉬지 않는 낮은 8분음표 베이스, 모든 박의 팀파니, 2·4박 스네어, 위협적인 금관 주제.
    후반(5~8마디)은 선율에 5도 화음을 겹쳐 압박하고, 7마디는 스네어가 쏟아지며 8마디 끝에서 처음으로 몰아친다."""
    eighth = 60.0 / bpm / 2
    steps = 64
    total = int(steps * eighth * RATE)
    buffer = [0.0] * total

    organ = lambda f, t: square(f, t, 0.5) * 0.5 + square(f * 2, t, 0.25) * 0.22
    brass = lambda f, t: square(f, t, 0.25) * 0.6 + triangle(f, t) * 0.4
    growl = lambda f, t: square(f, t, 0.125) * 0.55 + triangle(f, t) * 0.55

    for bar in range(8):
        root, fifth, third = (freq(n) for n in DOOM_TONES[DOOM_CHORDS[bar]])
        at = int(bar * 8 * eighth * RATE)
        bar_seconds = 8 * eighth * 0.99

        # 마디 내내 깔리는 낮은 오르간 화음
        add(buffer, at, note_wave(root, bar_seconds, organ, 0.20, 0.25, 0.002))
        add(buffer, at, note_wave(fifth, bar_seconds, organ, 0.12, 0.25, 0.003))
        add(buffer, at, note_wave(third, bar_seconds, organ, 0.10, 0.25, 0.003))

        # 종: 1·5마디 첫 박
        if bar % 4 == 0:
            bell = freq(DOOM_TONES[DOOM_CHORDS[bar]][0].replace('2', '5').replace('1', '4'))
            add(buffer, at, note_wave(bell, 3.0, lambda f, t: triangle(f, t) * 0.7 + triangle(f * 2.76, t) * 0.3, 0.12, 3.0))
            add(buffer, at, noise_hit(0.9, 0.34, 4.5, bar + 50))        # 심벌 같은 울림

        # 쉬지 않는 낮은 베이스: 으뜸음 8분음표, 3·7번째 칸은 옥타브 위로 튄다
        for step in range(8):
            f = root * (2 if step in (3, 7) else 1)
            add(buffer, at + int(step * eighth * RATE),
                note_wave(f, eighth * 0.9, growl, 0.42 if step % 2 == 0 else 0.30, 0.7))

        # 팀파니는 모든 박, 스네어는 2·4박 (7마디는 8분음표마다)
        for beat in range(4):
            beat_at = at + int(beat * 2 * eighth * RATE)
            add(buffer, beat_at, kick(0.95 if beat % 2 == 0 else 0.7))

            if beat % 2 == 1 or bar == 6:
                add(buffer, beat_at, noise_hit(0.10, 0.40, 34, bar * 4 + beat))

        if bar == 6:
            for roll in range(4):
                add(buffer, at + int((roll * 2 + 1) * eighth * RATE), noise_hit(0.07, 0.30, 40, 200 + roll))

        if bar == 7:                         # 마지막 마디: 북이 쏟아진다
            for roll in range(8):
                add(buffer, at + int((4 + roll * 0.5) * eighth * RATE), kick(0.35 + roll * 0.09))

    for start, length, note in doom_events():
        f = freq(note)
        at = int(start * eighth * RATE)
        seconds = length * eighth * 0.95
        add(buffer, at, note_wave(f / 2, seconds, brass, 0.26, 0.35, 0.006))
        add(buffer, at, note_wave(f, seconds, brass, 0.22, 0.45, 0.008))

        if start >= 32:                      # 후반: 5도를 겹쳐 압박
            add(buffer, at, note_wave(f * 1.5, seconds, brass, 0.10, 0.45, 0.008))

    return buffer


def main():
    songs = [
        ('bgm_forge_normal', render_normal(124)),
        ('bgm_forge_fast', render_normal(166)),
        ('bgm_forge_doom', render_doom(84)),
    ]

    gap = [0.0] * int(0.8 * RATE)
    preview = []

    for name, data in songs:
        write_wav(os.path.join(OUT_DIR, name + '.wav'), data, 0.85)
        preview += data + gap
        print('%-18s %.1f초 %d KB' % (name, len(data) / RATE, len(data) // 1024))

    write_wav(PREVIEW, preview, 0.85)
    print('이어 듣기:', PREVIEW)
    return 0


if __name__ == '__main__':
    sys.exit(main())
