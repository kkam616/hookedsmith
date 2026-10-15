"""루프 음악 변환기 (빌드 전에 EXTTOOL로 실행).

assets/audio/*.wav (8bit 모노 PCM)를 audio/*.xm(Butano/maxmod 음악)으로 바꾼다.
maxmod는 wav를 효과음으로만 다루고 루프를 못 걸어서, wav 샘플을 처음부터 끝까지 반복(forward loop)하는
1악기·1패턴짜리 XM에 넣는다. 샘플이 반복 재생되므로 곡이 끊김 없이 이어지고,
패턴은 가장 느린 속도(speed 31, bpm 32)로 256행(약 10분)이라 패턴이 다시 시작하는 일은 거의 없다.
재생: bn::music_items::<파일 이름>.play(volume) — 루프가 기본으로 켜져 있다.
"""

import glob
import math
import os
import struct
import sys
import wave

from make_font import write_if_changed

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
SOURCE_DIR = os.path.join(ROOT, 'assets', 'audio')
OUT_DIR = os.path.join(ROOT, 'audio')

ROWS = 256
SPEED = 31
BPM = 32
XM_BASE_RATE = 8363     # C-4가 이 속도로 재생된다


def delta_encode(unsigned_samples):
    previous = 0
    out = bytearray()
    for value in unsigned_samples:
        signed = value - 128
        out.append((signed - previous) & 0xFF)
        previous = signed
    return bytes(out)


def build_xm(name, rate, samples):
    semitones = 12 * math.log2(rate / XM_BASE_RATE)
    relative = int(math.floor(semitones))
    finetune = int(round((semitones - relative) * 128))
    if finetune >= 128:
        relative += 1
        finetune -= 128

    def text(value, size):
        return value.encode('ascii', 'replace')[:size].ljust(size, b'\0')

    header = b'Extended Module: ' + text(name, 20) + b'\x1a' + text('make_music.py', 20) + struct.pack('<H', 0x0104)
    header += struct.pack('<I', 276)
    header += struct.pack('<HHHHHHHH', 1, 0, 2, 1, 1, 1, SPEED, BPM)    # 길이, 재시작, 채널, 패턴, 악기, 선형 주파수, speed, bpm
    header += bytes([0] * 256)

    # 패턴: 0행 1채널에 C-4(노트 49) + 악기 1, 나머지는 빈 칸
    pattern = bytearray()
    pattern += bytes([0x83, 49, 1]) + bytes([0x80])
    pattern += bytes([0x80]) * ((ROWS - 1) * 2)
    pattern_block = struct.pack('<IBHH', 9, 0, ROWS, len(pattern)) + bytes(pattern)

    sample_header = struct.pack('<IIIBbBBbB', len(samples), 0, len(samples), 64, finetune, 1, 128, relative, 0)
    sample_header += text('loop', 22)

    instrument = struct.pack('<I', 263) + text(name, 22) + b'\0' + struct.pack('<H', 1)
    instrument += struct.pack('<I', 40) + bytes(96) + bytes(48) + bytes(48)
    instrument += bytes(14) + struct.pack('<H', 0) + bytes(22)
    instrument += sample_header + delta_encode(samples)

    return header + pattern_block + instrument


def convert(path):
    name = os.path.splitext(os.path.basename(path))[0]

    with wave.open(path) as wav:
        if wav.getnchannels() != 1 or wav.getsampwidth() != 1:
            raise SystemExit('%s: 8bit 모노 wav여야 함' % path)
        rate = wav.getframerate()
        samples = wav.readframes(wav.getnframes())

    write_if_changed(os.path.join(OUT_DIR, name + '.xm'), build_xm(name, rate, samples))


def main():
    for path in sorted(glob.glob(os.path.join(SOURCE_DIR, '*.wav'))):
        convert(path)

    return 0


if __name__ == '__main__':
    sys.exit(main())
