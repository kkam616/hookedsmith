# GBA 박스 표지 작업 (핸드오프)

HookedSmith의 GBA 박스 앞면(표지)을 만드는 작업의 현재 상태와 이어서 할 일. 새 세션에서 이 파일을 먼저 읽으면 바로 이어갈 수 있다. (게임 코드와는 무관한 그래픽 작업이다.)

## 목표
공식 GBA 박스 앞면과 같은 디자인으로 우리 게임 표지를 만든다 (참고: 사용자가 올린 캐슬바니아 Circle of the Moon 북미판 표지, 약 350x347).
- 이건 **비공식 팬 게임**이다. 실제 상표 그림(닌텐도 Seal of Quality, ESRB 등급 마크, 콘솔 공식 로고 아트, 다른 회사 로고)은 우리가 **그려 넣지 않는다**. 왼쪽 띠의 "GAME BOY ADVANCE"는 비슷한 글꼴의 **글자만** 쓴다. 사용자가 공식 로고 이미지를 직접 주면 그 자리에 넣을 수는 있다.

## 레이아웃 (공식 표지를 분석한 값, 비율 기준)
| 영역 | 내용 |
|---|---|
| 왼쪽 약 14% (350 중 약 50px) | 은회색 세로 띠 + 흰 글씨(진한 남색 외곽선) "GAME BOY ADVANCE" 세로, 위쪽에 작은 "ONLY FOR" 라벨 자리 |
| 왼쪽 위(일러스트 영역) | 퍼블리셔 로고 흰 박스 → 우리는 **JAY'S LOFT 로고** (`assets/graphics/splash/jays_loft_logo.webp`, 검은 바탕을 오려서 사용) |
| 위쪽 가운데·오른쪽 | 타이틀 로고가 일러스트 위에 겹침 → **HOOKED SMITH 로고** (`assets/graphics/title/logo_source.png`, 충혈된 눈 O) |
| 가운데 | 주인공이 대각선 동작으로 튀어나오는 풀블리드 일러스트 (AI로 생성, 아래 프롬프트) |
| 왼쪽 아래 | 공식은 등급 마크 → 우리는 **넣지 않고** 작은 문구 "Unofficial fan game. / Original concept (c) NBS." |

현재 시안 스크립트의 왼쪽 띠 폭은 96/1050(약 9%)이라 공식(약 14%)보다 좁다. 최종본은 `SPINE`을 폭의 약 14%(1050 기준 150)로 바꿔 맞춘다.

## 이미 있는 것
- `tools/preview_cover.py`: 시안 1 (게임 그림 재사용 3종 A 노을 / B 밤 / C 문 앞). `build/cover/sheet.png`. 부제 "한 번만 더..."는 **사용자가 빼기로 함**. (스크립트 안 `tagline`은 더 이상 안 쓴다.)
- `tools/preview_cover2.py`: 시안 2. 코드로 그린 **임시 배경**(검, 모루, 마을 실루엣, 불씨) + 로고 + 왼쪽 GAME BOY ADVANCE 띠 + JAY'S LOFT 로고 + 아래 문구 띠. `build/cover/cover2.png`(1050x1040), `cover2_small.png`. 사용: `python -B tools/preview_cover2.py` (Windows Python, PIL).
- 스크립트들은 `tools/preview_new_title.py`의 `key_out_black`(가장자리에서 이어진 검정만 투명으로)을 쓴다.
- 로고 원본의 이미지 생성기 마크(오른쪽 아래 별)는 이미 지웠다 (v0.3.2).

## 이어서 할 일
1. 사용자가 AI로 **표지 일러스트**를 생성해 `assets/graphics/cover/` (새 폴더)에 넣는다. 파일명 예: `cover_art.png`.
2. 일러스트가 오면: 오른쪽 아래 이미지 생성기 마크(반짝이)가 있으면 지운다 (타이틀·컷씬 때처럼 왼쪽 같은 높이의 그림으로 덮기).
3. `tools/preview_cover2.py`를 고쳐 배경 `background()` 대신 이 일러스트를 쓰고, 띠 폭을 14%로 조정, 로고·JAY'S LOFT 박스·문구를 얹어 시안을 보여 준다 (`SendUserFile`로 바로 전달, 사용자 승인 후 확정).
4. 확정본을 `assets/graphics/cover/hookedsmith_cover.png`(원본 크기)로 저장. 필요하면 인쇄용 큰 사이즈와 작은 미리보기도.
5. 작업 방식: 시안 먼저 → 승인 → 적용. 커밋은 파일을 골라서 (`git add -A` 금지).

## 일러스트 프롬프트 (이미지 생성기에 붙여 넣기)
**A. 일러스트만 (권장, 띠·로고·문구는 합성):**
```
Dramatic illustrated key art for a retro handheld game box cover, in the style of early-2000s Game Boy Advance box art: semi-realistic painterly fantasy illustration with bold dark outlines, rich saturated colors, strong rim lighting, dynamic diagonal composition, full-bleed, aspect ratio 1:1 (slightly taller than wide is fine).
Subject: Smith, a stocky muscular young blacksmith with wavy medium-length messy brown hair, light stubble, wide-eyed intense obsessed expression, cream linen shirt with rolled sleeves, dark brown leather blacksmith apron with crossed shoulder straps, soot-smudged face and forearms. He bursts toward the viewer on a diagonal, one arm swinging a heavy blacksmith hammer back over his shoulder, the other hand thrusting forward a legendary sword that is glowing white-hot orange and trailing sparks and a streak of fire. Behind him: a dark night sky, a huge glowing forge fire, an iron anvil, and the silhouettes of a sleeping medieval village with a few lit windows. Shattered broken sword blades and fragments scatter in the foreground.
Composition rules: the left 14 percent of the image should be calm, darker background with no important details (a label strip will cover it). The top 28 percent, above his head, should be a dark open sky with no important details (a title logo will be placed there). The bottom-left corner should also be simple dark ground. His face and the glowing sword must sit in the center-right area.
No text, no letters, no logos, no UI, no border, no watermark, no rating badges, no seals.
```
- 오프닝 컷씬의 스미스(문 앞 그림 `assets/graphics/cutscene/scene7_forge_door.bmp` 또는 병상 그림)를 참고 이미지로 같이 올리면 옷차림·얼굴이 통일된다. 눈빛을 로고와 맞추려면 "bloodshot eyes"를 추가.

**B. 로고와 공식 표지를 참고 이미지로 올리고 전체 표지를 생성 (글자가 깨지기 쉬움, 일러스트만 쓸 것):**
```
Use Image 1 as the exact title logo (HOOKED SMITH with two bloodshot eyes) and keep it unchanged. Use Image 2 ONLY as a layout reference for a Game Boy Advance box front: a silver-gray vertical strip on the left edge about 14 percent wide with the white text "GAME BOY ADVANCE" in large letters running vertically, a small white rounded-rectangle publisher box in the top-left of the art area (leave it empty), the title logo placed at the top center-right overlapping the art, a dynamic diagonal full-bleed character illustration filling the rest, and a plain dark area at the bottom-left. Do NOT copy Image 2's character, artwork, Konami logo, Nintendo Seal of Quality, ESRB rating badge, or any of its text other than the generic "GAME BOY ADVANCE" strip. Leave those spots empty or plain.

Create an original illustration in the same early-2000s GBA box art style: semi-realistic painterly fantasy art with bold dark outlines, rich saturated colors, strong rim lighting. Subject: Smith, a stocky muscular young blacksmith with wavy medium-length messy brown hair, light stubble, wide-eyed intense bloodshot eyes, cream linen shirt with rolled sleeves, dark brown leather apron with crossed straps, soot-smudged. He bursts toward the viewer on a diagonal, swinging a heavy blacksmith hammer back over his shoulder with one arm, thrusting forward a legendary white-hot glowing orange sword trailing sparks and a streak of fire with the other. Behind: night sky, huge glowing forge fire, an anvil, silhouettes of a sleeping medieval village with a few lit windows. Shattered sword fragments scatter in the foreground. Square aspect ratio, full-bleed.
```

## 참고 자료
- 우리 로고: `assets/graphics/title/logo_source.png` (1696x624). 컷씬 그림(스미스 얼굴·옷 기준): `assets/graphics/cutscene/*`.
- 스미스 고정 묘사: 짧은 갈색 웨이브 머리, 거친 수염, 아이보리 린넨 셔츠(소매 걷음), 어두운 갈색 가죽 앞치마(등에서 엇갈린 어깨끈), 어두운 회갈색 바지, 갈색 부츠. (조끼·망토·모자·장갑 없음)
- 게임 안 팬 게임 문구: "Unofficial fan game. / Original concept © NBS." (스플래시와 동일)
