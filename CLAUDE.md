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

## 현재 상태
- 흐름: 스플래시(Powered by Butano → JAY'S LOFT) → 타이틀 메뉴 → 강화 화면
- 타이틀 메뉴: STORY(이지) / INFINITE(하드, 스토리 클리어 시 해금 예정, 지금은 회색·선택 불가) / OPTION(준비 중)
- 강화 화면: 창고(L)·상점(R) 박스, 강화비용·판매가격, 검·이름·성공률, 판매(B)·강화(A) 박스, 방지권·돈
- 한글 폰트는 빌드 때 `tools/make_font.py`가 소스 문자열에서 글자를 모아 자동 생성한다
- 스튜디오 로고는 원본 위 패널(960x640, 240x160의 4배 픽셀아트)을 4픽셀마다 한 점씩 뽑아 만든 것
- 위쪽 메시지 줄은 두 박스 사이라 폭 126px 이하로 쓴다

## 다음 작업
1. 창고(L)·상점(R), 깨짐 방지권 기능
2. STORY/INFINITE 난이도 차이 정하기
3. 세이브 (INFINITE 해금 포함)
4. OPTION 항목
