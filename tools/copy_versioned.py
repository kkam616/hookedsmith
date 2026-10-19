"""빌드한 ROM을 버전이 붙은 이름으로도 복사한다 (tools/build_both.bat이 빌드 끝에 부른다).

src/main.cpp의 game_version("v0.6.3")을 읽어
  hookedsmith.gba        -> hookedsmith_v0.6.3.gba
  hookedsmith_debug.gba  -> hookedsmith_debug_v0.6.3.gba
로 복사한다. 이전 버전 이름의 복사본은 지운다 (원래 이름의 파일은 그대로 둔다: 자동 시험 스크립트와 저장 파일이 이 이름을 쓴다).
주의: mGBA의 저장 파일(.sav)은 ROM 파일 이름을 따라가므로, 버전 이름 ROM은 버전마다 따로 저장된다.
"""
import glob
import os
import re
import shutil
import sys

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))


def main():
    source = open(os.path.join(ROOT, 'src', 'main.cpp'), encoding='utf-8').read()
    match = re.search(r'game_version\s*=\s*"(v[0-9][^"]*)"', source)
    if not match:
        print('game_version을 찾지 못했다')
        return 1
    version = match.group(1)

    pairs = [('hookedsmith.gba', 'hookedsmith_%s.gba' % version), ('hookedsmith_debug.gba', 'hookedsmith_debug_%s.gba' % version)]
    for original, versioned in pairs:
        src = os.path.join(ROOT, original)
        if not os.path.exists(src):
            continue
        stem = os.path.splitext(original)[0] + '_v[0-9]*.gba'      # 버전 이름 복사본만 (원래 이름 파일과 디버그 파일은 건드리지 않는다)
        for old in glob.glob(os.path.join(ROOT, stem)):
            if os.path.basename(old) != versioned:
                os.remove(old)
        shutil.copyfile(src, os.path.join(ROOT, versioned))
        print('  %s' % versioned)
    return 0


if __name__ == '__main__':
    sys.exit(main())
