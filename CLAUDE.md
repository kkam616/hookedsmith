# HookedSmith (GBA)

세금에 시달리는 시골 대장장이 스미스가 낡은 단검을 +29까지 강화하는 GBA 게임.
기본 루프는 플래시 게임 「검 강화하기」를 모티브로 한다(강화 비용·확률, 실패 시 파괴, 판매가 주 수입, 깨짐 방지권, 잡템·화구 교환).
저장소: https://github.com/kkam616/hookedsmith (`main`)

## 문서 (이 파일이 핸드오프, 나머지는 필요할 때 읽는다)
- **`docs/GAME_MANUAL.md`** — 구현된 시스템 전체 매뉴얼(규칙·확률 표·화면·연출·미정). 시스템을 바꾸면 같이 고친다. 표(`<!-- AUTO:... -->`)는 `python -B tools/make_manual.py`가 `include/economy_data.h`에서 다시 만든다 (손으로 고치지 말 것).
- `CHANGELOG.md` — 버전별 변경 이력 (버전 번호는 `src/main.cpp`의 `game_version`, 올릴 때 같이 고친다)
- `docs/ECONOMY_SPEC.md` — 원작 수치 CSV와 경제 규칙(구현 스펙)
- `docs/DESIGN_NOTES.md` — 용어·스토리·데이터 취급 원칙
- 기획 문서(웹): https://claude.ai/code/artifact/73958658-7f11-4c5d-9b9d-c0411b66512b

## 작업 방식 (사용자 선호)
- 답변·요약·작업 목록은 모두 한국어로.
- **커밋·푸시는 사용자가 요청할 때만.** 끝나면 "커밋할까요?"로 묻는다. 커밋할 때 `git add -A`를 쓰지 말고 파일을 골라서 올린다 (과거에 `Python/` 폴더 180MB가 푸시됐다가 되돌린 적이 있다). 올리기 전에 `git status`로 이상한 폴더가 없는지 확인.
- 화면 변경은 빌드 후 mGBA에서 직접 보고 확인한 뒤 보고한다 (아래 자동 스크린샷 사용).
- 사용자가 그림(대개 AI 생성 일러스트)을 주면 GBA 스타일로 줄여 넣는 작업이 많다. 원본은 `assets/`에 두고 `tools/`의 변환 스크립트로 `graphics/`를 만든다. 적용하기 전에 시안(`build/` 미리보기)을 먼저 보여 주고 승인받는 흐름을 선호한다.
- 확인이 끝난 시안·스크린샷은 `SendUserFile`로 바로 보내 준다.
- 결정이 필요한 곳은 선택지와 추천을 같이 제시한다.

## 환경 (집 PC DSLA3DU, Windows)
- 엔진: Butano 21.8.0 (C++) — `C:\gba\butano` (라이브러리 `C:\gba\butano\butano`)
- 툴체인: devkitARM (GCC 16.1.0) — `C:\devkitPro`
- 에뮬레이터: mGBA 0.10.5 — `C:\Program Files\mGBA\mGBA.exe`
- Python 3.14 (PATH에 있음), Pillow 사용
- Makefile: `ROMTITLE := HOOKEDSMITH`, `ROMCODE := HKSM`, `LIBBUTANO := ../butano/butano`
- EXTTOOL(빌드 전 자동 실행): `tools/make_font.py` → `make_cutscenes.py` → `make_swords.py`(검 그림 + 연출용 `fx` 그림) → `make_music.py`

## 빌드 / 실행
- **일반+디버그 동시 빌드: PowerShell 도구에서** `& 'C:\Windows\System32\cmd.exe' /c 'tools\build_both.bat'` (`cmd`만 쓰면 msys의 cmd가 잡힌다). 결과: `hookedsmith.gba`(일반), `hookedsmith_debug.gba`(디버그, `build_debug/` 폴더를 따로 써서 섞이지 않고 저장 파일 `.sav`도 따로 생긴다). `.bat`은 ASCII만 쓴다(cmd가 시스템 코드 페이지로 읽음).
- 수동 빌드: `DEVKITARM=C:\devkitPro\devkitARM`, `DEVKITPRO=C:\devkitPro` 환경변수를 넣고 `C:\devkitPro\msys2\usr\bin\make.exe -j8`. 디버그는 `make -j8 TARGET=hookedsmith_debug BUILD=build_debug USERFLAGS=-DHS_DEBUG`.
- **Bash 도구에서 `powershell -Command`로 빌드하지 말 것**: 다른 `python`이 잡혀 PIL 오류가 나고 Python 설치 관리자가 저장소 안에 `Python/`을 설치해 버린다. Bash 도구의 python으로 큰 파일을 고치다 쓰기 오류가 나기도 한다 → 파일 수정용 python 스크립트는 파일로 써서 PowerShell로 실행하는 편이 안전하다 (heredoc 이스케이프 문제도 피한다).
- mGBA가 ROM을 열고 있으면 링크가 실패한다. 빌드 전에 mGBA를 모두 닫는다 (`Get-Process mGBA | Stop-Process`).
- ROMCODE를 바꾸면 `.gba`/`.elf`를 지우고 빌드해야 헤더에 반영된다.
- cmd의 `tar`는 devkitPro의 GNU tar가 먼저 잡혀 `C:` 경로를 못 읽는다. zip은 `C:\Windows\System32\tar.exe`로 푼다.
- 줄바꿈: `core.autocrlf=true`라 "LF will be replaced by CRLF" 경고는 정상이다. 작업 폴더 파일은 CRLF, 저장소는 LF.

### 디버그 빌드 키 (`hookedsmith_debug.gba`)
HARD가 열려 있고, **SELECT 도움말에서 ↑** = 시간대를 낮 → 노을 → 밤 → 실제 시계로 강제 순환 (도둑 이벤트·배경 확인용, 디버그에서는 도둑 확률 100%·쿨다운 2번), 강화 화면에서: **↑** = $100,000·방지권 5장·잡템 각 5개·보관 검, **B를 누른 채 ↑** = 파산 상태(게임 오버 확인), **+1 이상에서 R** = "무조건 실패 → 무조건 성공 → 끔" 순환, **+1 이상에서 L** = 단계를 +5→+14→+16→+21→+22→+23→+24→+25→+27→+28 순서로 점프 (연출·재료 강화비 확인용).

### mGBA 조작·스크린샷
- 기본 키: A=X, B=Z, L=A, R=S, START=Enter, SELECT=Space(이 PC 설정), 방향키.
- RTC: `%APPDATA%\mGBA\config.ini`에 `[override.HKSM]` / `hardware=1`이 들어 있다 (mGBA를 닫은 상태에서만 고칠 것). 없으면 시간대가 항상 낮.
- **자동 스크린샷(포커스 문제 없이, 권장)**: `. tools\mgba_auto.ps1`로 불러 `Start-Game <rom>`(mGBA를 `-3 -C screenshotPath=build\shots`로 실행), `Press $VK.A`(창에 키 메시지를 직접 보냄), `Shot '이름'`(F12 → 240x160 PNG를 `build/shots/`에 저장). 키 이름: `$VK.A/B/L/R/START/SELECT/LEFT/UP/RIGHT/DOWN`. 변수 `$VK`를 `$vk`로 덮어쓰지 말 것(PowerShell은 대소문자 구분 안 함). `Start-Game`은 `build/shots/*.png`를 지우니, 시험마다 따로 돌려서 결과를 바로 처리한다. 연출 확인은 `Stopwatch`로 프레임 시각(1프레임 ≈ 16.7ms)에 맞춰 `Shot`을 찍는다.
- `Shot`은 F12 스크린샷이 저장되지 않으면(가끔 그렇다) 창의 화면 영역을 직접 캡처해 대신 저장한다 (`ok(screen)`). 이때는 **mGBA 창이 다른 창에 가려지면 가려진 화면이 찍히니** 창을 가리지 말 것.
- 큰 확대본 만들기: Pillow로 `NEAREST` 4배.
- computer-use로 mGBA를 직접 조작하면 포커스가 바탕화면(알림 창 등)에 잡혀 입력이 막히는 일이 잦다. 위 자동 스크린샷 방법을 쓸 것.

## 현재 상태 (효과음·배경음까지 반영, 최근 이력은 `git log`)
흐름: 스플래시(Powered by Butano → JAY'S LOFT 로고) → 타이틀 → STORY/HARD 선택 → 시작 화면(NEW GAME / 슬롯 3개) → [STORY NEW GAME만 오프닝 컷씬] → 강화 화면 ⇄ 상점(L) / 화구(R) / 창고(←) / 잡화(↓) / 일시정지(START) / 게임 오버
- 구현 끝: +0~+29 강화·판매·방지권·잡템·상점·화구·창고, 시간대별 성공률 보정, 세이브 3슬롯, 일시정지, 게임 오버(파산), 단계별 성공 연출 4종, 실패·방지권 복구·+28 낚시 연출, 목록 화면 아이콘·흑백 배경, **효과음 9종·강화 화면 배경음 3곡(+6/+17/+28 단계별, 페이드)**.
- **없는 것**: +29 엔딩 화면과 HARD 해금 연결, OPTION 항목, 영어, 랜덤 이벤트, 게임 오버·엔딩 음악·효과음. 효과음·배경음은 아래 두 절.
- HARD: 컷씬 없음. 해금은 `hs::save::unlock_hard()`를 엔딩에서 부르면 된다 (지금은 영구히 잠김, 디버그 빌드만 열림).
- 자세한 규칙·확률·메시지·연출 표는 `docs/GAME_MANUAL.md`.

### 코드 지도
- `src/main.cpp` — 모든 화면 (`run_splash`, `run_title`, `run_start_select`, `run_opening_cutscene`, `run_forge`(강화 화면, 가장 큼), `run_recipe`(SELECT→START 레시피), `run_cat_event`(고양이 이벤트), `run_thief_event`(밤 도둑 이벤트), `run_shop`, `run_storage`(창고·잡화), `run_crafting`(화구), `run_pause`, `run_game_over`, `main()`).
- `include/economy_data.h` — 모든 수치 표(단계·상점·레시피·검 이름). `include/economy.h` + `src/economy.cpp` — 경제 로직(`upgrade`, `destroy`, `sell`, `buy`, `craft`, `is_bankrupt`, `success_pct`, `take_snapshot/restore`).
- `include/save.h` + `src/save.cpp` — SRAM 세이브(슬롯 3개 + HARD 해금 머리글). `include/time_band.h` + `src/time_band.cpp` — 낮/노을/밤.
- `tools/` — 변환 스크립트. **빌드 때 자동**: `make_font.py`(한글 폰트 + 외곽선 폰트), `make_cutscenes.py`, `make_swords.py`(검 프레임 4개 + 연출 그림 `fx`), `make_music.py`. **수동 실행**: `make_icons.py`, `make_anvil.py`, `make_forge_bg.py`, `make_title_icons.py`, `make_gameover.py`, `make_splash_logo.py [크기]`, `make_menu_bgs.py`, `make_item_icons.py --write`, `make_sword_icons.py --write`(검 그림을 바꾸면 다시), `make_manual.py`(매뉴얼 표), `make_thief_assets.py`(도둑 그림), `make_recipe_icon.py --write`(레시피 아이콘), `make_recipe_paper.py`(레시피 종이 배경), `make_forge_bg_time.py --write`(노을·밤 강화 배경: 낮 `forge_bg.bmp`에서 문 밖·빛줄기만 다시 칠함), `make_cat_assets.py`(고양이 그림), `make_sfx.py`(효과음 4종을 `audio/*.wav`로 만든다). 원본은 `assets/graphics/<종류>/`.

### 효과음 (칩튠, `tools/make_sfx.py` → `audio/sfx_*.wav` → `bn::sound_items::sfx_*`)
네모파·삼각파·잡음을 파이썬으로 합성한 8bit 모노 18157Hz wav. 수정하려면 스크립트의 각 함수를 고쳐 다시 실행(+ 이어 듣기용 `build/sfx_preview_all.wav`가 같이 생긴다).
- `sfx_menu_move` (0.04초 단음 "틱", 볼륨 0.6): 선택이 바뀔 때 — 타이틀 메뉴, 시작 화면, 상점·화구·창고 커서, 일시정지 메뉴·슬롯 (`play_menu_move()`).
- `sfx_hammer` (0.32초): 강화 버튼을 누르는 즉시. 땅 소리 두 번(0·0.17초)에 맞춰 검이 움찔하고 글자는 "땅! 땅! 땅!"이 처음부터 한 번에 뜬다. 망치질은 24프레임.
- **소리 지연 보정**: 소리는 play() 뒤 약 3프레임 늦게 들린다(믹서·에뮬레이터 버퍼). `sfx_latency_frames`(main.cpp)만큼 성공·실패 소리는 결과 화면보다 앞당겨 틀고(결과를 미리 알기 때문), 망치질 움찔은 그만큼 늦춘다. 어긋나면 이 값만 고친다.
- `sfx_success_0~3`: 성공 결과가 나오기 3프레임 전에 단계별로 — 0 = +1~5(0.4초, 가볍게), 1 = +6~16(0.95초, 반짝이는 상승), 2 = +17~28(1.4초, 팡파르+심벌), 3 = +29(3초, 대폭발, 210프레임 폭발 직전).
- `sfx_glow` (3.45초): +28→+29 시도와 +29 성공이 같이 쓰는 "빛이 차오르는" 소리. 둘 다 빛 연출이 시작되는 프레임에 시작하고, 결과가 나올 때 성공이면 `sfx_success_3`, 실패면 `sfx_fail`(빠직)이 이어진다.
- `sfx_select` (0.16초 "띠딩"): 확정(A)할 때 — 타이틀, 시작 화면, 일시정지, 상점·화구·창고 A, 방지권 사용 질문(`play_select()`).
- `sfx_fail` (0.15초, 가벼운 빠직): 검에 금이 가기 3프레임 전 (일반 실패는 12프레임째 금, +28 낚시는 빛이 꺼지고 금이 간 순간). 방지권으로 지켜도 금은 가므로 난다.
- `sfx_thief`(도둑의 "히히히" 웃음)와 장면 음악 `bgm_event_thief`: 도둑 이벤트(밤에 강화·판매 시 4%, 창고의 +17 이상 검 하나 또는 소지금 20%를 훔쳐 감, `run_thief_event`, 자세한 것은 `docs/GAME_MANUAL.md` 17-3).
- `sfx_event`(반짝, **이벤트 발생 공용**), `sfx_cat`(야옹 야옹): 고양이 이벤트(강화 실패 26/30/62회, `run_cat_event`, 선물 +17·18·20·21 검 중 하나를 창고로). 장면 음악 `bgm_event_cat`. 자세한 것은 `docs/GAME_MANUAL.md` 17-2. 그림은 `tools/make_cat_assets.py`.
- `sfx_sell_0~2`: 판매 버튼을 누르는 즉시 단계별로 — 0 = +1~5(0.3초, 동전 몇 닢), 1 = +6~16(0.8초), 2 = +17~28(1.5초, 금화 소나기+팡파르). 판매 화면 연출(`phase::SELL`)은 검이 번쩍이며 사라지고 동전(`fx` 그림 8~11, 최대 32개)이 튀며 소지금이 올라간다. 시안 확인용 `tools/preview_sell_fx.py`(GIF·wav, `build/sell_fx_preview/`).
- 아직 소리가 없는 곳: 취소(B), 상점 구매, 복구, 게임 오버, 컷씬.

### 강화 화면 배경음 (`tools/make_bgm.py` → `assets/audio/bgm_forge_*.wav` → 빌드 때 XM)
- +6~16 `bgm_forge_normal`(124BPM), +17~27 `bgm_forge_fast`(같은 곡 166BPM), +28 `bgm_forge_doom`(보스전용 새 곡, 84BPM), 그 밖(+0~5, +29)은 없음. 강화 화면 IDLE마다 `set_forge_bgm_level(eco.level())`로 원하는 곡을 알리고 매 프레임 `tick_forge_bgm()`이 볼륨을 오르내린다(`main.cpp`).
- 곡이 바뀌거나 꺼질 때는 45프레임에 걸쳐 서서히 작아지고(페이드 아웃) 켜질 때는 서서히 커진다(`tick_forge_bgm()`, 레벨은 `set_forge_bgm_level()`로 알린다). +28→+29 연출은 빛이 시작될 때 잦아들기 시작하고 빛이 꺼지는 순간 `cut_forge_bgm()`으로 끊는다. 곡은 8마디 루프, 수동 실행 `python -B tools/make_bgm.py`. 곡을 고치려면 스크립트의 선율 표와 `render_*`를 수정.

### 주요 설계 결정 (왜 이렇게 했나)
- 강화 화면: 왼쪽 아래 최고기록 `H +N 이름`(economy `best_level()`, 세이브 슬롯의 `best_level` 칸, 도달 기준 — 판매·파괴해도 남음). 메시지는 이 줄과 안 겹치게 한 줄 y 56·두 줄 y 45·57. 위쪽 가운데는 이름·성공률만, 칼 왼쪽 판매가 / 오른쪽 강화비, 아래 가운데는 메시지 자리(전부 외곽선 글자, 줄 폭 약 70px, `\n`으로 두 줄). 키 글자는 소지금과 안 겹치게 아이콘 구석에 둔다(`badge_corner`).
- 망치질 중에는 이름·판매가·강화비를 강화 전 값으로 유지하고, +29 성공은 폭발 순간(210프레임)에야 +29로 바꾼다 → +28 실패 "낚시 연출"과 끝까지 구분되지 않는다 (`reveal_hold`).
- 아이콘마다 자기 15색 팔레트를 쓴다(목록 화면은 팔레트가 남는다). 강화 화면은 다르다 → 아래 주의.
- 시간대는 저장하지 않고 강화할 때마다 시계로 정한다. 시계를 바꾸는 것은 막지 않는다.
- 검 이름은 일단 원작 한글 이름.

## 코드·그래픽 규칙과 주의 (여기서 자주 막힌다)
- **강화 화면은 스프라이트 팔레트(16개)가 빠듯하다.** 쓰는 것: 기본 글자(외곽선 폰트와 공유) 1, 소지금 1, 글자 색 회색·파랑·빨강 3(회색은 못 쓰는 버튼 글자와 공유), 버튼 아이콘 6, 모루 1, 검 1 = 13 + 재료 강화비 아이콘 1 + 연출용 `fx` 1. 검이 바뀔 때 새 검 팔레트가 잠깐 하나 더 필요하다. **새 색 글자·그림은 새 팔레트를 만들지 말고 기존 팔레트(`money_palette`, `red_tint`, `blue_tint`, `gray_tint`)를 돌려 쓸 것.** 부족하면 "BPP4 palette create failed"로 멈춘다. (아이콘 팔레트를 한 장으로 합치는 게 어려운 이유: 못 쓸 때 회색 처리가 팔레트 단위.)
- 스프라이트 타일 항목(`BN_CFG_SPRITE_TILES_MAX_ITEMS`)은 글꼴 글리프 종류가 많은 화면(레시피)에서 128을 넘어 "No more sprite tiles items"로 멈춰서 **192로 올렸다** (Makefile `USERFLAGS`와 `tools/build_both.bat`의 디버그 빌드 모두. 값을 바꾸면 build·build_debug를 지우고 다시 빌드해야 라이브러리에 반영된다). 레시피의 고정 글자는 타일 VRAM을 아끼려고 배경 그림에 굽는다(`make_recipe_paper.py`).
- 스프라이트 타일(1024개)도 빠듯하다. 화면을 오갈 때 `main()`에서 `bn::core::update()`를 한 번 불러 이전 화면의 타일을 반환시킨다 (안 하면 "Sprite tiles allocate failed"). 스프라이트는 최대 128개.
- 한글 폰트: `tools/make_font.py`가 소스의 문자열 리터럴에서 글자를 모아 자동 생성한다 (한글 13px 폭). 같은 글자의 사방 외곽선 폰트 `hs::font_outline`도 함께 생긴다 (폭 +2/+1, 메시지·일시정지·소지금에 쓴다). 작은 외곽선 폰트 `hs::font_small`(굴림 10px, ASCII도 굴림으로 그린다. `SMALL_SIZE`·`SMALL_TOP`)는 왼쪽 아래 최고기록에 쓴다 — 10px는 획 많은 글자가 뭉개지니(11px는 또렷) 크기를 바꿀 땐 화면으로 확인. 두 폰트의 팔레트는 같아서 색 바꾸는 방식도 같다 (`create_tinted_font_palette`, `create_two_tone_font_palette`, `text_line::set_sticky_palette`).
- 글자 색을 바꿀 때는 새로 만든 팔레트를 쓴다. 폰트와 색이 같은 팔레트를 만들면 다른 글자가 그 팔레트를 공유해 같이 색이 바뀐다.
- `bn::string<N>`은 바이트 수 기준 (한글 1자 = 3바이트). 넘치면 게임이 멈추니 넉넉히 잡는다.
- 스프라이트 글자(Butano `sprite_text_generator`)는 글자를 스프라이트 몇 개로 묶는다. `text_line`(main.cpp)에 `set_x/set_y/set_palette/set_offset/set_visible`이 있다.
- 색 줄이기는 `MAXCOVERAGE + kmeans`를 쓴다. `MEDIANCUT`은 개수가 적은 색을 합쳐 버린다.
- 검 그림 규격: 32x64, 4bpp 인덱스, 0번 투명, 그림은 y 0~52·x 7~24. 변환 때 프레임 4개(원래·금·위 조각·아래 조각)가 자동 생성된다.
- Butano 팁: `bn::sprite_ptr`을 `optional`로 들고 있다가 `.reset()`하면 타일·팔레트가 반환된다. `sword.set_item(item, 프레임)`으로 프레임을 바꾼다. 화면 번쩍임은 `bn::bg_palettes::set_fade` + `bn::sprite_palettes::set_fade`.

## 미정 / 정해야 할 것
- HARD 슬롯을 모드별로 나눌지 (지금은 시작 화면이 STORY·HARD 어느 쪽으로 들어가도 같고, 어떤 모드 슬롯이든 불러온다).
- 판매가·강화비 라벨과 값을 색으로 구분할지, 시간대를 알려 주는 표시를 둘지.
- 어두운 검 아이콘(+3, +12, +13, +15) 대비 보완 — 목록 배경이 정해진 뒤.
- "돈이 없다!"는 돈이 0이 되면 파산(게임 오버)이 먼저 걸려 거의 볼 수 없다. 의도와 맞는지 확인.
- 영어 지원(OPTION에서 Language 전환 예정): 한글 문자열이 코드에 박혀 있고(81개+) 조각을 이어 붙이는 문장이 있다 → 언어별 문자열 표 + 문장 틀로 바꿔야 한다. 영어가 더 길어 넘치는 곳: 메시지 줄(70px), 정보 줄, 잡템 이름 칸. 언어 설정도 세이브에 저장. 화면이 더 늘기 전에 문자열 표부터 만들어 두는 게 좋다.

## 다음 작업 후보
1. **+29 엔딩 + HARD 해금** (`unlock_hard()` 연결). "엔딩으로.." 메시지 뒤에 이어질 화면. 시나리오는 추후 기획 → 시간대(낮/노을/밤)별로 다르게. 틀(임시 화면)부터 만들 수도 있다.
2. 문자열 표 (영어 지원 준비) → OPTION 항목(Language).
3. 재료 강화 밸런스 확인 (+21~+27 재료 수급 경로, 창고 8칸이 모자라지 않는지).
4. 세이브 남은 것: 슬롯 삭제, 언어 저장. 상점·화구·창고·컷씬에서도 START가 동작하게.
5. 어두운 검 아이콘 보완, 배경 확정.
6. 남은 소리: 게임 오버·엔딩 음악, 취소(B)·구매·판매·복구 효과음, 컷씬 효과음. 소리를 들어 보고 고치는 것은 사용자가 실기(유선)로 확인하는 흐름이다 (블루투스 스피커는 100ms 이상 늦어 타이밍 판단이 어렵다).
