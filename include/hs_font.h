// 자동 생성 파일: tools/make_font.py가 만든다. 직접 고치지 말 것.

#ifndef HS_FONT_H
#define HS_FONT_H

#include "bn_sprite_font.h"
#include "bn_utf8_characters_map.h"
#include "bn_sprite_items_hs_font.h"

namespace hs
{

constexpr bn::utf8_character font_utf8_characters[] = {
    "가", "값", "강", "검", "격", "계", "고", "공", "권", "기", "까", "꺼", "난", "냈", "녹", "는",
    "다", "단", "돈", "되", "땅", "란", "률", "린", "만", "매", "모", "문", "미", "받", "방", "부",
    "비", "빵", "상", "새", "서", "설", "성", "소", "슨", "실", "쓸", "안", "었", "왕", "용", "웃",
    "원", "유", "은", "을", "의", "이", "자", "전", "점", "졌", "준", "중", "줬", "지", "진", "창",
    "최", "키", "판", "팔", "패", "하", "한", "화",
};

constexpr int8_t font_character_widths[] = {
    6, 4, 7, 8, 8, 7, 8, 4, 5, 5, 7, 7, 4, 7, 4, 7, 7, 4, 7, 7, 7, 7, 7, 7,
    7, 7, 4, 4, 6, 7, 6, 7, 8, 7, 7, 7, 7, 7, 7, 7, 7, 5, 7, 7, 7, 8, 7, 7,
    7, 8, 7, 7, 7, 7, 7, 8, 7, 7, 7, 5, 7, 5, 4, 7, 4, 7, 7, 7, 7, 7, 6, 7,
    7, 3, 4, 6, 3, 8, 7, 7, 7, 7, 6, 7, 6, 7, 7, 8, 7, 7, 7, 6, 3, 6, 8, 13,
    13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13,
    13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13,
    13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13,
};

constexpr bn::span<const bn::utf8_character> font_utf8_characters_span(font_utf8_characters);

constexpr auto font_utf8_characters_map = bn::utf8_characters_map<font_utf8_characters_span>();

constexpr bn::sprite_font font(
        bn::sprite_items::hs_font, font_utf8_characters_map.reference(), font_character_widths);

}

#endif
