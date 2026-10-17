"""도둑 이벤트 장면 음악 시안 (게임에는 아직 적용하지 않는다).

네 곡 모두 단조(가단조)이고, 이벤트 장면 동안 반복해서 흐른다는 가정으로 짧게 이어지는 길이로 만든다.
 1 살금살금: 느린 스타카토 반음 선율과 박자 사이를 찍는 낮은 베이스, 발소리 같은 가벼운 틱
 2 숨죽임: 낮은 지속음 + 심장 박동 같은 킥, 가끔 높은 곳에서 톡 떨어지는 음
 3 음흉한 왈츠: 3/4박자, 낮은 베이스와 화음이 번갈아 도는 슬며시 웃는 느낌
 4 도적단 행진: 빠르고 경쾌한 단조 행진곡 (코믹하게 도망치는 느낌)
출력: build/thief_audio_preview/*.wav, music_all.wav (순서대로 이어 붙임)
수동 실행: python -B tools/preview_thief_audio.py
"""

import os
import sys

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import make_sfx as sfx
from make_sfx import RATE, square, triangle, write_wav
from preview_cat_audio import render_song

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
OUT = os.path.join(ROOT, 'build', 'thief_audio_preview')


def music_1():      # 살금살금 (96BPM, 4마디)
    melody = ('A4:0.5 .:0.5 C5:0.5 .:0.5 B4:0.5 .:0.5 A#4:0.5 .:0.5  A4:0.5 .:0.5 E5:0.5 .:0.5 D#5:0.5 .:0.5 D5:1 '
              'F5:0.5 .:0.5 E5:0.5 .:0.5 D5:0.5 .:0.5 C#5:0.5 .:0.5  C5:0.5 .:0.5 B4:0.5 .:0.5 A4:2')
    bass = ('A2:0.5 .:1.5 E3:0.5 .:1.5  A2:0.5 .:1.5 E3:0.5 .:1.5  D3:0.5 .:1.5 A2:0.5 .:1.5  E3:0.5 .:1.5 A2:2')
    ticks = [i * 1.0 for i in range(16)]
    return render_song(96, 16, melody, bass, None, lambda f, t: square(f, t, 0.125), 0.26, ticks)


def music_2():      # 숨죽임 (72BPM, 4마디: 낮은 지속음 + 박동 + 높은 톡)
    melody = ('.:3 E6:0.25 .:0.75  .:2 B5:0.25 .:1.75  .:3.5 A5:0.25 .:0.25  .:1 E6:0.25 .:0.25 B5:0.25 .:2.25')
    bass = 'A1:4 A1:4 F2:4 E2:4'
    kicks = [0, 0.5, 4, 4.5, 8, 8.5, 12, 12.5]
    return render_song(72, 16, melody, bass, None, lambda f, t: square(f, t, 0.125), 0.20, None, kicks)


def music_3():      # 음흉한 왈츠 (112BPM, 3/4 8마디 = 24박)
    melody = ('A4:1 C5:1 E5:1  D5:2 C5:1  B4:1 D5:1 F5:1  E5:3 '
              'A4:1 C5:1 E5:1  A5:2 G5:1  F5:1 E5:1 D5:1  A4:3')
    bass = 'A2:3 A2:3 G2:3 E2:3 A2:3 F2:3 E2:3 A2:3'
    arp = ''
    for chord in (('E4', 'A4'), ('E4', 'A4'), ('D4', 'G4'), ('E4', 'G#4'), ('E4', 'A4'), ('F4', 'A4'), ('E4', 'G#4'), ('E4', 'A4')):
        arp += '.:1 %s:1 %s:1 ' % chord
    return render_song(112, 24, melody, bass, arp.strip(), lambda f, t: square(f, t, 0.25), 0.26)


def music_4():      # 도적단 행진 (132BPM, 4마디)
    melody = ('A4:0.5 A4:0.5 C5:1 A4:1 E5:1  D5:0.5 C5:0.5 B4:1 A4:2 '
              'A4:0.5 A4:0.5 C5:1 A4:1 G5:1  F5:0.5 E5:0.5 D5:1 E5:2')
    bass = 'A2:1 E3:1 A2:1 E3:1  D3:1 A2:1 E3:1 E3:1  A2:1 E3:1 A2:1 E3:1  F3:1 E3:1 A2:2'
    ticks = [0.5 + i for i in range(16)]
    kicks = [0, 2, 4, 6, 8, 10, 12, 14]
    return render_song(132, 16, melody, bass, None, lambda f, t: square(f, t, 0.5), 0.28, ticks, kicks)


def main():
    os.makedirs(OUT, exist_ok=True)
    songs = [('music_1_sneak', music_1()), ('music_2_hush', music_2()), ('music_3_waltz', music_3()), ('music_4_march', music_4())]
    combined = []
    for name, data in songs:
        write_wav(os.path.join(OUT, name + '.wav'), data)
        combined += data + [0.0] * sfx.samples(1.0)
        print('%-16s %.2f초' % (name, len(data) / RATE))
    write_wav(os.path.join(OUT, 'music_all.wav'), combined)
    print('출력:', OUT)
    return 0


if __name__ == '__main__':
    sys.exit(main())
