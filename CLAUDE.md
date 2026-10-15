# 한 번만 더: Hooked Smith (GBA)

세금에 시달리는 시골 대장장이 스미스가 낡은 단검을 +29까지 강화하는 GBA 게임.
기본 루프는 플래시 게임 「검 강화하기」를 모티브로 한다(강화 비용·확률, 실패 시 파괴, 판매가 주 수입, 깨짐 방지권, 잡템·조합소).
기획 문서: https://claude.ai/code/artifact/73958658-7f11-4c5d-9b9d-c0411b66512b
경제 스펙: `docs/ECONOMY_SPEC.md`
기획 메모(용어·스토리·미정 사항): `docs/DESIGN_NOTES.md`
저장소: https://github.com/kkam616/hookedsmith (`main`)

## 작업 방식 (사용자 선호)
- 답변·요약·작업 목록은 모두 한국어로.
- 커밋·푸시는 사용자가 요청할 때만. 끝나면 "커밋할까요?"로 묻는다.
- 화면 변경은 빌드 후 mGBA에서 직접 보고 확인한 뒤 보고한다.
- 사용자가 그림(대개 AI 생성 일러스트)을 주면 GBA 스타일로 줄여 넣는 작업이 많다. 원본은 `assets/`에 두고 `tools/`의 변환 스크립트로 `graphics/`를 만든다.

## 환경 (집 PC DSLA3DU, Windows)
- 엔진: Butano 21.8.0 (C++) — `C:\gba\butano` (라이브러리 `C:\gba\butano\butano`)
- 툴체인: devkitARM (GCC 16.1.0) — `C:\devkitPro`
- 에뮬레이터: mGBA 0.10.5 — `C:\Program Files\mGBA\mGBA.exe`
- Python 3.14.7 (PATH에 있음), Pillow 사용
- Makefile: `ROMTITLE := HOOKEDSMITH`, `ROMCODE := HKSM`, `LIBBUTANO := ../butano/butano`
- EXTTOOL(빌드 전 자동 실행): `tools/make_font.py` → `tools/make_cutscenes.py`

## 빌드 / 실행
일반 cmd(MSYS2 셸 아님)에서:
```
cd C:\gba\hookedsmith
make -j8
"C:\Program Files\mGBA\mGBA.exe" hookedsmith.gba
```
- Bash/PowerShell에서는 `DEVKITARM=C:\devkitPro\devkitARM`, `DEVKITPRO=C:\devkitPro` 환경변수를 넣고 `C:\devkitPro\msys2\usr\bin\make.exe`를 직접 부른다.
- `make`는 `C:\devkitPro\msys2\usr\bin\make.exe`가 잡혀야 한다. 이 PC에는 QMK MSYS도 있으니 다른 make가 잡히면 PATH를 확인한다.
- mGBA가 ROM을 열고 있으면 링크가 실패한다. 게임 창을 닫고 빌드한다.
- ROMCODE를 바꾸면 `.gba`/`.elf`를 지우고 빌드해야 헤더에 반영된다.
- 디버그 빌드: `build/main.*`, `build/economy.*`를 지우고 `make -j8 USERFLAGS=-DHS_DEBUG`. HARD가 열리고 강화 화면에서 START로 $100,000·방지권 5장·잡템 각 5개를 받는다. 끝나면 같은 파일을 지우고 일반 빌드로 되돌린다 (플래그가 바뀌어도 make가 다시 컴파일하지 않음).
- cmd의 `tar`는 devkitPro의 GNU tar가 먼저 잡혀 `C:` 경로를 못 읽는다. zip은 `C:\Windows\System32\tar.exe`로 푼다.

### mGBA
- 기본 키: A=X, B=Z, L=A, R=S, START=Enter, SELECT=Backspace (config.ini의 keySelect는 지금 32=Space로 되어 있으니 안 먹으면 Space)
- RTC: mGBA는 홈브류의 시계를 자동으로 켜지 않는다. `%APPDATA%\mGBA\config.ini`에 `[override.HKSM]` / `hardware=1`이 들어 있다 (섹션 이름은 `gba.override.`가 아니라 `override.`). mGBA를 닫은 상태에서만 고칠 것(종료할 때 덮어씀). 없으면 타이틀이 항상 낮.
- 화면 확인(computer-use): 모니터가 두 대(BenQ GW2780 가로, BenQ GL2580 세로)라 창이 어느 쪽에 뜰지 모른다. 실행 후 PowerShell `(New-Object -ComObject WScript.Shell).AppActivate(<mGBA PID>)`로 앞으로 가져온 뒤 `hold_key`(0.1초)로 입력한다. 짧은 `key`나 반복 입력은 게임이 못 받는다. `open_application`은 빈 mGBA를 새로 띄우니 쓰지 않는다.

## 현재 상태 (마지막 커밋 a73e4e0)
흐름: 스플래시(Powered by Butano → JAY'S LOFT) → 타이틀 → 강화 화면 ⇄ 상점(L) / 화구(R) / 창고(←) / 잡화(↓)
- STORY = EASY: 오프닝 컷씬 후 강화 화면
- HARD: 컷씬 없음. STORY 클리어 후 해금 예정이라 지금은 회색·선택 불가 (`main()`의 `hard_unlocked`)
- OPTION: "COMING SOON"만 뜬다

### 타이틀·스플래시
- 타이틀 음악: `assets/audio/title_chiptune.wav`(8bit 모노, 18157Hz) → `tools/make_music.py`가 빌드 때 `audio/title_chiptune.xm`(샘플 전체 forward loop)으로 변환. 타이틀 시작 때 `bn::music_items::title_chiptune.play(1)`, 메뉴를 고르면 페이드와 함께 볼륨을 줄이고 `bn::music::stop()`. wav를 `dmg_audio/`에 넣으면 안 됨(그 폴더는 DMG 사운드용). 곡을 더 넣을 땐 `assets/audio/`에 wav를 두면 같은 방식으로 변환된다. 원곡이 있다면 배포 전 사용 허가 확인.
- 타이틀 그림은 RTC 시각으로 낮(06~14)/노을(14~22)/밤(22~06). 시계가 없으면 낮.
- 스튜디오 로고는 원본 위 패널(960x640, 240x160의 4배 픽셀아트)을 4픽셀마다 한 점씩 뽑아 만든 것 (원본 6색).
- 난수 시드는 타이틀에서 메뉴를 고른 프레임 수.

### 오프닝 컷씬
- 원본 `assets/graphics/cutscene/*.bmp`(240x112, 16색 이하, 확장자만 bmp이고 실제 PNG). `tools/make_cutscenes.py`가 빌드 때 `graphics/`에 256x256 4bpp BMP와 텍스트 박스 배경을 만든다. 팔레트 0번(가장 많은 색)을 backdrop 색으로 맞춘다.
- 씬 목록과 대사는 `src/main.cpp`의 `opening_scenes`. 화자 이름은 노란색, A로 넘기기, 대기 중 오른쪽 아래에 ▼ 깜빡임, 씬 전환 페이드.
- 대사는 자동 줄바꿈. 첫 줄 228px, 둘째 줄 212px(▼ 자리)를 넘으면 assert.

### 강화 화면 배치 (240x160)
- 배경: 대장간 그림(`assets/graphics/forge/forge_bg_source.webp` → `tools/make_forge_bg.py`, 수동 실행, 8bpp 64색). 가운데 검·모루는 지우고 스프라이트(검은 단계마다 한 장씩 `assets/graphics/swords/sword_NN_name.png` → `tools/make_swords.py`가 빌드 때 `graphics/sword_NN.bmp`로 변환, 모루 `tools/make_anvil.py`)를 올린다. 검 그림 규격: 32x64, 4bpp 인덱스, 0번 투명, 그림은 y 0~52·x 7~24. 글자 자리(위·아래 가운데)는 배경을 어둡게 눌러 둔다.
- 아이콘 버튼(모두 40px, `tools/make_icons.py`, 원본 `assets/graphics/icons/ui_icons_v2.webp`): 상점(L) 왼쪽 위, 화구(R) 오른쪽 위(상점·화구는 +0 검일 때만 컬러·사용 가능, 그 밖에는 회색), 창고(←)·잡화(↓) 왼쪽, 판매(B)·강화(A) 오른쪽. 키 글자는 아이콘 오른쪽 아래 구석. 판매·강화는 못 쓸 때 회색.
- 위쪽 가운데: 이름(메시지가 90프레임 동안 이 자리를 대신 차지) + 성공률. 방지권 질문은 이름 자리에 질문, 성공률 자리에 "A 쓴다  B 안 쓴다".
- 아래 가운데: 강화비·판매가 (가운데 정렬, 잡템 아이콘과 판매 아이콘 사이 약 72px). 1,000 이상은 `$1.3K`/`$2.4M`처럼 줄여 표시(`format_money_short`). 왼쪽 아래 방지권, 오른쪽 아래 돈.
- 스프라이트 타일(1024개)이 빠듯하다. 화면을 오갈 때 `main()`에서 `bn::core::update()`를 한 번 불러 이전 화면의 타일을 반환시킨다 (안 하면 "Sprite tiles allocate failed").

### 경제 (`docs/ECONOMY_SPEC.md`)
- 수치는 `include/economy_data.h`, 로직은 `include/economy.h` + `src/economy.cpp`, 화면은 `src/main.cpp`.
- +0~+29 구현 (+29가 최종 검 = 엔딩, 엔딩 연출은 아직). +21~+27은 재료 강화: 보관한 검(+19·+21·+22) 또는 잡템을 소모(`level_row`의 `cost_sword`/`cost_material`/`cost_qty`). 검 이름은 `include/economy_data.h`의 `sword_names`(원작 한글 이름). 금액은 `$` 표시.
- 실패 = 파괴 + 잡템 1~3개. 방지권이 충분하면 쓸지 묻고, 쓰면 단계 유지.
- 상점(L): 방지권 1/3장, 워프 +9/+13/+14/+15 (+0 검일 때만, 50%).
- 창고(←): 보관한 검 목록(A 꺼내기 = 현재 검과 맞바꿈, 현재가 +0이면 버림 / R 현재 검 보관). 잡화(↓): 잡템 목록(보기만).
- SELECT 도움말(강화 화면): 배경·검·모루 흑백, 아이콘 컬러 + 아이콘 가운데에 이름, 버튼을 누르고 있는 동안만 위쪽 가운데 세 줄에 설명(검은 숨김). 설명 문구는 `src/main.cpp`의 `help_texts` (원하는 문구로 수정).
- 화구(R, 옛 조합소): 잡템 → 방지권 / 검(창고로).
- 스펙에 없어 임시로 정한 값: 시작 $100, +0인데 강화비가 없으면 이웃이 $50, 보관함 8칸, 잡템 이름(4글자 이하), 워프 확률 50%.

### 코드·그래픽 규칙
- 한글 폰트는 빌드 때 `tools/make_font.py`가 소스 문자열에서 글자를 모아 자동 생성한다 (한글 13px 폭).
- 글자 색을 바꿀 때는 `create_private_font_palette()`로 만든 팔레트를 쓴다. 폰트와 색이 같은 팔레트를 만들면 다른 글자가 그 팔레트를 공유해 같이 색이 바뀐다.
- `bn::string<N>`은 바이트 수 기준 (한글 1자 = 3바이트). 넘치면 게임이 멈추니 넉넉히 잡는다.
- 수동 실행 변환 스크립트(빌드 때 안 돔): `make_icons.py`, `make_anvil.py`, `make_forge_bg.py`. 원본을 바꾸면 직접 실행.
- 색 줄이기는 `MAXCOVERAGE + kmeans`를 쓴다. `MEDIANCUT`은 개수가 적은 색(예: 모루의 밝은 파란색)을 다른 색과 합쳐 버린다.

## 정해 둔 것 / 미정
- 세이브는 +29 엔딩을 먼저 만든 뒤, 진행 상황 + HARD 해금 플래그를 같이 만들기로 했다.
- 영어 지원(OPTION에서 Language 전환 예정) 검토 결과: 가능하지만 준비가 필요하다.
  - 한글 문자열이 코드에 박혀 있고(81개+), 한국어 어순으로 조각을 이어 붙이는 문장이 있다 → 언어별 문자열 표 + 문장 틀로 바꿔야 한다.
  - 영어가 더 길어서 넘치는 곳: 아이콘 사이 메시지 줄, 왼쪽 정보 줄, 창고·조합소 잡템 이름 칸.
  - 언어 설정도 세이브에 저장해야 한다.
  - 추천: 화면이 더 늘기 전에 문자열 표부터 만들어 두기.
- STORY/HARD 난이도 차이는 지금 이지/하드 테이블 차이뿐이다.

## 다음 작업 (칼 이미지 교체 포함)
1. +29 엔딩
2. 세이브 (난이도, 돈, 단계, 방지권, 잡템, 보관 검, HARD 해금, 나중에 언어)
3. 문자열 표 (영어 지원 준비)
4. 재료 강화 밸런스 확인 (+21~+27 필요 재료 수급 경로, 보관함 8칸이 모자라지 않는지)
5. OPTION 항목 (Language 등)
