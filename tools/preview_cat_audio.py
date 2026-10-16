"""고양이 이벤트 소리 시안 (게임에는 아직 적용하지 않는다).

- 이벤트 발생 효과음 3종 (띠링 알림 / 반짝 마법 / 뽀잉 등장)
- 고양이 등장 장면 음악 3곡 (오르골 자장가 / 총총 걸음 / 포근한 왈츠)
출력: build/cat_audio_preview/*.wav, 이어 듣기용 trigger_all.wav, music_all.wav
수동 실행: python -B tools/preview_cat_audio.py
"""

import math
import os
import sys

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import make_sfx as sfx
from make_bgm import add, freq, kick, noise_hit, note_wave
from make_sfx import RATE, square, triangle, write_wav

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
OUT = os.path.join(ROOT, 'build', 'cat_audio_preview')


# ── 이벤트 발생 효과음 ─────────────────────────────────────────────────────
def bell(f, seconds, volume):
    """종소리: 기본음 + 3배음, 빨리 줄어든다"""
    return sfx.render(seconds, lambda t, p: (square(f, t, 0.25) * 0.6 + square(f * 3, t, 0.125) * 0.25) *
                      sfx.envelope(p, seconds, 0.0, 1.6) * volume)


def trigger_1():        # 띠링! — 두 음 알림
    total = 0.55
    return sfx.mix(sfx.place(bell(1319, 0.14, 0.5), 0.0, total), sfx.place(bell(1976, 0.40, 0.5), 0.12, total))


def trigger_2():        # 반짝 마법 — 위로 흩뿌리는 반짝임
    total = 0.85
    notes = [1047, 1319, 1568, 2093, 2637, 3136]
    parts = [sfx.place(bell(f, 0.22, 0.34), 0.06 * i, total) for i, f in enumerate(notes)]
    parts.append(sfx.place(bell(3136, 0.5, 0.25), 0.36, total))
    return sfx.mix(*parts)


def trigger_3():        # 뽀잉~ — 올라가는 휘파람 + 통통 튀는 마무리
    total = 0.6
    slide = sfx.glide(500, 1500, 0.28, 0.5, 0.42, 0.01, 0.08)
    pop = sfx.glide(1500, 700, 0.10, 0.25, 0.4, 0.0, 0.0)
    tick = sfx.glide(1000, 1700, 0.08, 0.25, 0.3, 0.0, 0.0)
    return sfx.mix(sfx.place(slide, 0.0, total), sfx.place(pop, 0.27, total), sfx.place(tick, 0.40, total))


# ── 고양이 등장 장면 음악 ──────────────────────────────────────────────────
def parse(track):
    """'C5:1 E5:0.5 .:1' → [(시작 박, 길이 박, 음)]"""
    events = []
    beat = 0.0

    for token in track.split():
        note, length = token.split(':')
        length = float(length)

        if note != '.':
            events.append((beat, length, note))

        beat += length

    return events


def render_song(bpm, beats, melody, bass, arp=None, melody_wave=None, melody_volume=0.30, ticks=None, kicks=None):
    seconds_per_beat = 60.0 / bpm
    buffer = [0.0] * int(beats * seconds_per_beat * RATE)
    melody_wave = melody_wave or (lambda f, t: square(f, t, 0.25))

    for start, length, note in parse(melody):
        add(buffer, int(start * seconds_per_beat * RATE),
            note_wave(freq(note), length * seconds_per_beat * 0.92, melody_wave, melody_volume, 1.1, 0.004))

    for start, length, note in parse(bass):
        add(buffer, int(start * seconds_per_beat * RATE),
            note_wave(freq(note), length * seconds_per_beat * 0.9, triangle, 0.45, 0.8))

    if arp:
        for start, length, note in parse(arp):
            add(buffer, int(start * seconds_per_beat * RATE),
                note_wave(freq(note), length * seconds_per_beat * 0.85, lambda f, t: square(f, t, 0.5), 0.09, 1.4))

    if ticks:
        for beat in ticks:
            add(buffer, int(beat * seconds_per_beat * RATE), noise_hit(0.02, 0.07, 150, int(beat * 10) + 7))

    if kicks:
        for beat in kicks:
            add(buffer, int(beat * seconds_per_beat * RATE), kick(0.4))

    return buffer


def music_1():      # 오르골 자장가: 느리고 맑게, 종소리 같은 높은 선율 (4마디, 90BPM)
    melody = ('E5:1 G5:1 C6:1 G5:1  A5:1 G5:1 E5:1 D5:1  E5:1 G5:1 C6:1 E6:1  D6:2 C6:1 .:1')
    arp = ('C4:0.5 E4:0.5 G4:0.5 E4:0.5 C4:0.5 E4:0.5 G4:0.5 E4:0.5 '
           'A3:0.5 C4:0.5 E4:0.5 C4:0.5 A3:0.5 C4:0.5 E4:0.5 C4:0.5 '
           'C4:0.5 E4:0.5 G4:0.5 E4:0.5 C4:0.5 E4:0.5 G4:0.5 E4:0.5 '
           'G3:0.5 B3:0.5 D4:0.5 B3:0.5 C4:0.5 E4:0.5 G4:0.5 .:0.5')
    bass = 'C3:2 C3:2 A2:2 A2:2 C3:2 C3:2 G2:2 C3:2'
    return render_song(90, 16, melody, bass, arp, lambda f, t: square(f, t, 0.125), 0.26)


def music_2():      # 총총 걸음: 경쾌하게 톡톡 튀는 스타카토 (4마디, 120BPM, 바장조)
    melody = ('F5:0.5 A5:0.5 C6:0.5 A5:0.5  G5:0.5 Bb5:0.5 D6:0.5 Bb5:0.5  F5:0.5 A5:0.5 C6:0.5 F6:0.5  E6:1 C6:1 '
              'A5:0.5 C6:0.5 F6:0.5 C6:0.5  Bb5:0.5 D6:0.5 G6:0.5 D6:0.5  C6:0.5 A5:0.5 G5:0.5 E5:0.5  F5:2')
    melody = melody.replace('Bb', 'A#')
    bass = ('F2:0.5 .:0.5 C3:0.5 .:0.5  G2:0.5 .:0.5 D3:0.5 .:0.5  F2:0.5 .:0.5 C3:0.5 .:0.5  C3:0.5 .:0.5 G2:0.5 .:0.5 '
            'F2:0.5 .:0.5 C3:0.5 .:0.5  A#2:0.5 .:0.5 F3:0.5 .:0.5  C3:0.5 .:0.5 G2:0.5 .:0.5  F2:1 C3:1')
    ticks = [i * 0.5 for i in range(32)]
    return render_song(120, 16, melody, bass, None, lambda f, t: square(f, t, 0.25), 0.28, ticks, [0, 2, 4, 6, 8, 10, 12, 14])


def music_3():      # 포근한 왈츠: 3/4, 한 박에 베이스 + 두 박에 화음 (4마디, 100BPM)
    melody = ('G4:1 C5:1 E5:1  D5:2 C5:1  E5:1 G5:1 C6:1  B5:3  '
              'A5:1 G5:1 E5:1  F5:1 E5:1 D5:1  C5:2 E5:1  C5:3')
    bass = 'C3:3 C3:3 A2:3 G2:3 F2:3 F2:3 G2:3 C3:3'
    arp = ''
    for chord in (('E4', 'G4'), ('E4', 'G4'), ('C4', 'E4'), ('B3', 'D4'), ('A3', 'C4'), ('A3', 'C4'), ('B3', 'D4'), ('E4', 'G4')):
        arp += '.:1 %s:1 %s:1 ' % chord
    return render_song(100, 24, melody, bass, arp.strip(), lambda f, t: square(f, t, 0.5), 0.28)


def main():
    os.makedirs(OUT, exist_ok=True)
    gap = [0.0] * sfx.samples(0.6)

    triggers = [('trigger_1_ding', trigger_1()), ('trigger_2_sparkle', trigger_2()), ('trigger_3_boing', trigger_3())]
    combined = []
    for name, data in triggers:
        write_wav(os.path.join(OUT, name + '.wav'), data)
        combined += data + gap
        print('%-20s %.2f초' % (name, len(data) / RATE))
    write_wav(os.path.join(OUT, 'trigger_all.wav'), combined)

    songs = [('music_1_musicbox', music_1()), ('music_2_trot', music_2()), ('music_3_waltz', music_3())]
    combined = []
    for name, data in songs:
        write_wav(os.path.join(OUT, name + '.wav'), data)
        combined += data + [0.0] * sfx.samples(1.0)
        print('%-20s %.2f초' % (name, len(data) / RATE))
    write_wav(os.path.join(OUT, 'music_all.wav'), combined)

    print('출력:', OUT)
    return 0


if __name__ == '__main__':
    sys.exit(main())
