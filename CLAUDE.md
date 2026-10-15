# 한 번만 더: Hooked Smith (GBA)

세금에 시달리는 시골 대장장이 스미스가 녹슨 검을 +20까지 강화하는 GBA 게임.
기본 루프는 플래시 게임 「검 강화하기」를 모티브로 한다(강화 비용·확률, 실패 시 파괴, 판매가 주 수입, 깨짐 방지권, 잡템·조합소).
기획 문서: https://claude.ai/code/artifact/73958658-7f11-4c5d-9b9d-c0411b66512b

## 환경 (집 PC DSLA3DU, Windows)
- 엔진: Butano 21.8.0 (C++) — `C:\gba\butano` (라이브러리 `C:\gba\butano\butano`)
- 툴체인: devkitARM (GCC 16.1.0) — `C:\devkitPro`
- 에뮬레이터: mGBA 0.10.5 — `C:\Program Files\mGBA\mGBA.exe`
- Python 3.14.7 (PATH에 있음)
- Makefile: `ROMTITLE := HOOKEDSMITH`, `LIBBUTANO := ../butano/butano`

## 빌드 / 실행
일반 cmd(MSYS2 셸 아님)에서:
```
cd C:\gba\hookedsmith
make -j8
"C:\Program Files\mGBA\mGBA.exe" hookedsmith.gba
```
- `make`는 `C:\devkitPro\msys2\usr\bin\make.exe`가 잡힌다. 이 PC에는 QMK MSYS도 있으니 다른 make가 잡히면 PATH를 확인한다.
- cmd의 `tar`는 devkitPro의 GNU tar가 먼저 잡혀 `C:` 경로를 못 읽는다. zip은 `C:\Windows\System32\tar.exe`로 푼다.

## 사양 메모
- 화면 240×160, 스프라이트/배경 16색 팔레트
- 그래픽은 `graphics/`에 bmp + 같은 이름의 json (Butano 규칙)
- 단계 구간: +0~5 동네 대장간 / +6~10 소문이 퍼지다 / +11~15 왕의 눈에 띄다 / +16~20 전설에 도전

- Bash/PowerShell에서 빌드할 때는 `DEVKITARM=C:\devkitPro\devkitARM`, `DEVKITPRO=C:\devkitPro` 환경변수를 넣는다.
- mGBA가 ROM을 열고 있으면 링크가 실패한다. 게임 창을 닫고 빌드한다.
- ROM 코드는 `HKSM`. mGBA는 홈브류의 RTC를 자동으로 켜지 않으므로 `%APPDATA%\mGBA\config.ini`에 `[override.HKSM]` / `hardware=1`을 넣어 둠 (mGBA를 닫은 상태에서 고칠 것). 없으면 타이틀이 항상 낮이 된다.
- ROMCODE를 바꾸면 `.gba`/`.elf`를 지우고 빌드해야 헤더에 반영된다.
- 디버그 빌드: `build/main.*`, `build/economy.*`를 지우고 `make -j8 USERFLAGS=-DHS_DEBUG`. INFINITE가 열리고 강화 화면에서 SELECT로 $100,000·방지권 5장·잡템 각 5개를 받는다. 끝나면 같은 파일을 지우고 일반 빌드로 되돌린다 (플래그가 바뀌어도 make가 다시 컴파일하지 않음).
- mGBA 기본 키: A=X, B=Z, L=A, R=S, START=Enter, SELECT=Backspace

## 현재 상태
- 흐름: 스플래시(Powered by Butano → JAY'S LOFT) → 타이틀 메뉴 → 강화 화면 ⇄ 상점(R) / 창고(L)
- 타이틀 메뉴: STORY(이지) / INFINITE(하드, 스토리 클리어 시 해금 예정, 지금은 회색·선택 불가) / OPTION(준비 중)
- 경제: `docs/ECONOMY_SPEC.md` 기준. 수치는 `include/economy_data.h`, 로직은 `include/economy.h` + `src/economy.cpp`, 화면은 `src/main.cpp`
  - +0~+20만 구현 (+21 이후 재료 강화 행은 아직 안 넣음). 실패 = 파괴 + 잡템 1~3개, 방지권이 있으면 쓸지 묻는다
  - 상점: 방지권 1/3장, 워프 +9/+13/+14/+15 (+0 검일 때만, 50%)
  - 창고: 조합소 탭(잡템 → 방지권/보관 검), 보관한 검 탭(꺼내면 현재 검과 맞바꿈, 현재가 +0이면 버림)
  - 스펙에 없어 임시로 정한 값: 시작 $100, +0인데 강화비가 없으면 이웃이 $50, 보관함 8칸, 잡템 이름
- 한글 폰트는 빌드 때 `tools/make_font.py`가 소스 문자열에서 글자를 모아 자동 생성한다
- 스튜디오 로고는 원본 위 패널(960x640, 240x160의 4배 픽셀아트)을 4픽셀마다 한 점씩 뽑아 만든 것
- 강화 화면 위쪽 메시지 줄은 두 박스 사이라 폭 126px 이하로 쓴다

## 다음 작업
1. 세이브 (난이도, 돈, 단계, 방지권, 잡템, 보관 검, INFINITE 해금)
2. +20 엔딩
3. +16~+20 재료 강화 구조 (스펙 미정)
4. OPTION 항목
