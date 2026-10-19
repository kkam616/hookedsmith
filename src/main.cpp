#include "bn_core.h"
#include "bn_array.h"
#include "bn_assert.h"
#include "bn_bg_palette_ptr.h"
#include "bn_bg_palettes.h"
#include "bn_sprite_palettes.h"
#include "bn_color.h"
#include "bn_fixed.h"
#include "bn_fixed_point.h"
#include "bn_math.h"
#include "bn_blending.h"
#include "bn_sprite_affine_mat_ptr.h"
#include "bn_optional.h"
#include "bn_keypad.h"
#include "bn_music.h"
#include "bn_music_items.h"
#include "bn_sound.h"
#include "bn_sound_items.h"
#include "bn_regular_bg_ptr.h"
#include "bn_seed_random.h"
#include "bn_sprite_palette_ptr.h"
#include "bn_sprite_ptr.h"
#include "bn_sprite_text_generator.h"
#include "bn_span.h"
#include "bn_string.h"
#include "bn_time.h"
#include "bn_vector.h"

#include "save.h"
#include "time_band.h"

#include "bn_regular_bg_items_forge_bg.h"
#include "bn_regular_bg_items_forge_bg_sunset.h"
#include "bn_regular_bg_items_forge_bg_night.h"
#include "bn_regular_bg_items_gameover_bg.h"
#include "bn_regular_bg_items_menu_bg_shop.h"
#include "bn_regular_bg_items_menu_bg_furnace.h"
#include "bn_regular_bg_items_menu_bg_storage.h"
#include "bn_regular_bg_items_menu_bg_bag.h"
#include "bn_regular_bg_items_title_day.h"
#include "bn_regular_bg_items_title_night.h"
#include "bn_regular_bg_items_title_sunset.h"
#include "bn_regular_bg_items_cutscene_textbox.h"
#include "bn_regular_bg_items_scene1_bedside.h"
#include "bn_regular_bg_items_scene2_anvil.h"
#include "bn_regular_bg_items_scene3_father_close.h"
#include "bn_regular_bg_items_scene4_tax_collector.h"
#include "bn_regular_bg_items_scene5_decree.h"
#include "bn_regular_bg_items_scene6_dagger.h"
#include "bn_regular_bg_items_scene7_forge_door.h"
#include "bn_regular_bg_items_scene_bankrupt.h"
#include "bn_regular_bg_items_option_bg.h"
#include "bn_sprite_items_gallery_thumb_0.h"
#include "bn_sprite_items_gallery_thumb_1.h"
#include "bn_sprite_items_gallery_thumb_2.h"
#include "bn_sprite_items_gallery_thumb_3.h"
#include "bn_sprite_items_gallery_thumb_4.h"
#include "bn_sprite_items_gallery_thumb_5.h"
#include "bn_sprite_items_gallery_locked.h"
#include "bn_sprite_items_menu_arrow.h"
#include "bn_sprite_items_merchant.h"
#include "bn_regular_bg_items_merchant_room.h"
#include "bn_regular_bg_items_merchant_room_coins.h"
#include "bn_regular_bg_items_merchant_photo.h"
#include "bn_sprite_items_gallery_icon_0.h"
#include "bn_sprite_items_gallery_icon_1.h"
#include "bn_sprite_items_gallery_icon_2.h"
#include "bn_sprite_items_anvil.h"
#include "bn_sprite_items_title_icon_hard.h"
#include "bn_sprite_items_title_icon_option.h"
#include "bn_sprite_items_title_icon_story.h"
#include "bn_sprite_items_icon_bag.h"
#include "bn_sprite_items_icon_furnace.h"
#include "bn_sprite_items_icon_sell.h"
#include "bn_sprite_items_icon_shop.h"
#include "bn_sprite_items_icon_swords.h"
#include "bn_sprite_items_icon_upgrade.h"
#include "bn_sprite_items_sword_00.h"
#include "bn_sprite_items_sword_01.h"
#include "bn_sprite_items_sword_02.h"
#include "bn_sprite_items_sword_03.h"
#include "bn_sprite_items_sword_04.h"
#include "bn_sprite_items_sword_05.h"
#include "bn_sprite_items_sword_06.h"
#include "bn_sprite_items_sword_07.h"
#include "bn_sprite_items_sword_08.h"
#include "bn_sprite_items_sword_09.h"
#include "bn_sprite_items_sword_10.h"
#include "bn_sprite_items_sword_11.h"
#include "bn_sprite_items_sword_12.h"
#include "bn_sprite_items_sword_13.h"
#include "bn_sprite_items_sword_14.h"
#include "bn_sprite_items_sword_15.h"
#include "bn_sprite_items_sword_16.h"
#include "bn_sprite_items_sword_17.h"
#include "bn_sprite_items_sword_18.h"
#include "bn_sprite_items_sword_19.h"
#include "bn_sprite_items_sword_20.h"
#include "bn_sprite_items_sword_21.h"
#include "bn_sprite_items_sword_22.h"
#include "bn_sprite_items_sword_23.h"
#include "bn_sprite_items_sword_24.h"
#include "bn_sprite_items_sword_25.h"
#include "bn_sprite_items_sword_26.h"
#include "bn_sprite_items_sword_27.h"
#include "bn_sprite_items_sword_28.h"
#include "bn_sprite_items_sword_29.h"
#include "bn_sprite_items_fx.h"
#include "bn_sprite_items_splash_logo.h"
#include "bn_sprite_items_ico_ready.h"
#include "bn_sprite_items_icon_recipe.h"
#include "bn_sprite_tiles.h"
#include "bn_sprite_items_thief.h"
#include "bn_regular_bg_items_thief_room.h"
#include "bn_regular_bg_items_thief_room_coins.h"
#include "bn_regular_bg_items_thief_room_empty.h"
#include "bn_regular_bg_items_thief_photo.h"
#include "bn_regular_bg_items_recipe_paper.h"
#include "bn_sprite_items_cat.h"
#include "bn_regular_bg_items_cat_room.h"
#include "bn_regular_bg_items_cat_photo.h"
#include "bn_sprite_items_ico_ticket.h"
#include "bn_sprite_items_ico_scrap.h"
#include "bn_sprite_items_ico_tau_bone.h"
#include "bn_sprite_items_ico_faded_glow.h"
#include "bn_sprite_items_ico_swiss_iron.h"
#include "bn_sprite_items_ico_flame_hilt.h"
#include "bn_sprite_items_ico_evil_soul.h"
#include "bn_sprite_items_ico_axe_dust.h"
#include "bn_sprite_items_ico_clear_matter.h"
#include "bn_sprite_items_ico_warp.h"
#include "bn_sprite_items_ico_sword_00.h"
#include "bn_sprite_items_ico_sword_01.h"
#include "bn_sprite_items_ico_sword_02.h"
#include "bn_sprite_items_ico_sword_03.h"
#include "bn_sprite_items_ico_sword_04.h"
#include "bn_sprite_items_ico_sword_05.h"
#include "bn_sprite_items_ico_sword_06.h"
#include "bn_sprite_items_ico_sword_07.h"
#include "bn_sprite_items_ico_sword_08.h"
#include "bn_sprite_items_ico_sword_09.h"
#include "bn_sprite_items_ico_sword_10.h"
#include "bn_sprite_items_ico_sword_11.h"
#include "bn_sprite_items_ico_sword_12.h"
#include "bn_sprite_items_ico_sword_13.h"
#include "bn_sprite_items_ico_sword_14.h"
#include "bn_sprite_items_ico_sword_15.h"
#include "bn_sprite_items_ico_sword_16.h"
#include "bn_sprite_items_ico_sword_17.h"
#include "bn_sprite_items_ico_sword_18.h"
#include "bn_sprite_items_ico_sword_19.h"
#include "bn_sprite_items_ico_sword_20.h"
#include "bn_sprite_items_ico_sword_21.h"
#include "bn_sprite_items_ico_sword_22.h"
#include "bn_sprite_items_ico_sword_23.h"
#include "bn_sprite_items_ico_sword_24.h"
#include "bn_sprite_items_ico_sword_25.h"
#include "bn_sprite_items_ico_sword_26.h"
#include "bn_sprite_items_ico_sword_27.h"
#include "bn_sprite_items_ico_sword_28.h"
#include "bn_sprite_items_ico_sword_29.h"
#include "economy.h"
#include "common_variable_8x8_sprite_font.h"
#include "hs_font.h"
#include "hs_font_outline.h"
#include "hs_font_small.h"

namespace
{

// 검 그림 (tools/make_swords.py, +0~+29). 단계마다 따로 그려진 그림이고 팔레트도 따로다.
// 목록 화면용 16x16 아이콘 (tools/make_item_icons.py, tools/make_sword_icons.py --write)
// 잡템 아이콘 순서는 hs::material 순서와 같다 (낡은 고철, 타우 뼈, 바랜 빛, 스위스 철, 불꽃 자루, 악령 혼, 도끼 가루, 투명 물질)
const bn::sprite_item* const material_icons[hs::material_count] = {
    &bn::sprite_items::ico_scrap,
    &bn::sprite_items::ico_tau_bone,
    &bn::sprite_items::ico_faded_glow,
    &bn::sprite_items::ico_swiss_iron,
    &bn::sprite_items::ico_flame_hilt,
    &bn::sprite_items::ico_evil_soul,
    &bn::sprite_items::ico_axe_dust,
    &bn::sprite_items::ico_clear_matter,
};

const bn::sprite_item* const sword_icons[hs::level_count] = {
    &bn::sprite_items::ico_sword_00,
    &bn::sprite_items::ico_sword_01,
    &bn::sprite_items::ico_sword_02,
    &bn::sprite_items::ico_sword_03,
    &bn::sprite_items::ico_sword_04,
    &bn::sprite_items::ico_sword_05,
    &bn::sprite_items::ico_sword_06,
    &bn::sprite_items::ico_sword_07,
    &bn::sprite_items::ico_sword_08,
    &bn::sprite_items::ico_sword_09,
    &bn::sprite_items::ico_sword_10,
    &bn::sprite_items::ico_sword_11,
    &bn::sprite_items::ico_sword_12,
    &bn::sprite_items::ico_sword_13,
    &bn::sprite_items::ico_sword_14,
    &bn::sprite_items::ico_sword_15,
    &bn::sprite_items::ico_sword_16,
    &bn::sprite_items::ico_sword_17,
    &bn::sprite_items::ico_sword_18,
    &bn::sprite_items::ico_sword_19,
    &bn::sprite_items::ico_sword_20,
    &bn::sprite_items::ico_sword_21,
    &bn::sprite_items::ico_sword_22,
    &bn::sprite_items::ico_sword_23,
    &bn::sprite_items::ico_sword_24,
    &bn::sprite_items::ico_sword_25,
    &bn::sprite_items::ico_sword_26,
    &bn::sprite_items::ico_sword_27,
    &bn::sprite_items::ico_sword_28,
    &bn::sprite_items::ico_sword_29,
};

const bn::sprite_item* const sword_items[hs::level_count] = {
    &bn::sprite_items::sword_00,
    &bn::sprite_items::sword_01,
    &bn::sprite_items::sword_02,
    &bn::sprite_items::sword_03,
    &bn::sprite_items::sword_04,
    &bn::sprite_items::sword_05,
    &bn::sprite_items::sword_06,
    &bn::sprite_items::sword_07,
    &bn::sprite_items::sword_08,
    &bn::sprite_items::sword_09,
    &bn::sprite_items::sword_10,
    &bn::sprite_items::sword_11,
    &bn::sprite_items::sword_12,
    &bn::sprite_items::sword_13,
    &bn::sprite_items::sword_14,
    &bn::sprite_items::sword_15,
    &bn::sprite_items::sword_16,
    &bn::sprite_items::sword_17,
    &bn::sprite_items::sword_18,
    &bn::sprite_items::sword_19,
    &bn::sprite_items::sword_20,
    &bn::sprite_items::sword_21,
    &bn::sprite_items::sword_22,
    &bn::sprite_items::sword_23,
    &bn::sprite_items::sword_24,
    &bn::sprite_items::sword_25,
    &bn::sprite_items::sword_26,
    &bn::sprite_items::sword_27,
    &bn::sprite_items::sword_28,
    &bn::sprite_items::sword_29,
};

bn::string<24> format_money(int value)
{
    bn::string<16> digits = bn::to_string<16>(value);
    bn::string<24> result("$");
    int size = digits.size();

    for(int index = 0; index < size; ++index)
    {
        if(index > 0 && (size - index) % 3 == 0)
        {
            result.push_back(',');
        }

        result.push_back(digits[index]);
    }

    return result;
}

// 좁은 자리용 금액: 1,000 이상은 $12.5K / $445K / $2.4M 처럼 줄인다 (모두 소수 한 자리까지 정확한 값만 가정)
bn::string<24> format_money_short(int value)
{
    if(value < 1000)
    {
        return format_money(value);
    }

    bool million = value >= 1000000;
    int unit = million ? 1000000 : 1000;
    int whole = value / unit;
    int tenth = (value % unit) * 10 / unit;

    bn::string<24> result("$");
    result += bn::to_string<8>(whole);

    if(tenth > 0 && whole < 100)
    {
        result.push_back('.');
        result += bn::to_string<4>(tenth);
    }

    result.push_back(million ? 'M' : 'K');
    return result;
}

// 효과음 (tools/make_sfx.py가 audio/*.wav로 만든다)
void play_menu_move()
{
    bn::sound_items::sfx_menu_move.play(bn::fixed(0.6));
}

void play_select()
{
    bn::sound_items::sfx_select.play(bn::fixed(0.8));
}

bn::string<32> sword_label(int level)
{
    bn::string<32> result("+");
    result += bn::to_string<4>(level);
    result += " ";
    result += hs::sword_names[level];
    return result;
}

// 최고기록 줄: "H +21 다색검"
bn::string<40> sword_best_label(int level, bool with_name)
{
    bn::string<40> result("H +");
    result += bn::to_string<4>(level);

    if(with_name)
    {
        result += " ";
        result += hs::sword_names[level];
    }

    return result;
}

class text_line
{

public:
    text_line(bn::sprite_text_generator& generator, int x, int y) :
        _generator(generator),
        _x(x),
        _y(y)
    {
    }

    void set_text(const bn::string_view& text)
    {
        // 같은 글이면 다시 만들지 않는다 (타일 조각화·낭비 방지). 흔들기 오프셋이 있으면 원위치가 필요하니 그땐 다시 만든다.
        if(_has_text && _offset_x == 0 && _offset_y == 0 && _text == text)
        {
            return;
        }

        _text = text;
        _has_text = true;
        _sprites.clear();
        _offset_x = 0;
        _offset_y = 0;
        _generator.generate(_x, _y, text, _sprites);

        if(_sticky_palette)
        {
            set_palette(*_sticky_palette);
            _has_text = true;
        }

        apply_visible();
    }

    // 글자 전체의 팔레트를 바꾼다. (Butano는 글자 여러 개를 스프라이트 하나로 묶어 그리므로, 일부 글자만 색을 바꾸려면 글을 나눠 만들어야 한다)
    void set_palette(const bn::sprite_palette_ptr& palette)
    {
        _has_text = false;      // 색이 바뀌었으니 다음 set_text는 새로 만들어 기본 색으로 돌린다

        for(bn::sprite_ptr& sprite : _sprites)
        {
            sprite.set_palette(palette);
        }
    }

    // set_text로 다시 그려도 계속 이 팔레트를 쓴다 (소지금 같은 고정 색 글자용)
    void set_sticky_palette(const bn::sprite_palette_ptr& palette)
    {
        _sticky_palette = palette;
        set_palette(palette);
    }

    // 다음 set_text부터 적용되는 왼쪽(정렬 기준) x 좌표
    void set_x(int x)
    {
        _has_text = false;
        _x = x;
    }

    // 다음 set_text부터 적용되는 y 좌표
    void set_y(int y)
    {
        _has_text = false;
        _y = y;
    }

    // 지금 그려진 글자를 (dx, dy)만큼 옮긴다 (글자 흔들기용). 다시 set_text를 하면 원래 자리로 돌아간다.
    void set_offset(int dx, int dy)
    {
        for(bn::sprite_ptr& sprite : _sprites)
        {
            sprite.set_position(sprite.x() - _offset_x + dx, sprite.y() - _offset_y + dy);
        }

        _offset_x = dx;
        _offset_y = dy;
    }

    // 숨겨 둔 동안 set_text를 불러도 보이지 않는다
    void set_visible(bool visible)
    {
        _visible = visible;
        apply_visible();
    }

private:
    bn::sprite_text_generator& _generator;
    int _x;
    int _y;
    bool _visible = true;
    bn::optional<bn::sprite_palette_ptr> _sticky_palette;
    int _offset_x = 0;
    int _offset_y = 0;
    bn::vector<bn::sprite_ptr, 16> _sprites;
    bn::string<128> _text;
    bool _has_text = false;

    void apply_visible()
    {
        for(bn::sprite_ptr& sprite : _sprites)
        {
            sprite.set_visible(_visible);
        }
    }
};

// 글자 색을 따로 바꾸기 위한 폰트 팔레트 복사본.
// 글리프가 안 쓰는 1번 색을 바꿔 두지 않으면, 나중에 만드는 일반 글자가 색이 같은 이 팔레트를 찾아 공유해 버린다.
bn::sprite_palette_ptr create_private_font_palette()
{
    const bn::sprite_palette_item& item = hs::font.item().palette_item();
    const bn::span<const bn::color>& source = item.colors_ref();
    bn::array<bn::color, 16> colors;

    for(int index = 0; index < source.size(); ++index)
    {
        colors[index] = source[index];
    }

    colors[1] = bn::color(31, 0, 31);

    bn::sprite_palette_ptr result = bn::sprite_palette_ptr::create_new(item);
    result.set_colors(colors);
    return result;
}

// 글자 본체(14번 색)만 body 색으로 바꾼 폰트 팔레트. 1번 색도 같이 바꿔 다른 팔레트와 합쳐지지 않게 한다.
bn::sprite_palette_ptr create_tinted_font_palette(const bn::color& body)
{
    const bn::sprite_palette_item& item = hs::font.item().palette_item();
    const bn::span<const bn::color>& source = item.colors_ref();
    bn::array<bn::color, 16> colors;

    for(int index = 0; index < source.size(); ++index)
    {
        colors[index] = source[index];
    }

    colors[1] = body;
    colors[14] = body;

    bn::sprite_palette_ptr result = bn::sprite_palette_ptr::create_new(item);
    result.set_colors(colors);
    return result;
}

// 글자 몸통(body)과 외곽선·그림자(outline) 색을 따로 정한 팔레트. 소지금을 노랑 + 주황 외곽선으로 쓴다.
bn::sprite_palette_ptr create_two_tone_font_palette(const bn::color& body, const bn::color& outline)
{
    const bn::sprite_palette_item& item = hs::font.item().palette_item();
    const bn::span<const bn::color>& source = item.colors_ref();
    bn::array<bn::color, 16> colors;

    for(int index = 0; index < source.size(); ++index)
    {
        colors[index] = source[index];
    }

    colors[1] = body;
    colors[14] = body;
    colors[12] = outline;

    bn::sprite_palette_ptr result = bn::sprite_palette_ptr::create_new(item);
    result.set_colors(colors);
    return result;
}

// 소지금 글자 색: 노랑 + 주황 외곽선
constexpr bn::color gold_body_color(31, 27, 6);
constexpr bn::color gold_outline_color(29, 13, 1);

void set_sprites_palette(bn::ivector<bn::sprite_ptr>& sprites, const bn::sprite_palette_ptr& palette)
{
    for(bn::sprite_ptr& sprite : sprites)
    {
        sprite.set_palette(palette);
    }
}

// 타이틀 그림은 하루를 셋으로 나눠 바꾼다 (카트리지 RTC 시계 기준)
// 낮 06~16시, 노을 16~21시, 밤 21~06시. 시계가 없으면 낮.
const bn::regular_bg_item& title_item_for_now()
{
    switch(hs::current_time_band())
    {

    case hs::time_band::SUNSET:
        return bn::regular_bg_items::title_sunset;

    case hs::time_band::NIGHT:
        return bn::regular_bg_items::title_night;

    default:
        return bn::regular_bg_items::title_day;
    }
}

// 강화 화면 배경도 하루를 셋으로 나눠 바꾼다 (tools/make_forge_bg_time.py): 열린 문 밖 풍경과 바닥 빛줄기만 노을·밤으로 물든다.
const bn::regular_bg_item& forge_bg_item(hs::time_band band)
{
    switch(band)
    {

    case hs::time_band::SUNSET:
        return bn::regular_bg_items::forge_bg_sunset;

    case hs::time_band::NIGHT:
        return bn::regular_bg_items::forge_bg_night;

    default:
        return bn::regular_bg_items::forge_bg;
    }
}

// 스플래시 화면 — Powered by Butano → JAY'S LOFT
constexpr int SPLASH_HOLD   = 80;   // 로고 유지 프레임
constexpr int SPLASH_FADE   = 20;   // 페이드 인/아웃 프레임

void run_splash()
{
    // ── 1단계: Powered by Butano ─────────────────────────────────────
    // 스튜디오 로고 BMP의 배경색 (10,10,12)과 맞춘다 (팔레트 0번은 투명이라 이 색이 보인다)
    bn::bg_palettes::set_transparent_color(bn::color(1, 1, 1));

    bn::sprite_text_generator gen(hs::font);
    gen.set_center_alignment();

    // 페이드 인
    for(int f = 0; f <= SPLASH_FADE; ++f)
    {
        bn::bg_palettes::set_fade(bn::color(0, 0, 0), bn::fixed(SPLASH_FADE - f) / SPLASH_FADE);
        bn::sprite_palettes::set_fade(bn::color(0, 0, 0), bn::fixed(SPLASH_FADE - f) / SPLASH_FADE);
        bn::core::update();
    }

    bn::vector<bn::sprite_ptr, 16> text_sprites;
    gen.generate(0, 0, "Powered by Butano", text_sprites);

    // Butano 문구는 그대로 두고, 잠시 뒤 저작권 문구가 부드럽게 세 번 깜빡이며(페이드 인·아웃) 강조된 뒤 남는다.
    // 문구만 따로 페이드하려고 Butano 문구와 팔레트를 나눈다.
    for(int f = 0; f < SPLASH_HOLD; ++f)
    {
        bn::core::update();
    }

    bn::vector<bn::sprite_ptr, 48> credit_sprites;
    gen.generate(0, 36, "Unofficial fan game.", credit_sprites);
    gen.generate(0, 50, "Original concept © NBS.", credit_sprites);

    bn::sprite_palette_ptr credit_palette = bn::sprite_palette_ptr::create_new(hs::font.item().palette_item());

    for(bn::sprite_ptr& sprite : credit_sprites)
    {
        sprite.set_palette(credit_palette);
    }

    constexpr int credit_fade = 30;

    // 안 보이는 상태 → 보임 → 안 보임 → 보임 → 안 보임 → 보임(유지), 세 번 깜빡인다
    for(int step = 0; step < 5; ++step)
    {
        bool fade_in = step % 2 == 0;

        for(int f = 0; f <= credit_fade; ++f)
        {
            bn::fixed intensity = bn::fixed(fade_in ? credit_fade - f : f) / credit_fade;
            credit_palette.set_fade(bn::color(0, 0, 0), intensity);
            bn::core::update();
        }

        if(fade_in && step < 4)
        {
            for(int f = 0; f < 10; ++f)
            {
                bn::core::update();
            }
        }
    }

    // 세 번째로 켜진 뒤 유지하는 시간 (31프레임 ≈ 0.52초)
    for(int f = 0; f < credit_fade + 1; ++f)
    {
        bn::core::update();
    }

    // 페이드 아웃
    for(int f = 0; f <= SPLASH_FADE; ++f)
    {
        bn::bg_palettes::set_fade(bn::color(0, 0, 0), bn::fixed(f) / SPLASH_FADE);
        bn::sprite_palettes::set_fade(bn::color(0, 0, 0), bn::fixed(f) / SPLASH_FADE);
        bn::core::update();
    }

    text_sprites.clear();
    credit_sprites.clear();

    // ── 2단계: JAY'S LOFT ────────────────────────────────────────────
    // 로고(tools/make_splash_logo.py, 약 50px)를 화면 한가운데에 놓는다
    bn::sprite_ptr studio_logo = bn::sprite_items::splash_logo.create_sprite(0, 0);

    // 페이드 인
    for(int f = 0; f <= SPLASH_FADE; ++f)
    {
        bn::bg_palettes::set_fade(bn::color(0, 0, 0), bn::fixed(SPLASH_FADE - f) / SPLASH_FADE);
        bn::sprite_palettes::set_fade(bn::color(0, 0, 0), bn::fixed(SPLASH_FADE - f) / SPLASH_FADE);
        bn::core::update();
    }

    for(int f = 0; f < SPLASH_HOLD; ++f)
    {
        bn::core::update();
    }

    // 페이드 아웃
    for(int f = 0; f <= SPLASH_FADE; ++f)
    {
        bn::bg_palettes::set_fade(bn::color(0, 0, 0), bn::fixed(f) / SPLASH_FADE);
        bn::sprite_palettes::set_fade(bn::color(0, 0, 0), bn::fixed(f) / SPLASH_FADE);
        bn::core::update();
    }
}

// in이면 검정→화면, 아니면 화면→검정
// fade_music: 페이드와 함께 재생 중인 음악의 볼륨도 줄인다 (페이드 아웃 때만 쓴다)
void fade_screen(bool in, bool fade_music = false)
{
    for(int f = 0; f <= SPLASH_FADE; ++f)
    {
        bn::fixed intensity = bn::fixed(in ? SPLASH_FADE - f : f) / SPLASH_FADE;
        bn::bg_palettes::set_fade(bn::color(0, 0, 0), intensity);
        bn::sprite_palettes::set_fade(bn::color(0, 0, 0), intensity);

        if(fade_music)
        {
            bn::music::set_volume(1 - intensity);
        }

        bn::core::update();
    }
}

// 256x256 그림의 왼쪽 위 240x160이 화면에 오도록 옮긴다
constexpr int title_bg_x = 8;
constexpr int title_bg_y = 48;

enum class title_choice
{
    STORY,
    HARD,
    OPTION,
};

constexpr int menu_count = 3;
constexpr const char* menu_labels[menu_count] = { "STORY", "HARD", "OPTION" };
// 아이콘 셋(tools/make_title_icons.py, 32px)을 한 줄로: 가운데 HARD, 양옆 STORY·OPTION. 이름은 8x8 작은 폰트로 아이콘 바로 아래.
constexpr int menu_icon_x[menu_count] = { -64, 0, 64 };
constexpr int menu_icon_y = 38;
constexpr int menu_label_y = 61;

// hard_unlocked: 스토리를 전부 클리어하면 true (세이브 연동은 나중에)
// seed: 메뉴를 고른 순간의 프레임 수. 강화 난수 시드로 쓴다.
constexpr const char* game_version = "v0.6.3";

title_choice run_title(bool hard_unlocked, unsigned& seed)
{
    bn::sprite_text_generator left_generator(hs::font);
    left_generator.set_left_alignment();

    bn::sprite_text_generator center_generator(hs::font);
    center_generator.set_center_alignment();

    bn::bg_palettes::set_transparent_color(bn::color(4, 3, 3));

    // 타이틀 음악 (assets/audio/title_chiptune.wav → tools/make_music.py가 루프 XM으로 변환). 게임을 시작하면 끈다.
    if(! bn::music::playing())
    {
        bn::music_items::title_chiptune.play(1);        // 옵션 화면에서 돌아올 때는 이어서 들린다
    }

    const bn::regular_bg_item* title_item = &title_item_for_now();
    bn::regular_bg_ptr bg = title_item->create_bg(title_bg_x, title_bg_y);

    auto enabled = [hard_unlocked](int index)
    {
        return index != int(title_choice::HARD) || hard_unlocked;
    };

    // 아이콘: 고른 것만 밝고 위아래로 살짝 흔들리며, 나머지는 어둡다. 잠긴 HARD는 흑백.
    bn::sprite_ptr menu_icons[menu_count] = {
        bn::sprite_items::title_icon_story.create_sprite(menu_icon_x[0], menu_icon_y),
        bn::sprite_items::title_icon_hard.create_sprite(menu_icon_x[1], menu_icon_y),
        bn::sprite_items::title_icon_option.create_sprite(menu_icon_x[2], menu_icon_y),
    };
    bn::sprite_palette_ptr menu_icon_palettes[menu_count] = {
        menu_icons[0].palette(), menu_icons[1].palette(), menu_icons[2].palette()
    };

    if(! hard_unlocked)
    {
        menu_icon_palettes[int(title_choice::HARD)].set_grayscale_intensity(1);
    }

    // 이름: 고른 것은 흰색, 나머지는 회색 (잠긴 HARD는 늘 회색)
    bn::sprite_text_generator label_generator(common::variable_8x8_sprite_font);
    label_generator.set_center_alignment();

    const bn::sprite_palette_item& label_palette_item = common::variable_8x8_sprite_font.item().palette_item();
    bn::sprite_palette_ptr normal_label_palette = label_palette_item.create_palette();
    bn::sprite_palette_ptr locked_palette = bn::sprite_palette_ptr::create_new(label_palette_item);
    locked_palette.set_fade(bn::color(12, 12, 12), bn::fixed(0.6));
    bn::vector<bn::sprite_ptr, 8> menu_label_sprites[menu_count];

    for(int index = 0; index < menu_count; ++index)
    {
        label_generator.generate(menu_icon_x[index], menu_label_y, menu_labels[index], menu_label_sprites[index]);
    }

    text_line notice_text(center_generator, 0, 74);     // 이름 아래, 화면 맨 아래

    // 버전 표시: 왼쪽 아래 구석 (작은 8x8 글꼴)
    bn::sprite_text_generator version_generator(common::variable_8x8_sprite_font);
    version_generator.set_left_alignment();
    bn::vector<bn::sprite_ptr, 8> version_sprites;
    version_generator.generate(-116, 72, game_version, version_sprites);

    int selected = int(title_choice::STORY);
    int notice_frames = 0;
    unsigned frame = 0;
    int styled = -1;     // 아이콘·이름 색을 마지막으로 맞춘 선택 (바뀔 때만 다시 칠한다)

    auto style_menu = [&]()
    {
        for(int index = 0; index < menu_count; ++index)
        {
            bool chosen = index == selected;
            bool locked = ! enabled(index);
            menu_icon_palettes[index].set_fade(bn::color(0, 0, 0), chosen ? bn::fixed(0) : bn::fixed(0.4));
            set_sprites_palette(menu_label_sprites[index], chosen && ! locked ? normal_label_palette : locked_palette);
        }

        styled = selected;
    };

    fade_screen(true);

    while(true)
    {
        ++frame;

        if(bn::keypad::left_pressed())
        {
            selected = (selected + menu_count - 1) % menu_count;
            play_menu_move();
        }
        else if(bn::keypad::right_pressed())
        {
            selected = (selected + 1) % menu_count;
            play_menu_move();
        }

        if(selected != styled)
        {
            style_menu();
        }

        if(bn::keypad::a_pressed() || bn::keypad::start_pressed())
        {
            if(selected == int(title_choice::OPTION))
            {
                play_select();
                fade_screen(false);
                return title_choice::OPTION;        // 옵션 화면은 main()이 연다 (타이틀 스프라이트를 먼저 비우려고)
            }
            else if(! enabled(selected))
            {
                notice_text.set_text("HARD - LOCKED");
                notice_frames = 90;
            }
            else
            {
                play_select();
                seed = frame;
                fade_screen(false, true);
                bn::music::stop();
                return title_choice(selected);
            }
        }

        if(notice_frames > 0 && --notice_frames == 0)
        {
            notice_text.set_text("");
        }

        // 고른 아이콘은 위아래로 살짝 흔들린다
        for(int index = 0; index < menu_count; ++index)
        {
            int bounce = index == selected ? -int(frame / 14) % 2 * 2 : 0;
            menu_icons[index].set_y(menu_icon_y + bounce);
        }

        // 타이틀에 머무는 동안 시간대가 바뀌면 그림도 바꾼다
        if(frame % 60 == 0)
        {
            const bn::regular_bg_item* now_item = &title_item_for_now();

            if(now_item != title_item)
            {
                title_item = now_item;
                bg = title_item->create_bg(title_bg_x, title_bg_y);
            }
        }

        bn::core::update();
    }
}

// ─── 강화 화면 ───────────────────────────────────────────────────────

enum class phase
{
    IDLE,
    HAMMERING,
    SUCCESS,
    FAIL,
    PROTECT_ASK,
    RESTORE,    // 방지권으로 지킨 뒤 금이 사라지고 원래 검으로 돌아온다
    BREAK,      // 검이 두 동강 나 흩어진다
    DOOM,       // +28 -> +29 실패: 성공처럼 빛나다가 멎고 가루가 되어 바스러진다
    SELL,       // 검을 팔면 번쩍이며 사라지고 동전이 쏟아진다 (+1~5 / +6~16 / +17~28)
};

// 검 스프라이트 프레임 (tools/make_swords.py가 한 장에 4개를 만든다)
constexpr int frame_whole = 0;
constexpr int frame_cracked = 1;
constexpr int frame_upper = 2;
constexpr int frame_lower = 3;

// 강화 화면 배경음 (tools/make_bgm.py): +6~16 보통, +17~27 같은 곡을 빠르게, +28 느리고 무거운 곡, 그 밖에는 없음.
// 곡이 바뀌거나 꺼질 때는 서서히 작아지고(페이드 아웃), 켜질 때는 서서히 커진다(페이드 인).
constexpr int bgm_fade_frames = 45;

int forge_bgm_track = -1;       // 지금 재생 중인 곡 (-1 없음)
int forge_bgm_want = -1;        // 레벨이 원하는 곡
bn::fixed forge_bgm_volume = 0;

bn::fixed forge_bgm_target(int track)
{
    return track == 2 ? bn::fixed(0.8) : bn::fixed(0.7);
}

void set_forge_bgm_level(int level)
{
    forge_bgm_want = level >= hs::max_level ? -1 : level >= 28 ? 2 : level >= 17 ? 1 : level >= 6 ? 0 : -1;
}

// 매 프레임 한 번: 원하는 곡이 아니면 줄이고, 줄이다 0이 되면 끄고 새 곡을 0에서 켠다
void tick_forge_bgm()
{
    if(forge_bgm_track >= 0 && ! bn::music::playing())
    {
        forge_bgm_track = -1;
    }

    if(forge_bgm_track != forge_bgm_want && forge_bgm_track >= 0)
    {
        forge_bgm_volume -= forge_bgm_target(forge_bgm_track) / bgm_fade_frames;

        if(forge_bgm_volume <= 0)
        {
            bn::music::stop();
            forge_bgm_track = -1;
            forge_bgm_volume = 0;
        }
        else
        {
            bn::music::set_volume(forge_bgm_volume);
        }
    }
    else if(forge_bgm_track < 0 && forge_bgm_want >= 0)
    {
        forge_bgm_volume = 0;

        switch(forge_bgm_want)
        {

        case 0:
            bn::music_items::bgm_forge_normal.play(0);
            break;

        case 1:
            bn::music_items::bgm_forge_fast.play(0);
            break;

        default:
            bn::music_items::bgm_forge_doom.play(0);
            break;
        }

        forge_bgm_track = forge_bgm_want;
    }
    else if(forge_bgm_track >= 0)
    {
        const bn::fixed target = forge_bgm_target(forge_bgm_track);

        if(forge_bgm_volume < target)
        {
            forge_bgm_volume += target / bgm_fade_frames;

            if(forge_bgm_volume > target)
            {
                forge_bgm_volume = target;
            }

            bn::music::set_volume(forge_bgm_volume);
        }
    }
}

// 연출 중 음악을 즉시 끊고, 다시 켜지지 않게 한다
void cut_forge_bgm()
{
    bn::music::stop();
    forge_bgm_track = -1;
    forge_bgm_want = -1;
    forge_bgm_volume = 0;
}

enum class forge_exit
{
    SHOP,       // L
    FURNACE,    // R (화구: 잡템 조합)
    SWORDS,     // ← (창고)
    BAG,        // ↓ (잡화)
    PAUSE,      // START (일시정지 메뉴)
    GAME_OVER,  // 파산
    CAT,        // 강화 실패가 목표 횟수에 닿아 고양이 이벤트
    RECIPE,     // SELECT 도움말에서 START: 레시피 (낡은 종이에 적힌 단계별 강화표)
    THIEF,      // 밤에 도둑이 들어 창고의 검(없으면 돈)을 훔쳐 간다
    ENDING,     // +29에서 A: 엔딩 컷 (시간대별) + HARD 해금 안내
    MERCHANT,   // 노을에 검을 팔 때 수상한 상인이 나타나 판 금액만큼 더 준다
};

constexpr int hammer_frames = 24;
// 소리는 play() 뒤 약 3프레임(믹서·에뮬레이터 버퍼) 늦게 들린다. 결과를 미리 아는 소리(성공·실패)는 그만큼 앞당겨 틀고,
// 망치질은 소리가 들리는 때에 맞춰 움찔한다. 어긋나면 이 값만 고친다.
constexpr int sfx_latency_frames = 3;
// sfx_hammer의 "땅!" 두 번이 들리는 프레임 (소리의 0초·0.17초)
constexpr int hammer_hit_frames[2] = { sfx_latency_frames, sfx_latency_frames + 10 };
constexpr int success_frames = 24;
constexpr int fail_frames = 24;
constexpr int crack_frame = 12;         // 실패 떨림 도중 금이 가는 프레임
constexpr int restore_frames = 16;      // 방지권으로 지킨 뒤 금이 사라지며 번쩍이는 시간
constexpr int break_frames = 36;        // 검이 부서져 흩어지는 시간
constexpr int shard_frames = 28;        // 파편이 보이는 시간
constexpr int shard_count = 4;

// 강화 성공 연출은 새 단계에 따라 4단계로 커진다: +1~5 기본 / +6~16 파랑 / +17~28 빨강 / +29 마지막 검
constexpr int spark_count = 24;
constexpr int spark_white = 0;      // fx 그림의 프레임 (tools/make_swords.py가 한 장으로 만든다)
constexpr int spark_blue = 1;
constexpr int spark_gold = 2;
constexpr int spark_red = 3;
constexpr int fx_shard = 4;         // 부서지는 검의 파편
constexpr int fx_dust_first = 5;    // 가루 3종: 5, 6, 7
constexpr int success_lengths[4] = { success_frames, 44, 64, 360 };

// 판매 연출 (강화 성공 연출과 같은 3단계 +1~5 / +6~16 / +17~28). 동전은 fx 그림 8~11 (돌아가는 4장).
constexpr int fx_coin_first = 8;
constexpr int coin_count = 32;
constexpr int sell_lengths[3] = { 40, 60, 90 };
constexpr int sell_vanish_frames[3] = { 10, 14, 20 };       // 검이 흐려지며 사라지는 시간
constexpr int coin_floor_y = 66;                            // 동전이 튀는 바닥 (모루 아래)
// 검(그림 53줄)은 화면 y 42부터(성공률 줄 아래), 모루(tools/make_anvil.py, 60x34)는 검 폼멜 바로 아래 y 92부터
constexpr int sword_y = -6;
constexpr int anvil_y = 44;

// 글자 자리 (Butano 좌표, 화면 가운데가 0,0).
// 이름·성공률(·메시지)은 위쪽 가운데, 강화비·판매가는 아래 가운데(잡화 아이콘과 판매 아이콘 사이, 폭 약 72px).
constexpr int name_y = -66;
constexpr int chance_y = -50;
// 칼 왼쪽은 판매가, 오른쪽은 강화비 (이름 줄과 값 줄, 가운데 맞춤). 아래 가운데는 메시지 자리(한 줄 y 60, 두 줄 y 54·66).
constexpr int info_left_x = -52;
constexpr int info_right_x = 52;
constexpr int info_label_y = -16;
constexpr int info_value_y = -2;
constexpr int message_single_y = 56;
constexpr int message_first_y = 45;
constexpr int message_second_y = 57;
constexpr int message_frames = 90;      // 메시지가 이름 자리를 차지하는 시간

// SELECT 도움말: 버튼을 누르고 있는 동안 위쪽 가운데 세 줄에 설명을 보여준다 (줄 폭 150px 이하, 한글 11자 안팎).
// 순서: 강화(A), 판매(B), 상점(L), 화구(R), 창고(←), 잡화(↓). 마지막 줄은 ↑ (시간대 아이콘 설명, 버튼 글자는 없다).
constexpr int help_count = 6;
constexpr const char* help_labels[help_count] = { "강화", "판매", "상점", "화구", "창고", "잡화" };
constexpr const char* help_texts[help_count + 1][3] = {
    { "무기를 강화해 더", "강력한 검에", "도전하자." },
    { "강화 중인 무기를", "판매하여 돈을", "벌자." },
    { "돈을 사용해 강화에", "필요한 아이템을", "구매하자." },
    { "재료를 모아 특별한", "무기와 아이템을", "제작하자." },
    { "보유한 무기와", "아이템을 보관하고", "관리하자." },
    { "강화 실패로 얻은", "재료를 모으고", "관리하자." },
    { "시간에 따라", "강화 확률이", "조정됩니다." },
};

forge_exit run_forge(hs::economy& eco, bn::seed_random& random, const bn::string_view& first_message)
{
    bn::sprite_text_generator left_generator(hs::font);
    left_generator.set_left_alignment();

    bn::sprite_text_generator center_generator(hs::font);
    center_generator.set_center_alignment();

    bn::sprite_text_generator right_generator(hs::font);
    right_generator.set_right_alignment();

    bn::bg_palettes::set_transparent_color(bn::color(4, 3, 3));

    // 배경 그림은 tools/make_forge_bg.py (검·모루는 스프라이트가 올라온다)
    hs::time_band shown_band = hs::current_time_band();
    bn::optional<bn::regular_bg_ptr> background = forge_bg_item(shown_band).create_bg(title_bg_x, title_bg_y);

    // 버튼 아이콘(tools/make_icons.py, 모두 40px)과 아이콘 구석의 키 글자.
    // 아이콘은 z_order를 1로 두어 키 글자가 그 위에 그려지게 한다.
    struct button_layout
    {
        int x;
        int y;
        int size;
    };

    constexpr button_layout shop_button = { -96, -56, 40 };       // 왼쪽 위 (L)
    constexpr button_layout furnace_button = { 96, -56, 40 };     // 오른쪽 위 (R)
    constexpr button_layout swords_button = { -96, 20, 40 };      // 왼쪽 (←) — 강화(A)와 좌우 대칭
    constexpr button_layout bag_button = { -56, 48, 40 };         // 왼쪽 아래 (↓) — 판매(B)와 좌우 대칭
    constexpr button_layout sell_button = { 56, 48, 40 };         // 오른쪽 아래 (B)
    constexpr button_layout upgrade_button = { 96, 20, 40 };      // 오른쪽 (A)

    auto create_icon = [](const bn::sprite_item& item, const button_layout& layout)
    {
        bn::sprite_ptr icon = item.create_sprite(layout.x, layout.y);
        icon.set_z_order(1);
        return icon;
    };

    // 키 글자는 아이콘의 네 구석 중 정해 둔 곳에 둔다. 오른쪽 아래의 소지금·판매가 글자와 겹치지 않도록
    // 오른쪽 아이콘은 왼쪽(화구는 아래, 강화·판매는 위), 왼쪽 아이콘은 오른쪽(상점은 아래, 창고·잡화는 위)에 둔다.
    enum class badge_corner { LEFT_TOP, LEFT_BOTTOM, RIGHT_TOP, RIGHT_BOTTOM };

    auto generate_badge = [&](const button_layout& layout, badge_corner corner, const char* key,
                              bn::ivector<bn::sprite_ptr>& sprites)
    {
        int half = layout.size / 2;
        bool right = corner == badge_corner::RIGHT_TOP || corner == badge_corner::RIGHT_BOTTOM;
        bool bottom = corner == badge_corner::LEFT_BOTTOM || corner == badge_corner::RIGHT_BOTTOM;
        int badge_x = right ? layout.x + half - 4 : layout.x - half + 4;
        int badge_y = bottom ? layout.y + half - 5 : layout.y - half + 5;
        center_generator.generate(badge_x, badge_y, key, sprites);
    };

    bn::sprite_ptr shop_icon = create_icon(bn::sprite_items::icon_shop, shop_button);
    bn::sprite_ptr furnace_icon = create_icon(bn::sprite_items::icon_furnace, furnace_button);
    bn::sprite_ptr swords_icon = create_icon(bn::sprite_items::icon_swords, swords_button);
    bn::sprite_ptr bag_icon = create_icon(bn::sprite_items::icon_bag, bag_button);
    bn::sprite_ptr sell_icon = create_icon(bn::sprite_items::icon_sell, sell_button);
    bn::sprite_ptr upgrade_icon = create_icon(bn::sprite_items::icon_upgrade, upgrade_button);

    bn::vector<bn::sprite_ptr, 8> label_sprites;
    generate_badge(swords_button, badge_corner::RIGHT_TOP, "←", label_sprites);
    generate_badge(bag_button, badge_corner::RIGHT_TOP, "↓", label_sprites);

    // 판매·강화는 못 쓸 때 아이콘과 키 글자를 회색으로 바꾼다
    bn::vector<bn::sprite_ptr, 1> sell_badge;
    generate_badge(sell_button, badge_corner::LEFT_TOP, "B", sell_badge);

    bn::vector<bn::sprite_ptr, 1> upgrade_badge;
    generate_badge(upgrade_button, badge_corner::LEFT_TOP, "A", upgrade_badge);

    // 상점·화구는 +0 검일 때만 쓸 수 있다
    bn::vector<bn::sprite_ptr, 1> shop_badge;
    generate_badge(shop_button, badge_corner::RIGHT_BOTTOM, "L", shop_badge);

    bn::vector<bn::sprite_ptr, 1> furnace_badge;
    generate_badge(furnace_button, badge_corner::LEFT_BOTTOM, "R", furnace_badge);

    bn::sprite_palette_ptr normal_palette = sell_badge[0].palette();
    // 못 쓰는 버튼 글자와 "+0" 단계 글자는 같은 회색 팔레트 하나를 쓴다 (팔레트 한도 때문)
    bn::sprite_palette_ptr gray_tint = create_tinted_font_palette(bn::color(16, 16, 16));
    bn::sprite_palette_ptr& locked_palette = gray_tint;
    bn::sprite_palette_ptr sell_icon_palette = sell_icon.palette();
    bn::sprite_palette_ptr upgrade_icon_palette = upgrade_icon.palette();
    bn::sprite_palette_ptr shop_icon_palette = shop_icon.palette();
    bn::sprite_palette_ptr furnace_icon_palette = furnace_icon.palette();

    auto set_button_enabled = [&](bn::sprite_palette_ptr& icon_palette, bn::ivector<bn::sprite_ptr>& badge,
                                  bool enabled)
    {
        icon_palette.set_grayscale_intensity(enabled ? 0 : 1);
        icon_palette.set_fade(bn::color(4, 3, 3), enabled ? 0 : bn::fixed(0.45));
        set_sprites_palette(badge, enabled ? normal_palette : locked_palette);
    };

    // 위쪽 가운데는 이름과 성공률만 나온다
    text_line name_prefix_text(left_generator, 0, name_y);  // 이름의 "+N" (단계별 색)
    text_line name_rest_text(left_generator, 0, name_y);    // 이름의 나머지
    text_line chance_text(center_generator, 0, chance_y);

    // 칼 왼쪽 = 판매가, 오른쪽 = 강화비. 재료로 내는 강화비(+21~+25, +27)는 값 줄이 "아이콘 + x개수"가 된다.
    text_line sell_label(center_generator, info_left_x, info_label_y);
    text_line sell_value(center_generator, info_left_x, info_value_y);
    text_line cost_label(center_generator, info_right_x, info_label_y);
    text_line cost_value(center_generator, info_right_x, info_value_y);
    bn::optional<bn::sprite_ptr> cost_icon;
    text_line cost_qty_text(left_generator, 0, info_value_y);
    // 재료 강화비: "필요 / [아이콘] x개수 / 이름"처럼 아이콘 아래에 작은 글씨로 무엇인지 적는다
    bn::sprite_text_generator small_center_generator(hs::font_small);
    small_center_generator.set_center_alignment();
    text_line cost_name_text(small_center_generator, info_right_x, info_value_y + 15);
#ifdef HS_DEBUG
    // 디버그: 지금 쓰는 스프라이트 타일 수와 그동안의 최대값을 맨 위 왼쪽에 보여 준다 (VRAM 1024칸 중)
    bn::sprite_text_generator vram_generator(hs::font_small);
    vram_generator.set_left_alignment();
    text_line vram_text(vram_generator, -118, -76);
    int vram_peak = 0;
    int vram_frame = 0;
#endif

    // 아래 가운데 메시지 (한 줄 또는 두 줄). 모두 검은 외곽선이 있는 폰트(hs::font_outline)로 쓴다.
    bn::sprite_text_generator outline_generator(hs::font_outline);
    outline_generator.set_center_alignment();
    text_line message_line_1(outline_generator, 0, message_single_y);
    text_line message_line_2(outline_generator, 0, message_second_y);
    // 소지금: 외곽선 폰트에 노랑 몸통 + 주황 외곽선
    bn::sprite_text_generator money_generator(hs::font_outline);
    money_generator.set_right_alignment();
    text_line money_text(money_generator, 114, 72);
    bn::sprite_palette_ptr money_palette = create_two_tone_font_palette(gold_body_color, gold_outline_color);
    money_text.set_sticky_palette(money_palette);

    // 최고기록: 왼쪽 아래 "★ +N 이름" (지금까지 도달한 가장 높은 단계, 단계 색은 이름과 같다)
    bn::sprite_text_generator best_generator(hs::font_small);
    best_generator.set_left_alignment();
    text_line best_text(best_generator, -114, 72);          // "H +N" (단계 색)
    text_line best_name_text(best_generator, -114, 72);     // 검 이름 (흰색)
    int shown_best = eco.best_level();      // 망치질·연출 중에는 결과가 새어 나가지 않게 이전 값을 유지한다

    // 메시지는 아래 가운데에 message_frames 동안 보였다가 사라진다 (0 = 없음).
    // 방지권 질문(PROTECT_ASK)은 답할 때까지(-1) 유지한다.
    int message_timer = 0;
    int name_level = 0;

    // "엔딩으로.." 글자: 이미 있는 팔레트(금색+주황 외곽선, 빨강, 파랑)를 돌려 쓰며 흔들린다.
    // (강화 화면은 팔레트가 16개 한도에 아슬아슬해서 새 팔레트를 만들지 않는다)
    bool ending_shake = false;
    int ending_frame = 0;

    // 단계 표시 "+N" 색: +0 회색 / +1~5 흰색 / +6~16 파랑 / +17~29 빨강
    bn::sprite_palette_ptr blue_tint = create_tinted_font_palette(bn::color(8, 17, 31));
    bn::sprite_palette_ptr red_tint = create_tinted_font_palette(bn::color(31, 7, 7));
    const bn::sprite_palette_ptr* name_tint = nullptr;

    // "+N 이름"을 가운데에 맞춰 두 조각으로 쓴다 ("+N"만 색을 입힌다)
    auto set_name = [&]()
    {
        bn::string<8> prefix("+");
        prefix += bn::to_string<4>(name_level);
        const char* rest = hs::sword_names[name_level];
        int prefix_width = left_generator.width(prefix + " ");
        int start = -(prefix_width + left_generator.width(rest)) / 2;

        name_prefix_text.set_x(start);
        name_prefix_text.set_text(prefix);
        name_rest_text.set_x(start + prefix_width);
        name_rest_text.set_text(rest);

        if(name_tint)
        {
            name_prefix_text.set_palette(*name_tint);
        }
    };

    auto clear_message = [&]()
    {
        message_line_1.set_text("");
        message_line_2.set_text("");
        message_timer = 0;
        ending_shake = false;
    };

    // 아래 가운데 메시지. text에 '\n'이 있으면 두 줄. tint가 있으면 그 색(강화 성공! 파랑, 강화 실패.. 빨강)으로 쓴다.
    // 한 줄은 약 70px 안에 들어가야 한다 (양옆에 아이콘이 있다).
    auto show_message = [&](const bn::string_view& text, const bn::sprite_palette_ptr* tint)
    {
        clear_message();

        if(text.empty())
        {
            return;
        }

        bn::string<48> first;
        bn::string<48> second;
        bool in_second = false;

        for(char c : text)
        {
            if(c == '\n')
            {
                in_second = true;
            }
            else if(in_second)
            {
                second.push_back(c);
            }
            else
            {
                first.push_back(c);
            }
        }

        message_line_1.set_y(in_second ? message_first_y : message_single_y);
        message_line_1.set_text(first);

        if(tint)
        {
            message_line_1.set_palette(*tint);
        }

        if(in_second)
        {
            message_line_2.set_text(second);

            if(tint)
            {
                message_line_2.set_palette(*tint);
            }
        }

        message_timer = message_frames;
    };

    auto message_text = [&](const bn::string_view& text)
    {
        show_message(text, nullptr);
    };

    // 모루는 z_order 1로 검 뒤에 둔다 (폼멜이 모루 윗면에 1px 겹친다)
    bn::sprite_ptr anvil = bn::sprite_items::anvil.create_sprite(0, anvil_y);
    anvil.set_z_order(1);
    int sword_level = eco.level();
    bn::sprite_ptr sword = sword_items[sword_level]->create_sprite(0, sword_y);
    bn::sprite_palette_ptr sword_palette = sword.palette();

    // 대기 모션: 검이 모루 위에서 위아래로 둥실거린다. 높은 단계일수록 크고 빠르며, +17 이상은 날에 가끔 반짝임이 인다.
    int idle_tick = 0;
    int idle_glint_dy = 0;                          // 반짝임이 검 가운데보다 얼마나 위인지
    bn::optional<bn::sprite_ptr> idle_glint;        // 반짝임 (fx 팔레트를 잠깐만 쓰도록 반짝이는 동안만 만든다)

    bool ending_pending = false;        // +29에서 A를 눌러 "엔딩으로.." 메시지가 끝나면 엔딩 화면으로 간다

    // 도둑 이벤트: 밤에 강화·판매를 마칠 때마다 한 번 판정하고, 걸리면 결과 메시지가 끝난 뒤(IDLE) 도둑이 든다
    bool thief_pending = false;
    auto roll_thief = [&]()
    {
        if(hs::current_time_band() == hs::time_band::NIGHT)
        {
            eco.note_night_action();

            if(eco.thief_ready() && random.get_int(100) < hs::thief_chance_pct)
            {
                thief_pending = true;
            }
        }
    };

    // 수상한 상인 이벤트: 노을에 검을 팔 때마다 한 번 판정하고, 걸리면 결과 메시지가 끝난 뒤(IDLE) 상인이 나타난다
    bool merchant_pending = false;
    auto roll_merchant = [&]()
    {
        if(hs::current_time_band() == hs::time_band::SUNSET)
        {
            eco.note_sunset_sale();

            if(eco.merchant_ready() && random.get_int(100) < hs::merchant_chance_pct)
            {
                merchant_pending = true;
            }
        }
    };

    phase current_phase = phase::IDLE;
    int phase_frame = 0;
    bool upgrade_succeeded = false;     // 판정은 바로 하고, 결과는 망치질이 끝난 뒤 보여준다
    bool reveal_hold = false;           // +29 성공: 실패 낚시 연출과 구분이 안 되도록 이름·검 그림·메시지를 폭발 순간까지 숨긴다
#ifdef HS_DEBUG
    int debug_force = 0;                    // 0 끔 / 1 다음 강화 무조건 실패 / 2 무조건 성공 (+1 이상에서 R로 돌린다)
    int debug_jump_index = 0;               // +1 이상에서 L: 단계를 5 → 16 → 28로 옮긴다
#endif

    // SELECT 도움말: 배경·검·모루는 흑백, 아이콘은 컬러로 두고 아이콘 가운데에 이름을 쓴다.
    // 버튼을 누르고 있는 동안만 위쪽 가운데에 설명이 나온다.
    bool help_mode = false;
    int help_key = -1;
    bn::optional<bn::bg_palette_ptr> background_palette = background->palette();
    bn::sprite_palette_ptr anvil_palette = anvil.palette();
    text_line help_line_1(center_generator, 0, name_y);
    text_line help_line_2(center_generator, 0, chance_y);
    text_line help_line_3(center_generator, 0, chance_y + 16);
    bn::vector<bn::sprite_ptr, 16> help_label_sprites;
    bn::optional<bn::sprite_ptr> recipe_icon;       // 도움말에서만 보이는 레시피 버튼 (잡화 ↓ 와 판매 B 사이)
    // 도움말을 열고 닫을 때 글자를 지우는 것과 새로 만드는 것을 같은 프레임에 하면, 지운 타일은 한 프레임 뒤에야 돌아와서 VRAM이 모자라 멈춘다.
    // 그래서 열 때는 상태 글자를 지운 뒤 두 프레임 뒤에 도움말 글자를 만들고, 닫을 때는 도움말 글자를 지운 뒤 두 프레임 뒤에 상태를 다시 그린다.
    int help_labels_delay = 0;
    int status_refresh_delay = 0;
#ifdef HS_DEBUG
    const char* debug_band_name = nullptr;
#endif
    const button_layout* help_buttons[help_count] = {
        &upgrade_button, &sell_button, &shop_button, &furnace_button, &swords_button, &bag_button
    };

    // 성공률 줄: 항상 "성공률 N%" (메시지는 아래 가운데에 나온다)
    auto show_chance = [&]()
    {
        bn::string<32> chance_line;

        if(eco.at_max_level() && ! reveal_hold)
        {
            chance_line = "최고 단계";
        }
        else
        {
            // +29 성공 연출 중에는 아직 +28이었던 것처럼 이전 단계의 성공률을 보여 준다
            int pct = eco.success_pct();

            if(reveal_hold)
            {
                pct = hs::level_table(eco.diff())[sword_level].success_pct - eco.time_penalty();
                pct = pct < 0 ? 0 : pct;
            }

            chance_line = "성공률 ";
            chance_line += bn::to_string<4>(pct);
            chance_line += "%";
        }

        chance_text.set_text(chance_line);
    };

    auto refresh_status = [&]()
    {
        // 망치질 중에는 결과를 아직 보여주지 않는다: 이름·강화비·판매가는 강화 전 단계 그대로
        const int shown_level = current_phase == phase::HAMMERING || reveal_hold ? sword_level : eco.level();
        const hs::level_row& row = hs::level_table(eco.diff())[shown_level];

        // 칼 오른쪽: 강화비. 돈이면 "$5", 재료면 "아이콘 + x개수"
        cost_label.set_text("강화비");
        cost_icon.reset();
        cost_qty_text.set_text("");
        cost_name_text.set_text("");

        if(shown_level == hs::max_level)
        {
            cost_value.set_text("-");
        }
        else if(row.cost_sword > 0 || row.cost_material != hs::material::NONE)
        {
            const bn::sprite_item& icon_item = row.cost_sword > 0 ? *sword_icons[row.cost_sword]
                                                                  : *material_icons[int(row.cost_material)];
            bn::string<16> quantity_line("x");
            quantity_line += bn::to_string<4>(row.cost_qty);

            const int total_width = 16 + 3 + left_generator.width(quantity_line);
            const int start = info_right_x - total_width / 2;
            cost_icon = icon_item.create_sprite(start + 8, info_value_y);
            cost_icon->set_visible(! help_mode);
            cost_qty_text.set_x(start + 19);
            cost_qty_text.set_text(quantity_line);
            cost_value.set_text("");

            // 라벨은 "필요", 아이콘 아래에 이름: 검은 "+19 검"(단계 색), 잡템은 이름
            cost_label.set_text("필요");

            if(row.cost_sword > 0)
            {
                bn::string<16> sword_name("+");
                sword_name += bn::to_string<4>(row.cost_sword);
                sword_name += " 검";
                cost_name_text.set_text(sword_name);
                cost_name_text.set_palette(row.cost_sword <= 5 ? normal_palette : row.cost_sword <= 16 ? blue_tint : red_tint);
            }
            else
            {
                cost_name_text.set_text(hs::material_names[int(row.cost_material)]);
            }
        }
        else
        {
            cost_value.set_text(format_money_short(row.cost_usd));
        }

        // 칼 왼쪽: 판매가
        sell_label.set_text("판매가");
        sell_value.set_text(row.sell_usd < 0 || shown_level == hs::max_level ? bn::string<24>("-")
                                                                             : format_money_short(row.sell_usd));

        name_level = shown_level;
        name_tint = shown_level == 0 ? &gray_tint : shown_level <= 5 ? nullptr :
                    shown_level <= 16 ? &blue_tint : &red_tint;

        set_name();
        show_chance();

        money_text.set_text(format_money(eco.money()));

        if(current_phase != phase::HAMMERING && ! reveal_hold)
        {
            shown_best = eco.best_level();
        }

        // "H +N"만 단계 색, 검 이름은 흰색. 소지금과 겹칠 만큼 길면 이름을 뺀다.
        bn::string<40> best_prefix = sword_best_label(shown_best, false);
        bn::string<40> best_name(hs::sword_names[shown_best]);
        const int prefix_width = best_generator.width(best_prefix + " ");
        const bool show_name = -114 + prefix_width + best_generator.width(best_name) + 8 <=
                               114 - money_generator.width(format_money(eco.money()));

        best_text.set_text(best_prefix);
        best_text.set_palette(shown_best == 0 ? gray_tint : shown_best <= 5 ? normal_palette :
                              shown_best <= 16 ? blue_tint : red_tint);
        best_name_text.set_x(-114 + prefix_width);
        best_name_text.set_text(show_name ? best_name : bn::string<40>());
        best_name_text.set_palette(normal_palette);

        bool idle = current_phase == phase::IDLE;
        set_button_enabled(shop_icon_palette, shop_badge, eco.level() == 0);
        set_button_enabled(furnace_icon_palette, furnace_badge, eco.level() == 0);
        set_button_enabled(sell_icon_palette, sell_badge, idle && eco.can_sell());
        set_button_enabled(upgrade_icon_palette, upgrade_badge,
                           idle && ! eco.at_max_level() && eco.can_afford_upgrade());

        // 검 그림은 단계가 바뀔 때 새로 만든다. 망치질 중에는 결과를 아직 보여주지 않는다.
        if(current_phase != phase::HAMMERING && ! reveal_hold && eco.level() != sword_level)
        {
            idle_glint.reset();         // 팔레트 한도: 새 검 팔레트가 잠깐 하나 더 필요하다
            sword_level = eco.level();
            sword = sword_items[sword_level]->create_sprite(0, sword_y);
            sword.set_z_order(0);
            sword_palette = sword.palette();
        }
    };

    // 도움말은 배경을 흑백으로 만든다. 배경 팔레트의 65번 이후 색은 시간대 아이콘 전용(tools/make_forge_bg_time.py)이라
    // 앞쪽 65색만 회색으로 바꾸고 아이콘 색은 그대로 둔다: 스프라이트(VRAM)를 따로 쓰지 않고 아이콘이 컬러로 보인다.
    constexpr int bg_gray_colors = 65;

    auto set_background_gray = [&](bool gray)
    {
        if(gray)
        {
            const bn::span<const bn::color>& source = forge_bg_item(shown_band).palette_item().colors_ref();

            for(int index = 1; index < bg_gray_colors && index < source.size(); ++index)
            {
                const bn::color& color = source[index];
                const int value = (color.red() * 77 + color.green() * 150 + color.blue() * 29) / 256;
                background_palette->set_color(index, bn::color(value, value, value));
            }
        }
        else
        {
            background_palette->set_colors(forge_bg_item(shown_band).palette_item());
        }
    };

    auto build_help_labels = [&]()
    {
        for(int index = 0; index < help_count; ++index)
        {
            center_generator.generate(help_buttons[index]->x, help_buttons[index]->y, help_labels[index],
                                      help_label_sprites);
        }

        // 레시피 버튼: START로 연다. 키 글자는 잡화(↓)·판매(B)와 같은 줄(y 33)
        recipe_icon = bn::sprite_items::icon_recipe.create_sprite(0, 48);
        recipe_icon->set_z_order(1);
        center_generator.generate(0, 48, "레시피", help_label_sprites);
        center_generator.generate(0, 33, "START", help_label_sprites);

        center_generator.generate(-89, -25, "↑", help_label_sprites);        // 시간대 아이콘 옆의 키 표시 (←·↓ 처럼)
    };

    auto set_help = [&](bool on)
    {
        idle_glint.reset();
        help_mode = on;
        help_key = -1;
        help_labels_delay = 0;
        status_refresh_delay = 0;
        help_line_1.set_text("");
        help_line_2.set_text("");
        help_line_3.set_text("");
        help_label_sprites.clear();
        recipe_icon.reset();

        bn::fixed gray = on ? 1 : 0;
        set_background_gray(on);
        anvil_palette.set_grayscale_intensity(gray);
        sword_palette.set_grayscale_intensity(gray);
        sword.set_visible(! on);        // 설명 셋째 줄이 검 끝과 겹치므로 도움말 동안은 검을 숨긴다

        name_prefix_text.set_visible(! on);
        name_rest_text.set_visible(! on);
        chance_text.set_visible(! on);
        cost_label.set_visible(! on);
        cost_value.set_visible(! on);
        cost_qty_text.set_visible(! on);
        cost_name_text.set_visible(! on);
        message_line_1.set_visible(! on);
        message_line_2.set_visible(! on);

        if(cost_icon)
        {
            cost_icon->set_visible(! on);
        }

        sell_label.set_visible(! on);
        sell_value.set_visible(! on);
        money_text.set_visible(! on);
        best_text.set_visible(! on);
        best_name_text.set_visible(! on);

        if(on)
        {
            // 숨겨 두는 상태 글자도 타일을 차지하니 지워서 VRAM을 비운다 (닫을 때 refresh_status가 다시 그린다)
            name_prefix_text.set_text("");
            name_rest_text.set_text("");
            chance_text.set_text("");
            cost_label.set_text("");
            cost_value.set_text("");
            cost_qty_text.set_text("");
            cost_name_text.set_text("");
            message_line_1.set_text("");
            message_line_2.set_text("");
            sell_label.set_text("");
            sell_value.set_text("");
            money_text.set_text("");
            best_text.set_text("");
            best_name_text.set_text("");
            help_labels_delay = 2;

            // 판매·강화는 못 쓰는 상태여도 도움말에서는 컬러로 보여준다
            set_button_enabled(sell_icon_palette, sell_badge, true);
            set_button_enabled(upgrade_icon_palette, upgrade_badge, true);
            set_button_enabled(shop_icon_palette, shop_badge, true);
            set_button_enabled(furnace_icon_palette, furnace_badge, true);
        }
        else
        {
            status_refresh_delay = 2;
        }
    };

    auto destroy_sword = [&]()
    {
        hs::drop dropped = eco.destroy(random);

        if(dropped.quantity > 0)
        {
            bn::string<48> drop_line("파괴!\n");
            drop_line += hs::material_names[int(dropped.item)];
            drop_line += " x";
            drop_line += bn::to_string<4>(dropped.quantity);
            message_text(drop_line);
        }
        else if(message_timer < 0)
        {
            clear_message();    // 방지권 질문이 남아 있으면 지운다 (잡템이 없으면 따로 메시지를 쓰지 않는다)
        }

        current_phase = phase::IDLE;
        refresh_status();
    };

    eco.set_time_penalty(hs::time_penalty_pct(hs::current_time_band()));
    // 검이 부서질 때: 두 동강 + 파편 몇 개 (조각 그림은 검 스프라이트의 프레임 2·3, 파편은 shard)
    bn::optional<bn::sprite_ptr> piece_lower;
    bn::vector<bn::sprite_ptr, shard_count> shards;
    bn::fixed_point upper_position, lower_position, upper_velocity, lower_velocity;
    bn::fixed_point shard_position[shard_count];
    bn::fixed_point shard_velocity[shard_count];

    auto start_break = [&]()
    {
        current_phase = phase::BREAK;
        phase_frame = 0;

        const bn::sprite_item& item = *sword_items[sword_level];
        sword.set_x(0);
        sword_palette.set_fade(bn::color(31, 31, 31), 0);

        upper_position = bn::fixed_point(0, sword_y);
        lower_position = bn::fixed_point(0, sword_y);
        upper_velocity = bn::fixed_point(-1.2, -2.2);      // 위쪽 조각은 왼쪽 위로 튀고
        lower_velocity = bn::fixed_point(0.8, -0.8);       // 아래쪽 조각은 살짝 오른쪽으로 떨어진다
        sword.set_item(item, frame_upper);      // 위쪽 조각은 검 스프라이트를 그대로 써서 타일을 아낀다
        sword.set_position(upper_position);
        piece_lower = item.create_sprite(lower_position, frame_lower);

        // 파편은 금 간 자리(검 한가운데 조금 위)에서 사방으로
        constexpr bn::fixed_point crack_center(0, sword_y - 4);
        constexpr bn::fixed_point shard_start_velocity[shard_count] = {
            bn::fixed_point(-1.6, -2.4), bn::fixed_point(1.5, -2.8), bn::fixed_point(-0.7, -1.5), bn::fixed_point(1.9, -1.3)
        };

        shards.clear();

        for(int index = 0; index < shard_count; ++index)
        {
            shard_position[index] = crack_center;
            shard_velocity[index] = shard_start_velocity[index];
            shards.push_back(bn::sprite_items::fx.create_sprite(crack_center, fx_shard));
        }
    };

    // 판매 연출: 동전 (coin_count개). 위로 튀었다가 떨어져 바닥에서 몇 번 튕기고 깜빡이며 사라진다.
    struct coin_state
    {
        bn::optional<bn::sprite_ptr> sprite;
        bn::fixed_point position;
        bn::fixed_point velocity;
        bn::fixed floor;
        int delay = 0;
        int age = 0;
        int settled = 0;
        bool active = false;
    };

    coin_state coins[coin_count];
    int sell_tier = 0;
    int sell_money_before = 0;
    int sell_amount = 0;

    // 강화 성공 연출 (success_lengths 4단계). 별(spark)은 최대 24개, 색은 spark 그림의 프레임으로 고른다.
    bn::optional<bn::sprite_ptr> sparks[spark_count];
    bn::fixed_point spark_position[spark_count];
    bn::fixed_point spark_velocity[spark_count];
    int spark_life[spark_count] = {};
    int success_tier = 0;
    int success_length = success_frames;
    bn::fixed flash_level = 0;      // 화면 전체 하얀 번쩍임 세기 (매 프레임 줄어든다)

    // first부터 count개 별을 검 한가운데에서 사방으로 쏜다. up_bias는 위로 뜨는 정도(불씨 느낌).
    // 별 색은 color_a, color_b(spark_white 등)를 번갈아 쓴다 (같으면 한 색).
    auto spawn_sparks = [&](int first, int count, bn::fixed speed, bn::fixed up_bias, int life,
                            int color_a, int color_b)
    {
        for(int n = 0; n < count; ++n)
        {
            int index = first + n;
            bn::fixed angle = bn::fixed(360 * n) / count + (first * 7) % 30;

            if(angle >= 360)
            {
                angle -= 360;
            }

            spark_position[index] = bn::fixed_point(0, sword_y - 8);
            spark_velocity[index] = bn::fixed_point(speed * bn::degrees_lut_cos(angle),
                                                    speed * bn::degrees_lut_sin(angle) - up_bias);
            spark_life[index] = life;
            sparks[index] = bn::sprite_items::fx.create_sprite(spark_position[index], n % 2 == 0 ? color_a : color_b);
        }
    };

    auto update_sparks = [&]()
    {
        for(int index = 0; index < spark_count; ++index)
        {
            if(spark_life[index] > 0)
            {
                spark_velocity[index] *= bn::fixed(0.96);
                spark_position[index] += spark_velocity[index];
                sparks[index]->set_position(spark_position[index]);
                --spark_life[index];
                sparks[index]->set_visible(spark_life[index] > 10 || spark_life[index] % 2 == 0);     // 끝나갈 땐 깜빡

                if(spark_life[index] == 0)
                {
                    sparks[index].reset();
                }
            }
        }
    };

    auto apply_flash = [&]()
    {
        bn::bg_palettes::set_fade(bn::color(31, 31, 31), flash_level);
        bn::sprite_palettes::set_fade(bn::color(31, 31, 31), flash_level);
    };

    auto end_success = [&]()
    {
        reveal_hold = false;
        flash_level = 0;
        bn::bg_palettes::set_fade(bn::color(0, 0, 0), 0);
        bn::sprite_palettes::set_fade(bn::color(0, 0, 0), 0);

        for(int index = 0; index < spark_count; ++index)
        {
            spark_life[index] = 0;
            sparks[index].reset();
        }

        sword.set_position(0, sword_y);
        sword.remove_affine_mat();
        sword_palette.set_fade(bn::color(31, 31, 31), 0);
        current_phase = phase::IDLE;
        refresh_status();
    };

    // ── +29 성공 연출과 +28 실패 "낚시" 연출이 같이 쓰는 앞부분 ──
    // 0~222프레임: 떨림이 커지고 검이 떠오르며, 별 고리와 번쩍임이 쌓이다 210프레임에 가장 눈부시게 터진다.
    // 여기까지는 성공과 실패가 똑같이 보인다. 이후 성공은 황금빛이 내려앉고, 실패는 빛이 꺼지고 바스러진다.
    constexpr int grand_peak = 210;
    constexpr int grand_buildup_end = 222;

    auto grand_buildup = [&](int t, bn::fixed& offset_x, bn::fixed& offset_y, bn::fixed& fade,
                             bn::color& fade_color, bn::fixed& scale)
    {
        const bn::color gold(31, 25, 4);
        bn::fixed amplitude = t < 200 ? bn::fixed(t) / 18 : bn::fixed(11);
        bn::fixed rise = bn::fixed(t < 140 ? t : 140) / 10;
        offset_x = t % 2 == 0 ? amplitude : -amplitude;
        offset_y = -rise + ((t / 2) % 2 == 0 ? amplitude / 3 : -amplitude / 3);

        // 별 고리: 30프레임마다 두 묶음이 번갈아 점점 빠르게 퍼진다
        if(t >= 20 && t < 190 && t % 30 == 20)
        {
            const int ring = (t - 20) / 30;
            spawn_sparks(ring % 2 == 0 ? 0 : 12, 12, bn::fixed(1.2) + bn::fixed(ring) * bn::fixed(0.25), bn::fixed(0.1),
                         58, spark_gold, spark_red);
        }

        if(t == 60)
        {
            flash_level = bn::fixed(0.35);
        }

        if(t == 100)
        {
            flash_level = bn::fixed(0.5);
        }

        if(t == 140)
        {
            flash_level = bn::fixed(0.6);
        }

        if(t == 180)
        {
            flash_level = bn::fixed(0.7);
        }

        if(t == grand_peak)
        {
            flash_level = 1;        // 눈부신 순간 + 별이 한꺼번에
            spawn_sparks(0, spark_count, 3.0, bn::fixed(0.4), 40, spark_gold, spark_red);
        }

        // 검 빛: 금빛으로 깜빡이다가 210 부근에서 하얗게 달아오른다
        if(t < 200)
        {
            fade = (t / 8) % 2 == 0 ? bn::fixed(0.3) + bn::fixed(t) / 300 : bn::fixed(0);
            fade_color = gold;
        }
        else
        {
            fade = bn::fixed(0.9) - bn::fixed(t - 200) / 70;
            fade_color = bn::color(31, 31, 28);
        }

        if(t >= 140)
        {
            scale = 1 + bn::fixed(t < grand_peak ? t - 140 : 70) * bn::fixed(0.006);
        }
    };

    // 위에서 천천히 내리는 별 한 개 (12~23번 칸 중 빈 곳)
    auto emit_rain = [&]()
    {
        for(int index = 12; index < spark_count; ++index)
        {
            if(spark_life[index] == 0)
            {
                const int pick = random.get_int(3);
                spark_position[index] = bn::fixed_point(random.get_int(81) - 40, sword_y - 44 + random.get_int(10));
                spark_velocity[index] = bn::fixed_point(bn::fixed(random.get_int(7) - 3) / 10, bn::fixed(1.0) + bn::fixed(random.get_int(5)) / 10);
                spark_life[index] = 45;
                sparks[index] = bn::sprite_items::fx.create_sprite(spark_position[index],
                                                                      pick == 0 ? spark_gold : pick == 1 ? spark_red : spark_white);
                break;
            }
        }
    };

    // 한 프레임치 연출: phase_frame은 SUCCESS가 시작된 뒤 지난 프레임
    auto run_success_effect = [&]()
    {
        const int t = phase_frame;
        bn::fixed fade = t < success_frames ? bn::fixed(success_frames - t) / success_frames : bn::fixed(0);
        bn::color fade_color(31, 31, 31);
        bn::fixed offset_x = 0;
        bn::fixed offset_y = t < success_frames ? -bn::fixed(t < success_frames / 2 ? t / 2 : (success_frames - t) / 2) : bn::fixed(0);
        bn::fixed scale = 1;

        if(success_tier == 1)
        {
            // +6~16 (파랑): 별이 퍼지고 번쩍임이 한 번 더
            if(t == 1)
            {
                spawn_sparks(0, 8, 1.6, 0, 40, spark_white, spark_white);
            }

            if(t >= 24 && t < 40)
            {
                fade = bn::fixed(40 - t) / 16 * bn::fixed(0.8);
                fade_color = bn::color(14, 22, 31);
            }
        }
        else if(success_tier == 2)
        {
            // +17~28 (빨강): 별·불씨가 크게 터지고 검이 부풀었다 가라앉는다 (화면 번쩍임은 마지막 +29에서만)
            if(t == 1)
            {
                spawn_sparks(0, 12, 2.0, bn::fixed(0.5), 48, spark_blue, spark_blue);
            }

            if(t == 22)
            {
                spawn_sparks(12, 10, 1.3, bn::fixed(0.3), 36, spark_blue, spark_blue);
            }

            if(t < 20)
            {
                scale = 1 + bn::fixed(t < 10 ? t : 20 - t) * bn::fixed(0.03);
            }

            if(t >= 24 && t < 56)
            {
                fade = bn::fixed(56 - t) / 32 * bn::fixed(0.7);
                fade_color = bn::color(31, 10, 6);
            }
        }
        else if(success_tier == 3)
        {
            // +29 (마지막 검): 쌓이던 연출이 210프레임에 터지고, 황금빛 별이 비처럼 내리며 천천히 가라앉는다
            if(t < grand_buildup_end)
            {
                grand_buildup(t, offset_x, offset_y, fade, fade_color, scale);

                if(t == 1)
                {
                    bn::sound_items::sfx_glow.play();       // 실패 낚시와 같은 빛 차오르는 소리
                    forge_bgm_want = -1;                    // 음악은 서서히 잦아든다
                }

                if(t == grand_peak)
                {
                    // 가장 눈부신 순간에 비로소 +29 검과 이름, 성공 메시지가 드러난다 (실패 낚시와 끝까지 구분되지 않게)
                    reveal_hold = false;
                    refresh_status();
                    show_message("강화 성공!", &blue_tint);
                }

                if(t == grand_peak - sfx_latency_frames)
                {
                    bn::sound_items::sfx_success_3.play();
                }
            }
            else
            {
                const int u = t - grand_buildup_end;
                const bn::color gold(31, 25, 4);
                const int settle_length = success_lengths[3] - grand_buildup_end;

                // 검은 떠 있다가 끝에서 제자리로 내려온다
                bn::fixed rise = u < settle_length - 40 ? bn::fixed(14) : bn::fixed(14) * (settle_length - u) / 40;
                offset_y = -rise + ((u / 12) % 2 == 0 ? bn::fixed(0) : bn::fixed(-1));
                fade_color = gold;
                fade = bn::fixed(0.5) * (settle_length - u) / settle_length;
                scale = u < 60 ? 1 + bn::fixed(0.35) * (60 - u) / 60 : bn::fixed(1);

                if(u == 30 || u == 75)
                {
                    spawn_sparks(0, 12, bn::fixed(1.0), bn::fixed(0.1), 40, spark_gold, spark_red);
                }

                if(u == 40)
                {
                    flash_level = bn::fixed(0.3);
                }

                if(u < settle_length - 40 && u % 3 == 0)
                {
                    emit_rain();
                }
            }
        }

        sword_palette.set_fade(fade_color, fade);
        sword.set_position(offset_x, sword_y + offset_y);

        if(scale != 1)
        {
            sword.set_scale(scale);
        }
        else if(sword.affine_mat())
        {
            sword.remove_affine_mat();
        }

        if(flash_level > 0)
        {
            flash_level -= bn::fixed(0.07);

            if(flash_level < 0)
            {
                flash_level = 0;
            }

            apply_flash();
        }

        update_sparks();
    };

    auto add_coin = [&](bn::fixed x, bn::fixed y, bn::fixed vx, bn::fixed vy, int floor_y, int delay)
    {
        for(coin_state& coin : coins)
        {
            if(! coin.active)
            {
                coin.active = true;
                coin.position = bn::fixed_point(x, y);
                coin.velocity = bn::fixed_point(vx, vy);
                coin.floor = floor_y;
                coin.delay = delay;
                coin.age = 0;
                coin.settled = 0;
                coin.sprite.reset();
                return;
            }
        }
    };

    auto clear_coins = [&]()
    {
        for(coin_state& coin : coins)
        {
            coin.active = false;
            coin.sprite.reset();
        }
    };

    // sold_level 검을 판 직후에 부른다 (돈은 이미 올랐고, 화면 숫자는 연출 동안 올라간다)
    auto start_sell_effect = [&](int sold_level, int money_before, int amount)
    {
        sell_tier = sold_level <= 5 ? 0 : sold_level <= 16 ? 1 : 2;
        sell_money_before = money_before;
        sell_amount = amount;
        current_phase = phase::SELL;
        phase_frame = 0;
        reveal_hold = true;         // 검 그림·이름·판매가는 연출이 끝날 때까지 판매 전 그대로
        clear_coins();

        // 위로 튀는 동전 (검이 있던 모루 위에서)
        const int fountain = sell_tier == 0 ? 5 : sell_tier == 1 ? 12 : 14;
        const bn::fixed spread = sell_tier == 0 ? bn::fixed(1.6) : sell_tier == 1 ? bn::fixed(2.4) : bn::fixed(3.2);
        const bn::fixed power = sell_tier == 0 ? bn::fixed(4.2) : sell_tier == 1 ? bn::fixed(5.2) : bn::fixed(6.0);

        for(int index = 0; index < fountain; ++index)
        {
            bn::fixed vx = bn::fixed(random.get_int(2001) - 1000) / 1000 * spread;
            bn::fixed vy = -power * (bn::fixed(60 + random.get_int(41)) / 100);
            add_coin(random.get_int(13) - 6, 34, vx, vy, coin_floor_y + random.get_int(17) - 8, index);
        }

        // 가장 높은 단계는 하늘에서 금화 소나기가 더해진다
        if(sell_tier == 2)
        {
            for(int index = 0; index < 18; ++index)
            {
                add_coin(random.get_int(161) - 80, -88, bn::fixed(random.get_int(9) - 4) / 10, bn::fixed(random.get_int(10)) / 10,
                         coin_floor_y + random.get_int(17) - 8, index * 2 + random.get_int(4));
            }
        }

        sword_palette.set_fade(bn::color(31, 31, 31), 1);
        sword.set_blending_enabled(true);
        bn::blending::set_transparency_alpha(1);
        flash_level = sell_tier == 2 ? bn::fixed(0.7) : sell_tier == 1 ? bn::fixed(0.35) : bn::fixed(0);

        if(sell_tier == 0)
        {
            bn::sound_items::sfx_sell_0.play();
        }
        else if(sell_tier == 1)
        {
            bn::sound_items::sfx_sell_1.play();
        }
        else
        {
            bn::sound_items::sfx_sell_2.play();
        }
    };

    auto end_sell_effect = [&]()
    {
        clear_coins();
        flash_level = 0;
        bn::bg_palettes::set_fade(bn::color(0, 0, 0), 0);
        bn::sprite_palettes::set_fade(bn::color(0, 0, 0), 0);

        for(int index = 0; index < spark_count; ++index)
        {
            spark_life[index] = 0;
            sparks[index].reset();
        }

        sword.set_blending_enabled(false);
        bn::blending::set_transparency_alpha(1);
        sword.set_visible(true);
        sword.set_position(0, sword_y);
        sword_palette.set_fade(bn::color(31, 31, 31), 0);
        reveal_hold = false;
        current_phase = phase::IDLE;
        refresh_status();       // +0 검과 판매 후 돈·이름이 나타난다
    };

    auto run_sell_effect = [&]()
    {
        const int t = phase_frame;
        const int vanish = sell_vanish_frames[sell_tier];

        // 검: 번쩍였다가 위로 떠오르며 흐려진다
        if(t < vanish + 6)
        {
            bn::fixed glow = t < 6 ? 1 - bn::fixed(t) / 6 : bn::fixed(0);
            sword_palette.set_fade(bn::color(31, 31, 31), bn::fixed(0.25) + bn::fixed(0.75) * glow);
            sword.set_y(sword_y - bn::fixed(t) * (sell_tier < 2 ? bn::fixed(1.2) : bn::fixed(1.6)));
            bn::fixed alpha = t < 4 ? bn::fixed(1) : 1 - bn::fixed(t - 4) / vanish;
            bn::blending::set_transparency_alpha(alpha < 0 ? bn::fixed(0) : alpha);
        }
        else
        {
            sword.set_visible(false);
        }

        // 반짝이는 별 (+6 이상)
        if(t == 1 && sell_tier > 0)
        {
            spawn_sparks(0, sell_tier == 1 ? 4 : 8, bn::fixed(1.2), bn::fixed(0.3), 22, spark_gold, spark_white);
        }

        update_sparks();

        // 동전
        for(coin_state& coin : coins)
        {
            if(! coin.active)
            {
                continue;
            }

            if(coin.delay > 0)
            {
                --coin.delay;
                continue;
            }

            ++coin.age;
            coin.velocity += bn::fixed_point(0, 0.28);
            coin.position += coin.velocity;

            if(coin.position.y() >= coin.floor && coin.velocity.y() > 0)
            {
                coin.position.set_y(coin.floor);
                coin.velocity = bn::fixed_point(coin.velocity.x() * bn::fixed(0.6), -coin.velocity.y() * bn::fixed(0.45));

                if(coin.velocity.y() > -0.8)
                {
                    coin.velocity.set_y(0);
                }
            }

            if(coin.velocity.y() == 0 && coin.position.y() >= coin.floor)
            {
                ++coin.settled;
            }

            if(coin.settled >= 14)
            {
                coin.sprite.reset();
                coin.active = false;
                continue;
            }

            if(! coin.sprite)
            {
                coin.sprite = bn::sprite_items::fx.create_sprite(coin.position.x(), coin.position.y(), fx_coin_first);
            }

            coin.sprite->set_position(coin.position);
            coin.sprite->set_item(bn::sprite_items::fx, fx_coin_first + (coin.age / 2) % 4);
            coin.sprite->set_visible(coin.settled <= 8 || coin.settled % 2 == 0);
        }

        // 화면 번쩍임
        if(flash_level > 0)
        {
            apply_flash();
            flash_level -= sell_tier == 2 ? bn::fixed(0.18) : bn::fixed(0.1);

            if(flash_level <= 0)
            {
                flash_level = 0;
                bn::bg_palettes::set_fade(bn::color(0, 0, 0), 0);
                bn::sprite_palettes::set_fade(bn::color(0, 0, 0), 0);
            }
        }

        // 돈이 올라간다 (연출 초반부터 끝나기 조금 전까지)
        const int count_start = 6;
        const int count_end = sell_lengths[sell_tier] - 10;

        if(t >= count_start)
        {
            int progress = t > count_end ? count_end - count_start : t - count_start;
            money_text.set_text(format_money(sell_money_before + int(int64_t(sell_amount) * progress / (count_end - count_start))));
        }
    };

    // +28 -> +29 실패 연출 (phase::DOOM): 성공과 똑같이 쌓이다가(grand_buildup) 빛이 뚝 꺼지고,
    // 금 간 검이 잿빛으로 멎었다가 천천히 가루가 되어 바스러진다. 별(sparks) 칸을 가루(dust)가 이어서 쓴다.
    constexpr int doom_silence_start = grand_buildup_end;      // 빛이 꺼지고 정적 (222)
    constexpr int doom_crumble_start = doom_silence_start + 40; // 바스락 (262)
    constexpr int doom_crumble_length = 130;
    constexpr int doom_length = doom_crumble_start + doom_crumble_length;

    // 빈 칸에 가루를 n개 만든다 (검이 있던 자리에서)
    auto emit_dust = [&](int count)
    {
        for(int index = 0; index < spark_count && count > 0; ++index)
        {
            if(spark_life[index] == 0)
            {
                spark_position[index] = bn::fixed_point(random.get_int(15) - 7, sword_y - 20 + random.get_int(44));
                spark_velocity[index] = bn::fixed_point(bn::fixed(random.get_int(9) - 4) / 8, -bn::fixed(random.get_int(4)) / 8);
                spark_life[index] = 40 + random.get_int(24);
                sparks[index] = bn::sprite_items::fx.create_sprite(spark_position[index], fx_dust_first + random.get_int(3));
                --count;
            }
        }
    };

    auto end_doom = [&]()
    {
        flash_level = 0;
        bn::bg_palettes::set_fade(bn::color(0, 0, 0), 0);
        bn::sprite_palettes::set_fade(bn::color(0, 0, 0), 0);

        for(int index = 0; index < spark_count; ++index)
        {
            spark_life[index] = 0;
            sparks[index].reset();
        }

        sword.set_blending_enabled(false);
        bn::blending::set_transparency_alpha(1);
        sword.remove_affine_mat();
        sword_palette.set_grayscale_intensity(0);
        sword_palette.set_fade(bn::color(31, 31, 31), 0);
        sword.set_item(*sword_items[sword_level], frame_whole);
        sword.set_visible(true);
        sword.set_position(0, sword_y);
        destroy_sword();        // +0 검과 파괴 메시지
    };

    auto run_doom_effect = [&]()
    {
        const int t = phase_frame;
        bn::fixed offset_x = 0;
        bn::fixed offset_y = 0;
        bn::fixed fade = 0;
        bn::color fade_color(31, 25, 4);
        bn::fixed scale = 1;        // 쌓이는 동안의 부풀기
        bn::fixed scale_y = 1;      // 바스러질 때 내려앉기
        bn::fixed dim = 0;          // 정적 이후 화면이 어두워진다

        if(t == doom_silence_start)
        {
            cut_forge_bgm();        // 빛이 꺼지는 순간 음악도 뚝 끊긴다
        }

        // +29 성공과 똑같이 빛이 차오르는 소리로 시작하고, 빛이 꺼지기 직전에 "빠직"
        if(t == 1)
        {
            bn::sound_items::sfx_glow.play();
            forge_bgm_want = -1;                            // 음악은 서서히 잦아든다
        }

        if(t == doom_silence_start - sfx_latency_frames)
        {
            bn::sound_items::sfx_fail.play();
        }

        if(t < doom_silence_start)
        {
            grand_buildup(t, offset_x, offset_y, fade, fade_color, scale);
        }
        else
        {
            if(t == doom_silence_start)
            {
                // 빛이 뚝 꺼진다: 별이 사라지고 검은 금 간 채 잿빛으로 멎는다
                for(int index = 0; index < spark_count; ++index)
                {
                    spark_life[index] = 0;
                    sparks[index].reset();
                }

                flash_level = 0;
                sword.set_item(*sword_items[sword_level], frame_cracked);
                sword_palette.set_grayscale_intensity(1);
            }

            fade_color = bn::color(22, 21, 20);
            fade = bn::fixed(0.25);
            dim = bn::fixed(t - doom_silence_start < 30 ? t - doom_silence_start : 30) / 30 * bn::fixed(0.4);

            if(t >= doom_crumble_start)
            {
                // 바스락: 가루가 흩날리며 검이 천천히 내려앉고 흐려진다
                const int u = t - doom_crumble_start;

                if(u == 0)
                {
                    show_message("강화 실패..", &red_tint);
                    emit_dust(20);
                    sword.set_blending_enabled(true);
                }
                else if(u < 90 && u % 4 == 0)
                {
                    emit_dust(2);
                }

                bn::fixed alpha = 1 - bn::fixed(u) / 95;
                bn::blending::set_transparency_alpha(alpha < 0 ? bn::fixed(0) : alpha);
                scale_y = 1 - bn::fixed(u) * bn::fixed(0.012);

                if(scale_y < bn::fixed(0.25))
                {
                    scale_y = bn::fixed(0.25);
                }

                offset_y = (1 - scale_y) * 20;
                offset_x = u < 8 ? (u % 2 == 0 ? bn::fixed(2) : bn::fixed(-2)) : bn::fixed(0);

                for(int index = 0; index < spark_count; ++index)
                {
                    spark_velocity[index] += bn::fixed_point(0, bn::fixed(0.05));    // 가루는 천천히 가라앉는다
                }
            }
        }

        sword_palette.set_fade(fade_color, fade);
        sword.set_position(offset_x, sword_y + offset_y);

        if(scale_y != 1)
        {
            sword.set_vertical_scale(scale_y);
        }
        else if(scale != 1)
        {
            sword.set_scale(scale);
        }
        else if(sword.affine_mat())
        {
            sword.remove_affine_mat();
        }

        if(flash_level > 0)
        {
            flash_level -= bn::fixed(0.07);

            if(flash_level < 0)
            {
                flash_level = 0;
            }

            apply_flash();
        }
        else if(dim > 0)
        {
            bn::bg_palettes::set_fade(bn::color(0, 0, 0), dim);
        }

        update_sparks();
    };

    refresh_status();
    message_text(first_message);
    refresh_status();
    fade_screen(true);

    int clock_frames = 0;

    auto update_idle_sword = [&]()
    {
        // 단계별 폭(px)과 주기(프레임): +0~5 1px/2초, +6~16 2px/1.6초, +17~ 3px/1.2초
        const int amplitude = sword_level <= 5 ? 1 : sword_level <= 16 ? 2 : 3;
        const int period = sword_level <= 5 ? 120 : sword_level <= 16 ? 96 : 72;
        const bn::fixed bob = amplitude * bn::degrees_lut_sin(bn::fixed(360 * (idle_tick % period)) / period);
        sword.set_y(sword_y + bob);

        if(sword_level >= 17)
        {
            const int phase_tick = idle_tick % 40;

            if(phase_tick == 0)
            {
                // 날 위쪽 어딘가에서 금빛 별이 한 번 반짝인다 (위치는 틱으로 돌려 쓴다. 게임 난수는 건드리지 않는다)
                const int seed = (idle_tick / 40) * 7919;
                const int gx = (seed % 13) - 6;
                idle_glint_dy = -26 + (seed / 13) % 24;
                idle_glint = bn::sprite_items::fx.create_sprite(gx, sword_y + bob + idle_glint_dy, spark_gold);
            }

            if(idle_glint)
            {
                idle_glint->set_y(sword_y + bob + idle_glint_dy);
                idle_glint->set_visible(phase_tick < 6 && phase_tick % 2 == 0);

                if(phase_tick >= 6)
                {
                    idle_glint.reset();
                }
            }
        }
        else
        {
            idle_glint.reset();
        }

        ++idle_tick;
    };

    while(true)
    {
        // (아래 IDLE에서 update_idle_sword를 부른다)
        // 시간대 경계(16시·21시·06시)를 넘으면 성공률 보정과 표시를 바로 바꾼다 (1초에 한 번 확인)
        if(++clock_frames >= 60)
        {
            clock_frames = 0;
            int penalty = hs::time_penalty_pct(hs::current_time_band());

            if(penalty != eco.time_penalty())
            {
                eco.set_time_penalty(penalty);

                if(current_phase == phase::IDLE && ! help_mode)
                {
                    refresh_status();
                }
            }

            // 배경도 시간대가 바뀌면 바꾼다 (평상시에만 — 연출·도움말 중에는 다음 기회에)
            const hs::time_band band_now = hs::current_time_band();

            if(band_now != shown_band && current_phase == phase::IDLE && ! help_mode)
            {
                shown_band = band_now;
                background_palette.reset();     // 배경 타일 VRAM이 모자라서 새 배경을 만들기 전에 이전 것을(팔레트까지) 먼저 반납한다
                background.reset();
                background = forge_bg_item(band_now).create_bg(title_bg_x, title_bg_y);
                background_palette = background->palette();
            }
        }

        tick_forge_bgm();

        if(help_labels_delay > 0 && --help_labels_delay == 0 && help_mode)
        {
            build_help_labels();
        }

        if(status_refresh_delay > 0 && --status_refresh_delay == 0 && ! help_mode)
        {
            refresh_status();
        }

#ifdef HS_DEBUG
        {
            const int used = bn::sprite_tiles::used_tiles_count();
            vram_peak = used > vram_peak ? used : vram_peak;

            if(++vram_frame % 10 == 0)
            {
                bn::string<24> vram_line("T ");
                vram_line += bn::to_string<8>(used);
                vram_line += " / ";
                vram_line += bn::to_string<8>(vram_peak);
                vram_text.set_text(vram_line);
            }
        }
#endif

        switch(current_phase)
        {

        case phase::IDLE:
            set_forge_bgm_level(eco.level());

            // 고양이 이벤트: 실패 결과 메시지를 다 보여 준 뒤 (파산 판정보다 먼저: 선물이 구제가 될 수 있다).
            // 고양이 차례가 된 뒤에는 메시지가 끝날 때까지 버튼을 받지 않는다. 안 그러면 그 사이에 다시 강화해서
            // 성공한 뒤에 고양이가 나오는 일이 생긴다 (고양이는 "실패" 때문에 오는 것).
            if(! help_mode && eco.cat_due())
            {
                if(message_timer == 0)
                {
                    fade_screen(false);
                    return forge_exit::CAT;
                }

                if(! reveal_hold)
                {
                    update_idle_sword();
                }

                break;
            }

            // 파산: 메시지(판매·파괴 결과)를 다 보여 준 뒤 게임 오버
            if(message_timer == 0 && ! help_mode && eco.is_bankrupt())
            {
                fade_screen(false);
                return forge_exit::GAME_OVER;
            }

            // 엔딩: "엔딩으로.." 메시지가 끝난 뒤
            if(ending_pending && message_timer == 0 && ! help_mode)
            {
                ending_pending = false;
                fade_screen(false);
                return forge_exit::ENDING;
            }

            // 도둑 이벤트: 결과 메시지가 끝난 뒤 (게임 오버 판정 다음)
            if(thief_pending && message_timer == 0 && ! help_mode)
            {
                thief_pending = false;

                if(eco.thief_ready())
                {
                    fade_screen(false);
                    return forge_exit::THIEF;
                }
            }

            // 수상한 상인: 판매 결과 메시지가 끝난 뒤
            if(merchant_pending && message_timer == 0 && ! help_mode)
            {
                merchant_pending = false;

                if(eco.merchant_ready())
                {
                    fade_screen(false);
                    return forge_exit::MERCHANT;
                }
            }

            if(! help_mode && ! reveal_hold)
            {
                update_idle_sword();
            }

            if(bn::keypad::select_pressed())
            {
                set_help(! help_mode);
            }
            else if(help_mode)
            {
                if(bn::keypad::start_pressed())
                {
                    play_select();
                    fade_screen(false);
                    return forge_exit::RECIPE;
                }

#ifdef HS_DEBUG
                // 디버그: ↑ 을 누를 때마다 시간대를 낮 → 노을 → 밤 → 실제 시계로 돌리고, 세 번째 줄에 지금 시간대를 보여 준다
                if(bn::keypad::up_pressed())
                {
                    debug_band_name = hs::debug_cycle_time_band();
                    help_key = -1;
                }
#endif

                int key = bn::keypad::a_held() ? 0 : bn::keypad::b_held() ? 1 : bn::keypad::l_held() ? 2 :
                          bn::keypad::r_held() ? 3 : bn::keypad::left_held() ? 4 : bn::keypad::down_held() ? 5 :
                          bn::keypad::up_held() ? 6 : -1;

                if(key != help_key)
                {
                    help_key = key;
                    help_line_1.set_text(key < 0 ? "" : help_texts[key][0]);
                    help_line_2.set_text(key < 0 ? "" : help_texts[key][1]);
                    help_line_3.set_text(key < 0 ? "" : help_texts[key][2]);
#ifdef HS_DEBUG
                    if(key == 6 && debug_band_name)
                    {
                        help_line_3.set_text(debug_band_name);       // 디버그: 지금 강제로 정한 시간대
                    }
#endif
                }
            }
            else if(bn::keypad::a_pressed())
            {
                if(eco.at_max_level())
                {
                    // 최종 검(+29): 엔딩으로 가는 안내 (화려한 색이 도는 글자가 흔들린다). 메시지가 끝나면 엔딩 화면으로 간다.
                    show_message("엔딩으로..", &money_palette);
                    ending_shake = true;
                    ending_pending = true;
                }
                else if(! eco.can_afford_money())
                {
                    message_text("돈이 없다!");
                }
                else if(! eco.can_afford_materials())
                {
                    message_text("재료 부족!");
                }
                else
                {
                    eco.set_time_penalty(hs::time_penalty_pct(hs::current_time_band()));    // 판정 순간의 시계로
#ifdef HS_DEBUG
                    if(debug_force == 1)
                    {
                        eco.set_time_penalty(100);      // 성공률 0%
                    }
                    else if(debug_force == 2)
                    {
                        eco.set_time_penalty(-100);     // 성공률 100% 이상
                    }
#endif
                    upgrade_succeeded = eco.upgrade(random);

                    // HARD: 새 최고 단계에 닿으면 그 순간까지의 숫자를 최고 도달 기록으로 남긴다 (저장 슬롯과 별개)
                    if(eco.take_new_best() && eco.diff() == hs::difficulty::HARD)
                    {
                        hs::save::write_best(eco.best_level(), eco.stats());
                    }
                    roll_thief();               // 밤이면 도둑 판정 (강화 한 번)
                    current_phase = phase::HAMMERING;
                    phase_frame = 0;
                    bn::sound_items::sfx_hammer.play();
                    refresh_status();
                    message_text("땅! 땅! 땅!");
                }
            }
            else if(bn::keypad::b_pressed())
            {
                if(! eco.can_sell())
                {
                    show_message("팔 수 없다!", nullptr);
                }
                else
                {
                    const int sold_level = eco.level();
                    const int money_before = eco.money();
                    const int amount = eco.sell();
                    bn::string<48> sold_line("판매 +");
                    sold_line += format_money_short(amount);
                    message_text(sold_line);
                    start_sell_effect(sold_level, money_before, amount);
                    roll_thief();               // 밤이면 도둑 판정 (판매 한 번)
                    roll_merchant();            // 노을이면 수상한 상인 판정 (판매 한 번)
                }
            }
            else if(bn::keypad::l_pressed() || bn::keypad::r_pressed())
            {
                if(eco.level() != 0)
                {
#ifdef HS_DEBUG
                    if(bn::keypad::r_pressed())
                    {
                        debug_force = (debug_force + 1) % 3;
                        message_text(debug_force == 1 ? "DEBUG FAIL" : debug_force == 2 ? "DEBUG SUCCESS" : "DEBUG OFF");
                    }
                    else if(bn::keypad::l_pressed())
                    {
                        constexpr int jump_levels[11] = { 5, 14, 16, 21, 22, 23, 24, 25, 27, 28, 29 };
                        eco.debug_set_level(jump_levels[debug_jump_index]);
                        debug_jump_index = (debug_jump_index + 1) % 11;
                        refresh_status();
                    }
                    else
#endif
                    message_text("+0일 때만\n갈 수 있다");
                }
                else
                {
                    bool shop = bn::keypad::l_pressed();
                    fade_screen(false);
                    return shop ? forge_exit::SHOP : forge_exit::FURNACE;
                }
            }
            else if(bn::keypad::left_pressed())
            {
                fade_screen(false);
                return forge_exit::SWORDS;
            }
            else if(bn::keypad::down_pressed())
            {
                fade_screen(false);
                return forge_exit::BAG;
            }
            else if(bn::keypad::start_pressed())
            {
                fade_screen(false);
                return forge_exit::PAUSE;
            }
#ifdef HS_DEBUG
            else if(bn::keypad::up_pressed())
            {
                if(bn::keypad::b_held())
                {
                    eco.debug_bankrupt();       // B를 누른 채 ↑: 파산 상태 만들기
                    message_text("DEBUG BANKRUPT");
                }
                else
                {
                    eco.debug_grant();
                    message_text("DEBUG\n+$100,000");
                }

                refresh_status();
            }
#endif
            break;

        case phase::HAMMERING:
            ++phase_frame;

            // 망치질 소리(sfx_hammer)의 두 번 "땅!"에 맞춰 검이 움찔한다 (0.22초 ≈ 13프레임 간격)
            {
                int jolt = 0;

                for(int hit : hammer_hit_frames)
                {
                    int elapsed = phase_frame - hit;

                    if(elapsed >= 0 && elapsed < 6)
                    {
                        jolt = elapsed < 3 ? 3 : 1;
                    }
                }

                sword.set_y(sword_y + jolt);

            }

            if(upgrade_succeeded && phase_frame == hammer_frames - sfx_latency_frames && eco.level() < hs::max_level)
            {
                // 연출 단계(+1~5 / +6~16 / +17~28)와 같은 결의 성공음
                const int level_now = eco.level();

                if(level_now <= 5)
                {
                    bn::sound_items::sfx_success_0.play();
                }
                else if(level_now <= 16)
                {
                    bn::sound_items::sfx_success_1.play();
                }
                else
                {
                    bn::sound_items::sfx_success_2.play();
                }
            }

            if(phase_frame >= hammer_frames)
            {
                sword.set_y(sword_y);
                phase_frame = 0;

                if(upgrade_succeeded)
                {
                    success_tier = eco.level() <= 5 ? 0 : eco.level() <= 16 ? 1 : eco.level() <= 28 ? 2 : 3;
                    success_length = success_lengths[success_tier];
                    flash_level = 0;
                    reveal_hold = success_tier == 3;
                    current_phase = phase::SUCCESS;
                    refresh_status();

                    if(! reveal_hold)
                    {
                        show_message("강화 성공!", &blue_tint);
                    }
                }
                else if(sword_level == hs::max_level - 1)
                {
                    // +28 -> +29 실패: 결과를 일부러 숨기고 성공처럼 보이게 한다 (메시지는 바스러질 때)
                    current_phase = phase::DOOM;
                    flash_level = 0;
                }
                else
                {
                    current_phase = phase::FAIL;
                    show_message("강화 실패..", &red_tint);
                }
            }
            break;

        case phase::DOOM:
            ++phase_frame;
            run_doom_effect();

            if(phase_frame >= doom_length)
            {
                end_doom();
            }
            break;

        case phase::SELL:
            ++phase_frame;
            run_sell_effect();

            if(phase_frame >= sell_lengths[sell_tier])
            {
                end_sell_effect();
            }
            break;

        case phase::SUCCESS:
            ++phase_frame;
            run_success_effect();

            if(phase_frame >= success_length)
            {
                end_success();
            }
            break;

        case phase::FAIL:
            ++phase_frame;
            sword.set_x(phase_frame >= fail_frames ? 0 : ((phase_frame / 2) % 2 == 0 ? -3 : 3));

            // 절반쯤에서 "빠직": 오른쪽 위에서 왼쪽 아래로 금이 가며 번쩍인다
            if(phase_frame == crack_frame - sfx_latency_frames)
            {
                bn::sound_items::sfx_fail.play();
            }

            if(phase_frame == crack_frame)
            {
                sword.set_item(*sword_items[sword_level], frame_cracked);
            }

            if(phase_frame >= crack_frame)
            {
                sword_palette.set_fade(bn::color(31, 31, 31), bn::fixed(fail_frames - phase_frame) / (fail_frames - crack_frame));
            }

            if(phase_frame >= fail_frames)
            {
                if(eco.can_protect())
                {
                    bn::string<48> ask_line("방지권 ");
                    ask_line += bn::to_string<4>(eco.row().protect_cost);
                    ask_line += "장?";

                    clear_message();
                    message_line_1.set_y(message_first_y);
                    message_line_1.set_text(ask_line);
                    message_line_2.set_text("A 예 B 아니오");
                    message_timer = -1;     // 답할 때까지 유지 (금 간 검을 그대로 둔다)
                    current_phase = phase::PROTECT_ASK;
                }
                else
                {
                    start_break();
                }
            }
            break;

        case phase::PROTECT_ASK:
            if(bn::keypad::a_pressed())
            {
                play_select();
                eco.protect();
                current_phase = phase::RESTORE;
                phase_frame = 0;
                sword.set_item(*sword_items[sword_level], frame_whole);     // 금이 사라지고 번쩍
                sword_palette.set_fade(bn::color(31, 31, 31), 1);
                show_message("복구 했다!", nullptr);
                refresh_status();
            }
            else if(bn::keypad::b_pressed())
            {
                start_break();
            }
            break;

        case phase::RESTORE:
            ++phase_frame;
            sword_palette.set_fade(bn::color(31, 31, 31), bn::fixed(restore_frames - phase_frame) / restore_frames);

            if(phase_frame >= restore_frames)
            {
                current_phase = phase::IDLE;
                refresh_status();
            }
            break;

        case phase::BREAK:
            ++phase_frame;
            upper_velocity += bn::fixed_point(0, 0.22);
            lower_velocity += bn::fixed_point(0, 0.3);
            upper_position += upper_velocity;
            lower_position += lower_velocity;
            sword.set_position(upper_position);
            sword.set_rotation_angle(bn::fixed(360) - phase_frame * 3);      // 위쪽 조각은 왼쪽으로 돌며
            piece_lower->set_position(lower_position);
            piece_lower->set_rotation_angle(phase_frame * 2);                       // 아래쪽 조각은 오른쪽으로 돈다

            for(int index = 0; index < shards.size(); ++index)
            {
                shard_velocity[index] += bn::fixed_point(0, 0.25);
                shard_position[index] += shard_velocity[index];
                shards[index].set_position(shard_position[index]);
                shards[index].set_visible(phase_frame < shard_frames);
            }

            if(phase_frame >= break_frames)
            {
                sword.set_rotation_angle(0);
                sword.set_position(0, sword_y);       // 위쪽 조각으로 쓰던 검 스프라이트를 제자리로 (+0에서 깨지면 새 스프라이트를 만들지 않아 날아간 자리에 남던 문제)
                piece_lower.reset();
                shards.clear();
                sword.set_item(*sword_items[sword_level], frame_whole);
                sword.set_visible(true);
                destroy_sword();        // 다음 검(+0)이 나오고 파괴 메시지·잡템이 뜬다
            }
            break;

        default:
            break;
        }

        if(ending_shake)
        {
            const bn::sprite_palette_ptr* cycle[3] = { &money_palette, &red_tint, &blue_tint };
            message_line_1.set_palette(*cycle[(ending_frame / 5) % 3]);
            message_line_1.set_offset((ending_frame % 2 == 0 ? 1 : -1), ((ending_frame / 2) % 2 == 0 ? 1 : -1));
            ++ending_frame;
        }
        else
        {
            ending_frame = 0;
        }

        if(message_timer > 0 && --message_timer == 0)
        {
            clear_message();
        }

        bn::core::update();
    }
}

// ─── 목록 화면 공통 ──────────────────────────────────────────────────

constexpr int list_cursor_x = -114;
constexpr int list_name_x = -100;
// 아이콘(16x16)은 이름 자리 왼쪽 끝에 놓고, 글자는 그 오른쪽에서 시작한다
constexpr int list_icon_x = list_name_x + 8;        // 아이콘 가운데
constexpr int list_text_x = list_name_x + 19;       // 아이콘이 있는 줄의 글자 시작

void add_icon(const bn::sprite_item& item, int x, int y, bn::ivector<bn::sprite_ptr>& sprites)
{
    sprites.push_back(item.create_sprite(x, y));
}
constexpr int notice_frames = 90;
constexpr int ready_light_x = 108;       // 초록불(살 수 있음·만들 수 있음) 가운데
constexpr int shop_price_x = 8;          // 상점 금액 왼쪽 끝 ($에 맞춤. 오른쪽 점원이 보이게 비운다)

class list_cursor
{

public:
    list_cursor(bn::sprite_text_generator& left_generator, int top_y, int step_y) :
        _step_y(step_y)
    {
        left_generator.generate(list_cursor_x, top_y, ">", _sprites);
        _origin = _sprites[0].position();
    }

    void update(int index, int frame, bool visible)
    {
        bn::sprite_ptr& sprite = _sprites[0];
        sprite.set_visible(visible);
        sprite.set_position(_origin.x() - (frame / 16) % 2, _origin.y() + index * _step_y);
    }

private:
    int _step_y;
    bn::vector<bn::sprite_ptr, 1> _sprites;
    bn::fixed_point _origin;
};

// ─── 상점 (L) ────────────────────────────────────────────────────────

void run_shop(hs::economy& eco, bn::seed_random& random)
{
    bn::sprite_text_generator left_generator(hs::font);
    left_generator.set_left_alignment();

    bn::sprite_text_generator right_generator(hs::font);
    right_generator.set_right_alignment();

    constexpr int top_y = -50;
    constexpr int step_y = 16;

    // 배경: 흑백 상점 그림 (tools/make_menu_bgs.py)
    bn::regular_bg_ptr background = bn::regular_bg_items::menu_bg_shop.create_bg(title_bg_x, title_bg_y);

    text_line title_text(left_generator, -114, -72);
    bn::sprite_text_generator money_generator(hs::font_outline);
    money_generator.set_right_alignment();
    text_line money_text(money_generator, 114, -72);
    money_text.set_sticky_palette(create_two_tone_font_palette(gold_body_color, gold_outline_color));
    text_line status_text(left_generator, -114, 50);
    text_line footer_text(left_generator, -114, 68);

    bn::vector<bn::sprite_ptr, 96> row_sprites;

    for(int index = 0; index < hs::shop_item_count; ++index)
    {
        const hs::shop_item& item = hs::shop_items[index];
        add_icon(item.effect == hs::shop_effect::PROTECT ? bn::sprite_items::ico_ticket : bn::sprite_items::ico_warp,
                 list_icon_x, top_y + index * step_y, row_sprites);
        left_generator.generate(list_text_x, top_y + index * step_y, item.name, row_sprites);
        left_generator.generate(shop_price_x, top_y + index * step_y, format_money(eco.price(item)), row_sprites);
    }

    // 지금 살 수 있는 줄 끝에 초록불 (돈이 충분하고, 워프는 +0 검일 때만)
    bn::vector<bn::sprite_ptr, hs::shop_item_count> ready_lights;

    list_cursor cursor(left_generator, top_y, step_y);
    int selected = 0;
    int frame = 0;
    int notice = 0;

    auto refresh_status = [&]()
    {
        money_text.set_text(format_money(eco.money()));

        ready_lights.clear();

        for(int index = 0; index < hs::shop_item_count; ++index)
        {
            const hs::shop_item& item = hs::shop_items[index];

            if(eco.money() >= eco.price(item) && (item.effect != hs::shop_effect::WARP || eco.level() == 0))
            {
                ready_lights.push_back(bn::sprite_items::ico_ready.create_sprite(ready_light_x, top_y + index * step_y));
            }
        }

        bn::string<48> status_line("방지권 ");
        status_line += bn::to_string<8>(eco.tickets());
        status_line += "장  지금 검 +";
        status_line += bn::to_string<4>(eco.level());
        status_text.set_text(status_line);
    };

    title_text.set_text("상점");
    footer_text.set_text("A 구매  B 돌아가기");
    refresh_status();
    fade_screen(true);

    while(! bn::keypad::b_pressed())
    {
        ++frame;

        if(bn::keypad::up_pressed())
        {
            selected = (selected + hs::shop_item_count - 1) % hs::shop_item_count;
            play_menu_move();
        }
        else if(bn::keypad::down_pressed())
        {
            selected = (selected + 1) % hs::shop_item_count;
            play_menu_move();
        }
        else if(bn::keypad::a_pressed())
        {
            play_select();
            const hs::shop_item& item = hs::shop_items[selected];
            bn::string<48> result_line;

            switch(eco.buy(item, random))
            {

            case hs::buy_result::OK:
                result_line = "방지권 +";
                result_line += bn::to_string<4>(item.amount);
                result_line += "장";
                break;

            case hs::buy_result::NOT_ENOUGH_MONEY:
                result_line = "돈이 모자란다";
                break;

            case hs::buy_result::WARP_NEEDS_PLUS_ZERO:
                result_line = "워프는 +0 검일 때만 쓸 수 있다";
                break;

            case hs::buy_result::WARP_SUCCESS:
                result_line = "워프 성공! 검이 +";
                result_line += bn::to_string<4>(item.amount);
                break;

            case hs::buy_result::WARP_FAILED:
                result_line = "워프 실패...";
                break;

            default:
                break;
            }

            footer_text.set_text(result_line);
            notice = notice_frames;
            refresh_status();
        }

        if(notice > 0 && --notice == 0)
        {
            footer_text.set_text("A 구매  B 돌아가기");
        }

        cursor.update(selected, frame, true);
        bn::core::update();
    }

    fade_screen(false);
}

// ─── 창고 (←) / 잡화 (↓) ──────────────────────────────────────

constexpr int list_top_y = -52;
constexpr int list_step_y = 14;

// sword_rack = true면 보관한 검(꺼내면 현재 검과 맞바꿈), false면 잡템 목록(보기만 한다. 쓰는 곳은 화구)
void run_storage(hs::economy& eco, bool sword_rack)
{
    bn::sprite_text_generator left_generator(hs::font);
    left_generator.set_left_alignment();

    bn::sprite_text_generator right_generator(hs::font);
    right_generator.set_right_alignment();

    constexpr int quantity_x = 0;

    // 배경: 창고(검 진열대) / 잡화(자루) 흑백 그림 (tools/make_menu_bgs.py)
    bn::regular_bg_ptr background = (sword_rack ? bn::regular_bg_items::menu_bg_storage
                                                : bn::regular_bg_items::menu_bg_bag).create_bg(title_bg_x, title_bg_y);

    text_line title_text(left_generator, -114, -72);
    text_line ticket_text(right_generator, 114, -72);
    text_line footer_text(left_generator, -114, 68);

    bn::vector<bn::sprite_ptr, 112> row_sprites;
    list_cursor cursor(left_generator, list_top_y, list_step_y);

    int selected = 0;
    int frame = 0;
    int notice = 0;

    auto selectable_count = [&]()
    {
        return sword_rack ? eco.stored_swords().size() : 0;
    };

    auto footer_hint = [&]()
    {
        footer_text.set_text(sword_rack ? "A 꺼내기  R 보관  B 뒤로" : "B 돌아가기");
    };

    auto refresh = [&]()
    {
        row_sprites.clear();

        bn::string<32> ticket_line("방지권 ");
        ticket_line += bn::to_string<8>(eco.tickets());
        ticket_text.set_text(ticket_line);

        int row = 0;

        if(sword_rack)
        {
            bn::string<48> title_line("창고  지금 +");
            title_line += bn::to_string<4>(eco.level());
            title_text.set_text(title_line);

            for(int8_t level : eco.stored_swords())
            {
                add_icon(*sword_icons[level], list_icon_x, list_top_y + row * list_step_y, row_sprites);
                left_generator.generate(list_text_x, list_top_y + row * list_step_y, sword_label(level), row_sprites);
                ++row;
            }
        }
        else
        {
            title_text.set_text("잡화");

            for(int index = 0; index < hs::material_count; ++index)
            {
                int quantity = eco.material_quantity(hs::material(index));

                if(quantity > 0)
                {
                    int y = list_top_y + row * list_step_y;
                    bn::string<16> quantity_line("x");
                    quantity_line += bn::to_string<8>(quantity);
                    add_icon(*material_icons[index], list_icon_x, y, row_sprites);
                    left_generator.generate(list_text_x, y, hs::material_names[index], row_sprites);
                    left_generator.generate(quantity_x, y, quantity_line, row_sprites);
                    ++row;
                }
            }
        }

        if(row == 0)
        {
            left_generator.generate(list_name_x, list_top_y, "비어 있음", row_sprites);
        }

        if(selected >= selectable_count())
        {
            selected = selectable_count() > 0 ? selectable_count() - 1 : 0;
        }
    };

    refresh();
    footer_hint();
    fade_screen(true);

    while(! bn::keypad::b_pressed())
    {
        ++frame;
        int count = selectable_count();

        if(count > 0 && bn::keypad::up_pressed())
        {
            selected = (selected + count - 1) % count;
            play_menu_move();
        }
        else if(count > 0 && bn::keypad::down_pressed())
        {
            selected = (selected + 1) % count;
            play_menu_move();
        }
        else if(sword_rack && bn::keypad::r_pressed())
        {
            // 현재 검을 보관함에 넣는다 (재료로 쓸 +19, +21, +22 검을 모을 때)
            bn::string<48> result_line;

            if(eco.level() == 0)
            {
                result_line = "보관할 검이 없다";
            }
            else if(! eco.can_store_sword())
            {
                result_line = "창고가 가득 찼다";
            }
            else
            {
                result_line = sword_label(eco.level());
                result_line += " 보관";
                eco.store_sword();
            }

            refresh();
            footer_text.set_text(result_line);
            notice = notice_frames;
        }
        else if(count > 0 && bn::keypad::a_pressed())
        {
            play_select();
            eco.take_out_sword(selected);
            bn::string<48> result_line = sword_label(eco.level());
            result_line += " 장착";

            refresh();
            footer_text.set_text(result_line);
            notice = notice_frames;
        }

        if(notice > 0 && --notice == 0)
        {
            footer_hint();
        }

        cursor.update(selected, frame, selectable_count() > 0);
        bn::core::update();
    }

    fade_screen(false);
}

// ─── 화구 (R): 잡템 → 방지권 / 검(창고로) ───────────────────────

void run_crafting(hs::economy& eco)
{
    bn::sprite_text_generator left_generator(hs::font);
    left_generator.set_left_alignment();

    bn::sprite_text_generator right_generator(hs::font);
    right_generator.set_right_alignment();

    // 배경: 흑백 화로 그림 (tools/make_menu_bgs.py)
    bn::regular_bg_ptr background = bn::regular_bg_items::menu_bg_furnace.create_bg(title_bg_x, title_bg_y);

    constexpr int output_icon_x = shop_price_x + 20;  // 결과 아이콘 가운데 (보유 수량이 세 자리(최대 999)여도 이름과 겹치지 않게 오른쪽으로 옮겼다)
    constexpr int count_x = output_icon_x - 12;              // 보유/필요 수량(오른쪽 맞춤): 가장 긴 이름(네 글자)과 세 자리 보유 수량이 겹치지 않는 자리
    constexpr int output_text_x = output_icon_x + 11;   // 결과 글자

    text_line title_text(left_generator, -114, -72);
    text_line ticket_text(right_generator, 114, -72);
    text_line footer_text(left_generator, -114, 68);

    // 교환이 9줄이라 다른 목록보다 위에서 좁게 시작한다
    constexpr int craft_top_y = -56;
    constexpr int craft_step_y = 13;

    bn::vector<bn::sprite_ptr, 112> row_sprites;
    list_cursor cursor(left_generator, craft_top_y, craft_step_y);

    int selected = 0;
    int frame = 0;
    int notice = 0;

    auto footer_hint = [&]()
    {
        footer_text.set_text("A 조합  B 돌아가기");
    };

    auto refresh = [&]()
    {
        row_sprites.clear();

        bn::string<32> ticket_line("방지권 ");
        ticket_line += bn::to_string<8>(eco.tickets());
        ticket_text.set_text(ticket_line);

        for(int index = 0; index < hs::recipe_count; ++index)
        {
            const hs::recipe& recipe = hs::recipes[index];
            int y = craft_top_y + index * craft_step_y;

            const int owned = eco.material_quantity(recipe.input);
            bn::string<16> count_line = bn::to_string<16>(owned);
            count_line += "/";
            count_line += bn::to_string<4>(eco.required_quantity(recipe));

            bn::string<32> output_line;

            if(recipe.output == hs::craft_output::PROTECT)
            {
                add_icon(bn::sprite_items::ico_ticket, output_icon_x, y, row_sprites);
                output_line = "x";
                output_line += bn::to_string<4>(eco.output_amount(recipe));
            }
            else
            {
                add_icon(*sword_icons[eco.output_amount(recipe)], output_icon_x, y, row_sprites);
                output_line = "+";
                output_line += bn::to_string<4>(eco.output_amount(recipe));
            }

            add_icon(*material_icons[int(recipe.input)], list_icon_x, y, row_sprites);
            left_generator.generate(list_text_x, y, hs::material_names[int(recipe.input)], row_sprites);
            right_generator.generate(count_x, y, count_line, row_sprites);
            left_generator.generate(output_text_x, y, output_line, row_sprites);

            // 지금 만들 수 있으면 줄 끝에 초록불 (재료가 충분하고, 검이면 창고에 자리가 있을 때)
            if(eco.material_quantity(recipe.input) >= eco.required_quantity(recipe) &&
               (recipe.output != hs::craft_output::SWORD || eco.stored_swords().size() < eco.stored_swords().max_size()))
            {
                add_icon(bn::sprite_items::ico_ready, ready_light_x, y, row_sprites);
            }
        }
    };

    title_text.set_text("화구");
    refresh();
    footer_hint();
    fade_screen(true);

    while(! bn::keypad::b_pressed())
    {
        ++frame;

        if(bn::keypad::up_pressed())
        {
            selected = (selected + hs::recipe_count - 1) % hs::recipe_count;
            play_menu_move();
        }
        else if(bn::keypad::down_pressed())
        {
            selected = (selected + 1) % hs::recipe_count;
            play_menu_move();
        }
        else if(bn::keypad::a_pressed())
        {
            play_select();
            const hs::recipe& recipe = hs::recipes[selected];
            bn::string<48> result_line;

            switch(eco.craft(recipe))
            {

            case hs::craft_result::OK:
                result_line = recipe.output == hs::craft_output::SWORD ? "조합 완료! 검은 창고로" : "조합 완료!";
                break;

            case hs::craft_result::NOT_ENOUGH_MATERIAL:
                result_line = "재료가 모자란다";
                break;

            case hs::craft_result::STORAGE_FULL:
                result_line = "창고가 가득 찼다";
                break;

            default:
                break;
            }

            refresh();
            footer_text.set_text(result_line);
            notice = notice_frames;
        }

        if(notice > 0 && --notice == 0)
        {
            footer_hint();
        }

        cursor.update(selected, frame, true);
        bn::core::update();
    }

    fade_screen(false);
}

// ─── 슬롯 목록 (일시정지 Save / 타이틀 불러오기 공통) ─────────────────

// "SLOT 1  STORY +13  $1.3K" 또는 "SLOT 1  비어 있음"
bn::string<48> slot_label(int slot)
{
    hs::save::slot_info info = hs::save::info(slot);
    bn::string<48> line("SLOT ");
    line += bn::to_string<4>(slot + 1);
    line += "  ";

    if(! info.used)
    {
        line += "비어 있음";
        return line;
    }

    line += info.diff == hs::difficulty::HARD ? "HARD +" : "STORY +";
    line += bn::to_string<4>(info.level);
    line += "  ";
    line += format_money_short(info.money);
    return line;
}

// 노란 도트 화살표 (tools/make_menu_arrow.py): 고른 줄 왼쪽에 고정되고, 고른 글자만 흔들린다.
// 8x16 그림이라 글자 줄(가운데 정렬, 글자 폭 text_width, 가운데 높이 y)의 가운데에 그대로 맞는다.
class menu_arrow
{

public:
    menu_arrow() :
        _sprite(bn::sprite_items::menu_arrow.create_sprite(0, 0))
    {
        _sprite.set_visible(false);
    }

    void show(int text_width, int y)
    {
        _sprite.set_position(-text_width / 2 - 11, y);
        _sprite.set_visible(true);
    }

    void hide()
    {
        _sprite.set_visible(false);
    }

private:
    bn::sprite_ptr _sprite;
};

// 슬롯 3줄(with_new면 맨 위에 "NEW GAME" 한 줄 더)을 그리고, 고른 줄만 위아래로 흔든다.
// 줄 번호는 with_new일 때 0 = NEW GAME, 1~3 = 슬롯 1~3, 아니면 0~2 = 슬롯 1~3.
class slot_list
{

public:
    // with_slots가 false면 슬롯 줄은 만들지 않는다 (HARD는 저장할 수 없다)
    slot_list(bn::sprite_text_generator& center_generator, int top_y, int step_y, bool with_new = false, bool with_slots = true) :
        _generator(center_generator),
        _top_y(top_y),
        _step_y(step_y),
        _first_slot_line(with_new ? 1 : 0),
        _with_slots(with_slots)
    {
        refresh();
    }

    [[nodiscard]] int line_count() const { return _first_slot_line + (_with_slots ? hs::save::slot_count : 0); }
    [[nodiscard]] int line_width(int line) const { return _widths[line]; }       // 화살표 위치용 글자 폭

    void refresh()
    {
        if(_first_slot_line > 0)
        {
            _lines[0].clear();
            _widths[0] = _generator.width("NEW GAME");
            _generator.generate(0, _top_y, "NEW GAME", _lines[0]);
        }

        for(int slot = 0; _with_slots && slot < hs::save::slot_count; ++slot)
        {
            bn::ivector<bn::sprite_ptr>& line = _lines[_first_slot_line + slot];
            line.clear();
            const bn::string<48> label = slot_label(slot);
            _widths[_first_slot_line + slot] = _generator.width(label);
            _generator.generate(0, _top_y + (_first_slot_line + slot) * _step_y, label, line);
        }
    }

    void set_visible(bool visible)
    {
        for(bn::ivector<bn::sprite_ptr>& sprites : _lines)
        {
            for(bn::sprite_ptr& sprite : sprites)
            {
                sprite.set_visible(visible);
            }
        }
    }

    void animate(int selected, int frame)
    {
        for(int line = 0; line < line_count(); ++line)
        {
            int offset = line == selected && (frame / 8) % 2 == 1 ? -2 : 0;

            for(bn::sprite_ptr& sprite : _lines[line])
            {
                sprite.set_y(_top_y + line * _step_y + offset);
            }
        }
    }

private:
    bn::sprite_text_generator& _generator;
    int _top_y;
    int _step_y;
    int _first_slot_line;
    bool _with_slots;
    int _widths[1 + hs::save::slot_count] = {};
    bn::vector<bn::sprite_ptr, 40> _lines[1 + hs::save::slot_count];
};

// ─── 일시정지 메뉴 (START) ───────────────────────────────────────────

enum class pause_choice
{
    RESUME,
    QUIT,
};

// PAUSE 글자는 고정, Resume / Save / Quit은 ↑↓로 고르고 고른 줄이 위아래로 흔들린다.
// Save는 슬롯 3개 중 하나를 골라 저장한다. 이미 저장된 슬롯은 덮어쓸지 한 번 묻는다.
pause_choice run_pause(const hs::economy& eco)
{
    const bool hard = eco.diff() == hs::difficulty::HARD;      // HARD는 저장할 수 없다 (Save가 회색)
    constexpr int item_count = 3;
    constexpr const char* item_names[item_count] = { "Resume", "Save", "Quit" };
    constexpr int title_y = -44;
    constexpr int top_y = -8;
    constexpr int step_y = 20;
    constexpr const char* default_hint = "A 선택  B 돌아가기";

    // 강화 화면 배경을 흑백으로 어둡게 깔고, 글자는 검은 외곽선이 있는 폰트(hs::font_outline)로 쓴다
    bn::regular_bg_ptr background = forge_bg_item(hs::current_time_band()).create_bg(title_bg_x, title_bg_y);
    bn::bg_palette_ptr background_palette = background.palette();
    background_palette.set_grayscale_intensity(1);
    background_palette.set_fade(bn::color(0, 0, 0), bn::fixed(0.3));

    bn::sprite_ptr anvil = bn::sprite_items::anvil.create_sprite(0, anvil_y);
    anvil.set_z_order(1);
    bn::sprite_palette_ptr anvil_palette = anvil.palette();
    anvil_palette.set_grayscale_intensity(1);
    anvil_palette.set_fade(bn::color(0, 0, 0), bn::fixed(0.3));

    bn::sprite_text_generator center_generator(hs::font_outline);
    center_generator.set_center_alignment();

    bn::sprite_text_generator left_generator(hs::font_outline);
    left_generator.set_left_alignment();

    bn::vector<bn::sprite_ptr, 8> title_sprites;
    center_generator.generate(0, title_y, "PAUSE", title_sprites);

    bn::vector<bn::sprite_ptr, 12> item_sprites[item_count];

    for(int index = 0; index < item_count; ++index)
    {
        center_generator.generate(0, top_y + index * step_y, item_names[index], item_sprites[index]);
    }

    // HARD: Save 줄을 회색으로, PAUSE 아래에 "HARD - 저장 불가" 안내
    bn::optional<bn::sprite_palette_ptr> gray_palette;
    bn::optional<bn::sprite_palette_ptr> red_palette;
    bn::vector<bn::sprite_ptr, 16> hard_label_sprites;

    if(hard)
    {
        gray_palette = create_tinted_font_palette(bn::color(14, 14, 14));
        red_palette = create_tinted_font_palette(bn::color(31, 7, 7));
        set_sprites_palette(item_sprites[1], *gray_palette);
        center_generator.generate(0, title_y + 17, "HARD - 저장 불가", hard_label_sprites);
        set_sprites_palette(hard_label_sprites, *red_palette);      // 저장 불가는 빨간 글씨
    }

    slot_list slots(center_generator, top_y, step_y);
    slots.set_visible(false);
    text_line footer_text(left_generator, -114, 68);
    menu_arrow arrow;

    bool slot_screen = false;
    bool confirming = false;     // 덮어쓰기 확인 중
    bool quit_confirming = false;       // 나가기 확인 중 (저장하지 않은 진행은 사라진다)
    int selected = 0;
    int slot = 0;
    int frame = 0;
    int notice = 0;

    auto show_items = [&](bool visible)
    {
        for(bn::ivector<bn::sprite_ptr>& sprites : item_sprites)
        {
            for(bn::sprite_ptr& sprite : sprites)
            {
                sprite.set_visible(visible);
            }
        }
    };

    auto animate_items = [&]()
    {
        for(int index = 0; index < item_count; ++index)
        {
            int offset = index == selected && (frame / 8) % 2 == 1 ? -2 : 0;

            for(bn::sprite_ptr& sprite : item_sprites[index])
            {
                sprite.set_y(top_y + index * step_y + offset);
            }
        }
    };

    auto save_to_slot = [&]()
    {
        hs::save::write(slot, eco);
        slots.refresh();
        footer_text.set_text("저장했다!");
        notice = notice_frames;
    };

    footer_text.set_text(default_hint);
    fade_screen(true);

    while(true)
    {
        ++frame;

        // 화살표는 고른 줄 왼쪽에 고정 (글자만 흔들린다)
        if(slot_screen && ! quit_confirming)
        {
            arrow.show(slots.line_width(slot), top_y + slot * step_y);
        }
        else
        {
            arrow.show(center_generator.width(item_names[selected]), top_y + selected * step_y);
        }

        if(quit_confirming)
        {
            if(bn::keypad::a_pressed())
            {
                play_select();
                fade_screen(false);
                return pause_choice::QUIT;
            }
            else if(bn::keypad::b_pressed() || bn::keypad::start_pressed())
            {
                quit_confirming = false;
                footer_text.set_text(default_hint);
            }

            animate_items();
        }
        else if(! slot_screen)
        {
            if(bn::keypad::up_pressed())
            {
                selected = (selected + item_count - 1) % item_count;
                play_menu_move();
                frame = 8;
            }
            else if(bn::keypad::down_pressed())
            {
                selected = (selected + 1) % item_count;
                play_menu_move();
                frame = 8;
            }
            else if(bn::keypad::start_pressed() || bn::keypad::b_pressed())
            {
                fade_screen(false);
                return pause_choice::RESUME;
            }
            else if(bn::keypad::a_pressed())
            {
                play_select();
                if(selected == 0)
                {
                    fade_screen(false);
                    return pause_choice::RESUME;
                }

                if(selected == 1)
                {
                    if(hard)
                    {
                        footer_text.set_text("HARD 모드는 저장할 수 없다");
                        notice = notice_frames;
                    }
                    else
                    {
                        slot_screen = true;
                        slot = 0;
                        frame = 8;
                        show_items(false);
                        slots.set_visible(true);
                    }
                }
                else
                {
                    quit_confirming = true;
                    footer_text.set_text("정말 나갈까요?  A 예  B 아니오");
                }
            }

            if(notice > 0 && --notice == 0)
            {
                footer_text.set_text(default_hint);
            }

            animate_items();
        }
        else if(confirming)
        {
            if(bn::keypad::a_pressed())
            {
                play_select();
                confirming = false;
                save_to_slot();
            }
            else if(bn::keypad::b_pressed())
            {
                confirming = false;
                footer_text.set_text(default_hint);
            }

            slots.animate(slot, frame);
        }
        else
        {
            if(bn::keypad::up_pressed())
            {
                slot = (slot + hs::save::slot_count - 1) % hs::save::slot_count;
                play_menu_move();
                frame = 8;
            }
            else if(bn::keypad::down_pressed())
            {
                slot = (slot + 1) % hs::save::slot_count;
                play_menu_move();
                frame = 8;
            }
            else if(bn::keypad::b_pressed())
            {
                slot_screen = false;
                slots.set_visible(false);
                show_items(true);
                footer_text.set_text(default_hint);
                notice = 0;
            }
            else if(bn::keypad::a_pressed())
            {
                play_select();
                if(hs::save::info(slot).used)
                {
                    confirming = true;
                    footer_text.set_text("덮어쓸까요?  A 예  B 아니오");
                    notice = 0;
                }
                else
                {
                    save_to_slot();
                }
            }

            if(notice > 0 && --notice == 0)
            {
                footer_text.set_text(default_hint);
            }

            slots.animate(slot, frame);
        }

        bn::core::update();
    }
}

// ─── 시작 화면: NEW GAME / 저장 슬롯 (타이틀에서 STORY·HARD를 고른 뒤) ────

enum class start_result
{
    NEW_GAME,   // 새로 시작 (STORY는 오프닝 컷씬부터)
    LOADED,     // 슬롯을 불러옴 (바로 강화 화면으로)
    BACK,       // 타이틀로
};

// 맨 위 NEW GAME과 슬롯 3개를 보여 준다. 슬롯을 고르면 eco에 불러오고 seed를 새로 정한다.
start_result run_start_select(hs::economy& eco, unsigned& seed)
{
    constexpr int title_y = -54;
    constexpr int top_y = -30;
    constexpr int step_y = 18;
    constexpr const char* default_hint = "A 선택  B 돌아가기";

    bn::sprite_text_generator center_generator(hs::font);
    center_generator.set_center_alignment();

    bn::sprite_text_generator left_generator(hs::font);
    left_generator.set_left_alignment();

    bn::vector<bn::sprite_ptr, 8> title_sprites;
    center_generator.generate(0, title_y, "START", title_sprites);

    // HARD는 저장할 수 없다: NEW GAME만 두고 슬롯 칸에는 안내 문구를 회색으로 보여 준다
    const bool hard = eco.diff() == hs::difficulty::HARD;
    slot_list list(center_generator, top_y, step_y, true, ! hard);
    text_line footer_text(left_generator, -114, 68);

    menu_arrow arrow;
    bn::optional<bn::sprite_palette_ptr> gray_palette;
    bn::vector<bn::sprite_ptr, 24> hard_notice_sprites;

    if(hard)
    {
        gray_palette = create_tinted_font_palette(bn::color(31, 7, 7));      // 빨간 글씨로 강조
        center_generator.generate(0, top_y + step_y + 18, "HARD 모드는 저장 할 수 없습니다.", hard_notice_sprites);
        set_sprites_palette(hard_notice_sprites, *gray_palette);
    }

    int selected = 0;
    unsigned frame = 0;
    int notice = 0;

    footer_text.set_text(default_hint);
    fade_screen(true);

    while(true)
    {
        ++frame;

        if(bn::keypad::up_pressed())
        {
            selected = (selected + list.line_count() - 1) % list.line_count();
            play_menu_move();
            frame = 8;
        }
        else if(bn::keypad::down_pressed())
        {
            selected = (selected + 1) % list.line_count();
            play_menu_move();
            frame = 8;
        }
        else if(bn::keypad::b_pressed())
        {
            fade_screen(false);
            return start_result::BACK;
        }
        else if(bn::keypad::a_pressed())
        {
            play_select();
            if(selected == 0)
            {
                fade_screen(false);
                return start_result::NEW_GAME;
            }

            if(hs::save::load(selected - 1, eco))
            {
                seed = frame;
                fade_screen(false);
                return start_result::LOADED;
            }

            footer_text.set_text("비어 있는 슬롯이다");
            notice = notice_frames;
        }

        if(notice > 0 && --notice == 0)
        {
            footer_text.set_text(default_hint);
        }

        arrow.show(list.line_width(selected), top_y + selected * step_y);
        list.animate(selected, int(frame));
        bn::core::update();
    }
}

// ─── 게임 오버 (파산) ────────────────────────────────────────────────

// 아무 버튼이나 누르면 돌아간다 (타이틀로). 저장 슬롯은 건드리지 않는다.
void run_game_over()
{
    bn::sprite_text_generator center_generator(hs::font);
    center_generator.set_center_alignment();

    // 그림(assets/graphics/gameover → tools/make_gameover.py)에 GAME OVER 간판이 그려져 있어 글자는 안내 한 줄뿐
    bn::regular_bg_ptr background = bn::regular_bg_items::gameover_bg.create_bg(title_bg_x, title_bg_y);
    text_line prompt_text(center_generator, 0, 68);

    fade_screen(true);

    int frame = 0;

    while(true)
    {
        ++frame;

        // 결과를 보기도 전에 넘어가지 않게 잠깐은 입력을 받지 않는다
        if(frame > 45)
        {
            prompt_text.set_text((frame / 30) % 2 == 0 ? "아무 버튼이나 누르세요" : "");

            if(bn::keypad::any_pressed())
            {
                break;
            }
        }

        bn::core::update();
    }

    fade_screen(false);
}

// ─── 오프닝 컷씬 ─────────────────────────────────────────────────────
// 그림은 assets/graphics/cutscene/ → tools/make_cutscenes.py가 graphics/로 변환 (240x112, 위쪽)
// 아래 48줄은 텍스트 박스: 화자 1줄 + 대사 2줄. 대사는 자동 줄바꿈하고 2줄을 넘으면 assert.

struct dialogue_line
{
    const char* speaker;
    const char* text;
};

struct cutscene_scene
{
    const bn::regular_bg_item& bg;
    bn::span<const dialogue_line> lines;
};

constexpr dialogue_line scene1_lines[] = {
    { "아버지", "우리 가문은... 시공간을 넘나들던 전설의 대장장이 집안이었다." },
    { "아버지", "어떻게든... 이름을 떨쳐라. 가문의 명예든, 부자든... 뭐든 좋다." },
};

constexpr dialogue_line scene2_lines[] = {
    { "아버지", "이 모루와 함께라면..." },
};

constexpr dialogue_line scene3_lines[] = {
    { "스미스", "아버지...!!" },
};

constexpr dialogue_line scene4_lines[] = {
    { "징수관", "대장장이 스미스! 아버지 몫까지 밀린 세금을 받으러 왔다." },
    { "징수관", "세금은 대물림되는 법이지. 이번 달 안에 못 내면 압류다!" },
    { "스미스", "아버지 장례도 치르기 전에... 너무하십니다." },
};

constexpr dialogue_line scene5_lines[] = {
    { "포고문", "왕의 포고. 화염에 달군 검을 바치는 자에게 평생 세금을 면제한다." },
    { "스미스", "화염에 달군 검...! 이거다. 아버지가 말한 길이!" },
};

constexpr dialogue_line scene6_lines[] = {
    { "스미스", "남은 건 이 낡은 단검 하나뿐인가..." },
    { "스미스", "...아니, 하나면 충분해. 화염에 달군 검으로 키워 주마." },
};

constexpr dialogue_line scene7_lines[] = {
    { "스미스", "아버지의 소원을 이루겠다." },
    { "스미스", "기필코... 반드시." },
};

constexpr cutscene_scene opening_scenes[] = {
    { bn::regular_bg_items::scene1_bedside, scene1_lines },
    { bn::regular_bg_items::scene2_anvil, scene2_lines },
    { bn::regular_bg_items::scene3_father_close, scene3_lines },
    { bn::regular_bg_items::scene4_tax_collector, scene4_lines },
    { bn::regular_bg_items::scene5_decree, scene5_lines },
    { bn::regular_bg_items::scene6_dagger, scene6_lines },
    { bn::regular_bg_items::scene7_forge_door, scene7_lines },
};

constexpr int textbox_x = -114;
constexpr int textbox_width = 228;
constexpr int indicator_width = 16;                                     // 둘째 줄 오른쪽 끝의 ▼ 자리
constexpr int second_line_width = textbox_width - indicator_width;
constexpr int indicator_x = textbox_x + textbox_width - 12;
constexpr int indicator_blink_frames = 20;
constexpr int speaker_y = 41;
constexpr int first_line_y = 56;
constexpr int second_line_y = 71;

using dialogue_string = bn::string<160>;

// 공백 단위로 끊어 두 줄로 나눈다 (둘째 줄은 ▼ 자리만큼 짧게)
void wrap_dialogue(const bn::sprite_text_generator& generator, const char* text,
                   dialogue_string& first, dialogue_string& second)
{
    dialogue_string* line = &first;
    bn::string_view rest(text);

    while(! rest.empty())
    {
        int space = 0;

        while(space < rest.size() && rest[space] != ' ')
        {
            ++space;
        }

        bn::string_view word = rest.substr(0, space);
        rest = space < rest.size() ? rest.substr(space + 1) : bn::string_view();

        dialogue_string candidate = *line;

        if(! candidate.empty())
        {
            candidate.push_back(' ');
        }

        candidate.append(word);

        int limit = line == &first ? textbox_width : second_line_width;

        if(line->empty() || generator.width(candidate) <= limit)
        {
            *line = candidate;
        }
        else
        {
            BN_ASSERT(line == &first, "Dialogue too long: ", text);
            line = &second;
            line->append(word);
        }
    }

    BN_ASSERT(generator.width(first) <= textbox_width && generator.width(second) <= second_line_width,
              "Dialogue too long: ", text);
}

// 컷씬 틀: 위쪽 그림 + 아래쪽 대사 박스. 장면마다 A로 대사를 넘기고 장면이 끝나면 페이드. 음악은 없으면 nullptr (마지막 페이드와 함께 끈다)
void run_cutscene(bn::span<const cutscene_scene> scenes, const bn::music_item* music)
{
    bn::sprite_text_generator left_generator(hs::font);
    left_generator.set_left_alignment();

    bn::regular_bg_ptr textbox_bg = bn::regular_bg_items::cutscene_textbox.create_bg(title_bg_x, title_bg_y);

    // 화자 이름은 노란빛으로 구분한다
    bn::sprite_palette_ptr speaker_palette = create_private_font_palette();
    speaker_palette.set_fade(bn::color(31, 24, 8), bn::fixed(0.5));

    bn::vector<bn::sprite_ptr, 8> speaker_sprites;
    text_line first_text(left_generator, textbox_x, first_line_y);
    text_line second_text(left_generator, textbox_x, second_line_y);

    // A를 누르라는 깜빡이는 ▼
    bn::vector<bn::sprite_ptr, 1> indicator_sprites;
    left_generator.generate(indicator_x, second_line_y, "▼", indicator_sprites);
    bn::sprite_ptr& indicator = indicator_sprites[0];
    indicator.set_visible(false);

    // 컷씬 음악 (assets/audio/story_chiptune_sad.wav): 컷씬이 끝나 인게임으로 넘어갈 때까지 반복, 마지막 장면의 페이드와 함께 끈다
    if(music)
    {
        music->play(1);
    }

    int scene_number = 0;

    for(const cutscene_scene& scene : scenes)
    {
        ++scene_number;
        bn::regular_bg_ptr scene_bg = scene.bg.create_bg(title_bg_x, title_bg_y);

        // 그림의 팔레트 0번(투명)은 backdrop으로 보이므로 같은 색으로 맞춘다
        bn::bg_palettes::set_transparent_color(scene.bg.palette_item().colors_ref()[0]);

        for(int index = 0; index < scene.lines.size(); ++index)
        {
            const dialogue_line& line = scene.lines[index];
            dialogue_string first;
            dialogue_string second;
            wrap_dialogue(left_generator, line.text, first, second);

            speaker_sprites.clear();
            left_generator.generate(textbox_x, speaker_y, line.speaker, speaker_sprites);
            set_sprites_palette(speaker_sprites, speaker_palette);
            first_text.set_text(first);
            second_text.set_text(second);

            if(index == 0)
            {
                fade_screen(true);
            }

            int frame = 0;

            do
            {
                indicator.set_visible((frame / indicator_blink_frames) % 2 == 0);
                ++frame;
                bn::core::update();
            }
            while(! bn::keypad::a_pressed());

            indicator.set_visible(false);
        }

        fade_screen(false, music && scene_number == scenes.size());
    }

    if(music)
    {
        bn::music::stop();
    }
}

void run_opening_cutscene()
{
    run_cutscene(opening_scenes, &bn::music_items::story_chiptune_sad);
}

// 파산 컷: 스미스의 독백 한 줄 (그림 scene_bankrupt)
constexpr dialogue_line bankrupt_lines[] = {
    { "스미스", "돈이 다 떨어졌다..." },
};

constexpr cutscene_scene bankrupt_scenes[] = {
    { bn::regular_bg_items::scene_bankrupt, bankrupt_lines },
};

void run_bankrupt_cutscene()
{
    run_cutscene(bankrupt_scenes, nullptr);
}

// 엔딩 (+29): 컷씬 틀에 (모드 × 시간대) 6종. 엔딩 그림·시나리오는 아직 없어 그림은 오프닝 장면을 빌려 쓰고 문구는 임시다.
// 나중에 그림(ending_background)과 문구(ending_lines)만 바꾼다. 번호 = 모드*3 + 시간대 (모드 0 STORY / 1 HARD, 시간대 0 낮 / 1 노을 / 2 밤).
constexpr dialogue_line ending_lines[6] = {
    { "엔딩", "아침 엔딩입니다. (임시 문구)" },
    { "엔딩", "노을 엔딩입니다. (임시 문구)" },
    { "엔딩", "밤 엔딩입니다. (임시 문구)" },
    { "엔딩", "HARD 아침 엔딩입니다. (임시 문구)" },
    { "엔딩", "HARD 노을 엔딩입니다. (임시 문구)" },
    { "엔딩", "HARD 밤 엔딩입니다. (임시 문구)" },
};
constexpr dialogue_line ending_unlock_line = { "알림", "HARD 모드가 해금되었습니다." };

const bn::regular_bg_item& ending_background(int index)
{
    switch(index)
    {
    case 0: return bn::regular_bg_items::scene7_forge_door;
    case 1: return bn::regular_bg_items::scene4_tax_collector;
    case 2: return bn::regular_bg_items::scene5_decree;
    case 3: return bn::regular_bg_items::scene6_dagger;
    case 4: return bn::regular_bg_items::scene2_anvil;
    default: return bn::regular_bg_items::scene_bankrupt;
    }
}

void run_ending(hs::difficulty diff)
{
    const bool story = diff != hs::difficulty::HARD;
    const int index = (story ? 0 : 3) + int(hs::current_time_band());

    // 해금 안내는 STORY 엔딩에서 처음 해금될 때만 (디버그 빌드는 항상 열려 있으니 확인용으로 STORY에서는 늘 보여 준다)
    bool announce_unlock = story && ! hs::save::hard_unlocked();
#ifdef HS_DEBUG
    announce_unlock = story;
#endif

    const dialogue_line lines[2] = { ending_lines[index], ending_unlock_line };
    const cutscene_scene scene = { ending_background(index), bn::span<const dialogue_line>(lines, announce_unlock ? 2 : 1) };
    run_cutscene(bn::span<const cutscene_scene>(&scene, 1), nullptr);

    hs::save::mark_ending(index);       // 엔딩 도감(Gallery)에 체크

    if(story)
    {
        hs::save::unlock_hard();
    }
}

// ─── OPTION (타이틀에서): Record / Gallery / Language ─────────────────────
// 작업대 그림을 흑백으로 어둡게 깔고 글자는 외곽선 폰트로 쓴다. 화면마다 따로 만들고 지운다.

bn::regular_bg_ptr option_background()
{
    return bn::regular_bg_items::option_bg.create_bg(title_bg_x, title_bg_y);       // 작업대 위 검 설계도 (흑백, tools/make_option_bg.py)
}

enum class option_choice
{
    RECORD,
    GALLERY,
    LANGUAGE,
    BACK,
};

option_choice run_option_menu()
{
    constexpr int item_count = 3;
    constexpr const char* item_names[item_count] = { "Record", "Gallery", "Language" };
    constexpr int top_y = -8;
    constexpr int step_y = 20;

    bn::regular_bg_ptr background = option_background();

    bn::sprite_text_generator center_generator(hs::font_outline);
    center_generator.set_center_alignment();
    bn::sprite_text_generator left_generator(hs::font_outline);
    left_generator.set_left_alignment();

    bn::vector<bn::sprite_ptr, 8> title_sprites;
    center_generator.generate(0, -44, "OPTION", title_sprites);

    bn::vector<bn::sprite_ptr, 12> item_sprites[item_count];

    for(int index = 0; index < item_count; ++index)
    {
        center_generator.generate(0, top_y + index * step_y, item_names[index], item_sprites[index]);
    }

    menu_arrow arrow;
    text_line footer_text(left_generator, -114, 70);
    footer_text.set_text("A 선택  B 돌아가기");

    int selected = 0;
    int frame = 0;

    fade_screen(true);

    while(true)
    {
        ++frame;

        if(bn::keypad::up_pressed())
        {
            selected = (selected + item_count - 1) % item_count;
            play_menu_move();
            frame = 8;
        }
        else if(bn::keypad::down_pressed())
        {
            selected = (selected + 1) % item_count;
            play_menu_move();
            frame = 8;
        }
        else if(bn::keypad::b_pressed())
        {
            fade_screen(false);
            return option_choice::BACK;
        }
        else if(bn::keypad::a_pressed())
        {
            play_select();
            fade_screen(false);
            return option_choice(selected);
        }

        arrow.show(center_generator.width(item_names[selected]), top_y + selected * step_y);       // 화살표는 고정

        const int bob = (frame / 8) % 2 == 1 ? -2 : 0;

        for(int index = 0; index < item_count; ++index)
        {
            for(bn::sprite_ptr& sprite : item_sprites[index])
            {
                sprite.set_y(top_y + index * step_y + (index == selected ? bob : 0));
            }
        }

        bn::core::update();
    }
}

// HARD 최고 도달 기록: 새 최고 단계에 닿는 순간 자동으로 남은 값 (그 순간까지의 강화·성공·실패·깨뜨린 검·방지권·번 돈)
// 반환: true면 기록을 지웠다는 뜻이라 화면을 다시 연다 ("아직 기록이 없습니다."로 바뀐다)
bool run_record_screen_once()
{
    bn::regular_bg_ptr background = option_background();

    bn::sprite_text_generator center_generator(hs::font_outline);
    center_generator.set_center_alignment();
    bn::sprite_text_generator left_generator(hs::font_outline);
    left_generator.set_left_alignment();
    bn::sprite_text_generator right_generator(hs::font_outline);
    right_generator.set_right_alignment();
    bn::sprite_text_generator small_generator(hs::font_small);
    small_generator.set_center_alignment();

    bn::sprite_palette_ptr gold = create_tinted_font_palette(bn::color(31, 26, 12));
    bn::sprite_palette_ptr red = create_tinted_font_palette(bn::color(31, 7, 7));
    bn::sprite_palette_ptr gray = create_tinted_font_palette(bn::color(18, 18, 18));
    bn::sprite_palette_ptr light = create_tinted_font_palette(bn::color(25, 25, 25));
    bn::sprite_palette_ptr money_palette = create_two_tone_font_palette(gold_body_color, gold_outline_color);

    bn::vector<bn::sprite_ptr, 12> title_sprites;
    center_generator.generate(0, -66, "RECORD", title_sprites);

    bn::vector<bn::sprite_ptr, 24> subtitle_sprites;
    small_generator.generate(0, -44, "HARD 최고 도달 기록", subtitle_sprites);
    set_sprites_palette(subtitle_sprites, gold);

    const hs::save::best_record best = hs::save::read_best();
    bn::vector<bn::sprite_ptr, 24> level_sprites;
    bn::vector<bn::sprite_ptr, 40> label_sprites;
    bn::vector<bn::sprite_ptr, 40> value_sprites;
    bn::vector<bn::sprite_ptr, 24> money_sprites;
    bn::vector<bn::sprite_ptr, 32> hint_sprites;

    if(best.valid)
    {
        constexpr const char* labels[5] = { "강화", "성공", "실패", "깨뜨린 검", "방지권" };
        const bn::string<24> values[5] = {
            bn::to_string<24>(int(best.stats.upgrades)), bn::to_string<24>(int(best.stats.successes)),
            bn::to_string<24>(int(best.stats.fails)), bn::to_string<24>(int(best.stats.broken)),
            bn::to_string<24>(int(best.stats.tickets_used))
        };

        center_generator.generate(0, -26, sword_label(best.level), level_sprites);
        set_sprites_palette(level_sprites, red);

        for(int index = 0; index < 5; ++index)
        {
            const int column = index % 2;
            const int y = -4 + (index / 2) * 16;        // 줄 간격 16px 균등
            left_generator.generate(-100 + column * 108, y, labels[index], label_sprites);
            right_generator.generate(-4 + column * 108, y, values[index], value_sprites);
        }

        // 최고 소지금: 한 줄을 통째로 쓴다 (가장 많이 가졌던 금액, 시작 자금부터)
        left_generator.generate(-100, 44, "최고 소지금", label_sprites);
        right_generator.generate(104, 44, format_money(int(best.stats.peak_money)), money_sprites);       // 인게임 돈처럼 전체 금액, 노란색
        set_sprites_palette(money_sprites, money_palette);

        set_sprites_palette(label_sprites, light);
    }
    else
    {
        center_generator.generate(0, 2, "아직 기록이 없습니다.", hint_sprites);
    }

    set_sprites_palette(hint_sprites, gray);

    text_line footer_text(left_generator, -114, 70);
    constexpr const char* default_hint = "START 기록 지우기  B 돌아가기";
    footer_text.set_text(best.valid ? default_hint : "B 돌아가기");

    bool confirming = false;
    fade_screen(true);

    while(true)
    {
        if(confirming)
        {
            if(bn::keypad::a_pressed())
            {
                play_select();
                hs::save::clear_best();
                fade_screen(false);
                return true;
            }
            else if(bn::keypad::b_pressed() || bn::keypad::start_pressed())
            {
                confirming = false;
                footer_text.set_text(default_hint);
            }
        }
        else if(bn::keypad::b_pressed())
        {
            break;
        }
        else if(best.valid && bn::keypad::start_pressed())
        {
            play_select();
            confirming = true;
            footer_text.set_text("정말 지울까요?  A 예  B 아니오");
        }

        bn::core::update();
    }

    fade_screen(false);
    return false;
}

void run_record_screen()
{
    while(run_record_screen_once())
    {
        bn::core::update();         // 지운 뒤에는 이전 화면의 타일을 비우고 빈 기록 화면을 다시 연다
    }
}

// 엔딩 도감: STORY / HARD 두 줄 × 낮·노을·밤. 맨 아래 시간대 아이콘이 열 이름이다. 본 엔딩만 그림이 보이고 A로 크게 본다.
void run_gallery_screen()
{
    constexpr int cell_x[3] = { -73, -1, 71 };
    constexpr int row_y[2] = { -22, 22 };

    bn::regular_bg_ptr background = option_background();

    bn::sprite_text_generator center_generator(hs::font_outline);
    center_generator.set_center_alignment();
    bn::sprite_text_generator left_generator(hs::font_outline);
    left_generator.set_left_alignment();

    bn::sprite_palette_ptr gold = create_tinted_font_palette(bn::color(31, 26, 12));
    bn::sprite_palette_ptr red = create_tinted_font_palette(bn::color(31, 7, 7));

    bn::vector<bn::sprite_ptr, 12> title_sprites;
    center_generator.generate(0, -66, "GALLERY", title_sprites);

    bn::vector<bn::sprite_ptr, 8> story_sprites;
    left_generator.generate(-106, -45, "STORY", story_sprites);
    set_sprites_palette(story_sprites, gold);

    bn::vector<bn::sprite_ptr, 8> hard_sprites;
    left_generator.generate(-106, -1, "HARD", hard_sprites);
    set_sprites_palette(hard_sprites, red);

    const bn::sprite_item* thumb_items[6] = {
        &bn::sprite_items::gallery_thumb_0, &bn::sprite_items::gallery_thumb_1, &bn::sprite_items::gallery_thumb_2,
        &bn::sprite_items::gallery_thumb_3, &bn::sprite_items::gallery_thumb_4, &bn::sprite_items::gallery_thumb_5,
    };
    bn::optional<bn::sprite_ptr> cells[6];
    bool seen[6];
    int seen_count = 0;

    for(int index = 0; index < 6; ++index)
    {
        seen[index] = hs::save::ending_seen(index);
        seen_count += seen[index] ? 1 : 0;
        const int x = cell_x[index % 3];
        const int y = row_y[index / 3];
        cells[index] = seen[index] ? thumb_items[index]->create_sprite(x, y) : bn::sprite_items::gallery_locked.create_sprite(x, y);
    }

    bn::sprite_ptr icons[3] = {
        bn::sprite_items::gallery_icon_0.create_sprite(cell_x[0], 53),
        bn::sprite_items::gallery_icon_1.create_sprite(cell_x[1], 53),
        bn::sprite_items::gallery_icon_2.create_sprite(cell_x[2], 53),
    };

    bn::string<64> footer = bn::to_string<8>(seen_count);
    footer += "/6  A 크게 보기  B 돌아가기";
    text_line footer_text(left_generator, -114, 71);
    footer_text.set_text(footer);

    // 보는 동안 도감 화면은 숨겼다가 돌아와서 다시 보인다
    auto set_gallery_visible = [&](bool visible)
    {
        background.set_visible(visible);

        for(bn::sprite_ptr& sprite : title_sprites)
        {
            sprite.set_visible(visible);
        }

        for(bn::sprite_ptr& sprite : story_sprites)
        {
            sprite.set_visible(visible);
        }

        for(bn::sprite_ptr& sprite : hard_sprites)
        {
            sprite.set_visible(visible);
        }

        footer_text.set_visible(visible);

        for(bn::optional<bn::sprite_ptr>& cell : cells)
        {
            cell->set_visible(visible);
        }

        for(bn::sprite_ptr& icon : icons)
        {
            icon.set_visible(visible);
        }
    };

    int selected = 0;
    int styled = -1;
    int frame = 0;

    fade_screen(true);

    while(true)
    {
        ++frame;
        int column = selected % 3;
        int row = selected / 3;

        if(bn::keypad::left_pressed())
        {
            column = (column + 2) % 3;
            play_menu_move();
            frame = 8;
        }
        else if(bn::keypad::right_pressed())
        {
            column = (column + 1) % 3;
            play_menu_move();
            frame = 8;
        }
        else if(bn::keypad::up_pressed() || bn::keypad::down_pressed())
        {
            row = 1 - row;
            play_menu_move();
            frame = 8;
        }
        else if(bn::keypad::b_pressed())
        {
            break;
        }
        else if(bn::keypad::a_pressed() && seen[selected])
        {
            play_select();
            fade_screen(false);
            set_gallery_visible(false);
            const cutscene_scene scene = { ending_background(selected), bn::span<const dialogue_line>(&ending_lines[selected], 1) };
            run_cutscene(bn::span<const cutscene_scene>(&scene, 1), nullptr);
            set_gallery_visible(true);
            fade_screen(true);
        }

        selected = row * 3 + column;

        if(selected != styled)
        {
            styled = selected;

            for(int index = 0; index < 6; ++index)
            {
                if(seen[index])
                {
                    bn::sprite_palette_ptr cell_palette = cells[index]->palette();
                    cell_palette.set_fade(bn::color(0, 0, 0), index == selected ? bn::fixed(0) : bn::fixed(0.4));
                }
            }
        }

        for(int index = 0; index < 6; ++index)
        {
            const int offset = index == selected && (frame / 8) % 2 == 1 ? -2 : 0;
            cells[index]->set_y(row_y[index / 3] + offset);
        }

        bn::core::update();
    }

    fade_screen(false);
}

// 언어: 지금은 한국어만 (영어는 문자열 표 작업 뒤에 연결)
void run_language_screen()
{
    bn::regular_bg_ptr background = option_background();

    bn::sprite_text_generator center_generator(hs::font_outline);
    center_generator.set_center_alignment();
    bn::sprite_text_generator left_generator(hs::font_outline);
    left_generator.set_left_alignment();
    bn::sprite_text_generator small_generator(hs::font_small);
    small_generator.set_center_alignment();

    bn::sprite_palette_ptr gray = create_tinted_font_palette(bn::color(18, 18, 18));

    bn::vector<bn::sprite_ptr, 12> title_sprites;
    center_generator.generate(0, -44, "LANGUAGE", title_sprites);

    bn::vector<bn::sprite_ptr, 12> korean_sprites;
    center_generator.generate(0, -6, "한국어", korean_sprites);

    bn::vector<bn::sprite_ptr, 12> english_sprites;
    center_generator.generate(0, 14, "ENGLISH", english_sprites);
    set_sprites_palette(english_sprites, gray);

    bn::vector<bn::sprite_ptr, 16> soon_sprites;
    small_generator.generate(0, 32, "(준비 중)", soon_sprites);
    set_sprites_palette(soon_sprites, gray);

    menu_arrow arrow;
    arrow.show(center_generator.width("한국어"), -6);

    text_line footer_text(left_generator, -114, 70);
    footer_text.set_text("B 돌아가기");

    fade_screen(true);

    while(! bn::keypad::b_pressed())
    {
        bn::core::update();
    }

    fade_screen(false);
}

void run_option()
{
    while(true)
    {
        const option_choice choice = run_option_menu();
        bn::core::update();         // 이전 화면의 스프라이트 타일을 먼저 비운다

        if(choice == option_choice::BACK)
        {
            return;
        }

        if(choice == option_choice::RECORD)
        {
            run_record_screen();
        }
        else if(choice == option_choice::GALLERY)
        {
            run_gallery_screen();
        }
        else
        {
            run_language_screen();
        }

        bn::core::update();
    }
}

}

// ─── 레시피 (강화 화면 SELECT 도움말에서 START) ─────────────────────────────
// 낡은 종이(tools/make_recipe_paper.py)에 잉크로 적은 단계별 강화표. 지금 모드(STORY·HARD)의 값이 나오고,
// 재료로 내는 단계는 붉은 잉크. ↑↓ 한 줄, L R 한 쪽(8줄)씩 넘기고 B·START로 닫는다.
constexpr int recipe_visible_rows = 8;
constexpr int recipe_row_y = -34;           // 첫 줄 가운데 y (화면 46) — 줄 간격 13
constexpr int recipe_row_step = 13;
constexpr int recipe_col_level = -108;      // 단계 (왼쪽 맞춤): 밑줄 왼쪽 끝
constexpr int recipe_col_need = -62;        // 필요한 것 (왼쪽 맞춤)
constexpr int recipe_col_ticket = 38;       // 방지권 개수 (오른쪽 맞춤, 머리글은 방지권 아이콘)
constexpr int recipe_col_success = 64;      // 성공 (오른쪽 맞춤)
constexpr int recipe_col_sell = 110;        // 판매가 (오른쪽 맞춤): 밑줄 오른쪽 끝

void run_recipe(const hs::economy& eco)
{
    bn::regular_bg_ptr background = bn::regular_bg_items::recipe_paper.create_bg(title_bg_x, title_bg_y);

    bn::sprite_text_generator left_generator(hs::font);
    left_generator.set_left_alignment();
    bn::sprite_text_generator right_generator(hs::font);
    right_generator.set_right_alignment();
    bn::sprite_text_generator small_left_generator(hs::font_small);
    small_left_generator.set_left_alignment();
    // 잉크 색: 짙은 갈색(보통), 붉은 잉크(재료), 번진 옅은 갈색(머리글·안내). 두 번째 색은 글자 그림자(번짐)
    bn::sprite_palette_ptr ink_palette = create_two_tone_font_palette(bn::color(7, 4, 2), bn::color(19, 14, 9));
    bn::sprite_palette_ptr red_palette = create_two_tone_font_palette(bn::color(19, 4, 3), bn::color(23, 15, 9));
    bn::sprite_palette_ptr faint_palette = create_two_tone_font_palette(bn::color(18, 13, 8), bn::color(24, 19, 13));

    bn::vector<bn::sprite_ptr, 32> fixed_sprites;
    auto add_text = [&](bn::sprite_text_generator& generator, int x, int y, const bn::string_view& text,
                        const bn::sprite_palette_ptr& palette, bn::ivector<bn::sprite_ptr>& target)
    {
        bn::vector<bn::sprite_ptr, 12> temp;
        generator.generate(x, y, text, temp);

        for(bn::sprite_ptr& sprite : temp)
        {
            sprite.set_palette(palette);
            target.push_back(sprite);
        }
    };

    // 제목 "레시피", 머리글(단계/필요한 것/성공/판매가), 아래 안내는 스프라이트 타일을 아끼려고 배경 그림에 미리 그려 두었다 (tools/make_recipe_paper.py)
    add_text(left_generator, -62, -67, eco.diff() == hs::difficulty::HARD ? "(HARD)" : "(STORY)", red_palette, fixed_sprites);
    bn::sprite_ptr ticket_header = bn::sprite_items::ico_ticket.create_sprite(recipe_col_ticket - 8, -49);        // 방지권 열 머리글

    const hs::level_row* table = hs::level_table(eco.diff());
    const int total = hs::max_level;                    // +0→1 ~ +28→29
    int offset = eco.level() < total - recipe_visible_rows ? eco.level() : total - recipe_visible_rows;
    int shown_offset = -1;
    int hold = 0;

    // 줄마다 따로 들고 있다가, 한 줄씩 넘길 때는 가려지는 줄 하나만 지우고 새로 보이는 줄 하나만 만든다.
    // (스프라이트 타일은 한 프레임이 지나야 반환되니, 화면 전체를 지우고 같은 프레임에 다시 만들면 타일이 두 배로 필요해 VRAM이 모자란다)
    bn::vector<bn::sprite_ptr, 24> slot_sprites[recipe_visible_rows];
    bn::vector<bn::sprite_ptr, 4> arrow_sprites;

    auto build_row = [&](int slot, int index)
    {
        bn::ivector<bn::sprite_ptr>& target = slot_sprites[slot];
        target.clear();

        const hs::level_row& row = table[index];
        const int y = recipe_row_y + slot * recipe_row_step;
        const bool material = row.cost_sword > 0 || row.cost_material != hs::material::NONE;
        const bn::sprite_palette_ptr& tint = material ? red_palette : ink_palette;

        bn::string<16> level_line = bn::to_string<4>(index);        // "+"는 빼서 필요한 것 칸과 붙어 보이지 않게 한다
        level_line += "→";
        level_line += bn::to_string<4>(index + 1);

        bn::string<32> need_line;

        if(row.cost_sword > 0)
        {
            need_line = "+";
            need_line += bn::to_string<4>(row.cost_sword);
            need_line += "검 x";
            need_line += bn::to_string<4>(row.cost_qty);
        }
        else if(row.cost_material != hs::material::NONE)
        {
            need_line = hs::material_names[int(row.cost_material)];
            need_line += " x";
            need_line += bn::to_string<4>(row.cost_qty);
        }
        else if(row.cost_usd > 0)
        {
            need_line = format_money_short(row.cost_usd);
        }
        else
        {
            need_line = "무료";
        }

        bn::string<8> success_line = bn::to_string<4>(row.success_pct);
        success_line += "%";

        add_text(left_generator, recipe_col_level, y, level_line, tint, target);
        add_text(left_generator, recipe_col_need, y, need_line, tint, target);

        if(row.protect_cost > 0)       // 방지권을 못 쓰는 단계는 칸을 비워 둔다 (글자 조각을 아낀다)
        {
            add_text(right_generator, recipe_col_ticket, y, bn::to_string<8>(row.protect_cost), tint, target);
        }

        add_text(right_generator, recipe_col_success, y, success_line, ink_palette, target);
        add_text(right_generator, recipe_col_sell, y, row.sell_usd > 0 ? format_money_short(row.sell_usd) : bn::string<24>("-"),
                 ink_palette, target);
    };

    auto build_arrows = [&]()
    {
        arrow_sprites.clear();

        if(offset > 0)
        {
            add_text(small_left_generator, 114, -49, "▲", ink_palette, arrow_sprites);
        }

        if(offset + recipe_visible_rows < total)
        {
            add_text(small_left_generator, 114, 58, "▼", ink_palette, arrow_sprites);
        }
    };

    for(int slot = 0; slot < recipe_visible_rows; ++slot)
    {
        build_row(slot, offset + slot);
    }

    build_arrows();
    shown_offset = offset;
    fade_screen(true);

    while(! bn::keypad::b_pressed() && ! bn::keypad::start_pressed())
    {
        const int max_offset = total - recipe_visible_rows;
        int step = 0;

        if(bn::keypad::up_pressed() || (bn::keypad::up_held() && hold > 16 && hold % 4 == 0))
        {
            step = -1;
        }
        else if(bn::keypad::down_pressed() || (bn::keypad::down_held() && hold > 16 && hold % 4 == 0))
        {
            step = 1;
        }
        else if(bn::keypad::l_pressed())
        {
            step = -recipe_visible_rows;
        }
        else if(bn::keypad::r_pressed())
        {
            step = recipe_visible_rows;
        }

        hold = bn::keypad::up_held() || bn::keypad::down_held() ? hold + 1 : 0;
        offset += step;
        offset = offset < 0 ? 0 : offset > max_offset ? max_offset : offset;

        if(offset != shown_offset)
        {
            play_menu_move();
            const int moved = offset - shown_offset;

            if(moved == 1 || moved == -1)
            {
                // 한 줄: 나머지 줄은 그대로 위아래로 밀고, 빠지는 줄을 지운 자리에 새 줄 하나를 만든다
                if(moved == 1)
                {
                    slot_sprites[0].clear();

                    for(int slot = 0; slot < recipe_visible_rows - 1; ++slot)
                    {
                        slot_sprites[slot] = bn::move(slot_sprites[slot + 1]);

                        for(bn::sprite_ptr& sprite : slot_sprites[slot])
                        {
                            sprite.set_y(sprite.y() - recipe_row_step);
                        }
                    }

                    build_row(recipe_visible_rows - 1, offset + recipe_visible_rows - 1);
                }
                else
                {
                    slot_sprites[recipe_visible_rows - 1].clear();

                    for(int slot = recipe_visible_rows - 1; slot > 0; --slot)
                    {
                        slot_sprites[slot] = bn::move(slot_sprites[slot - 1]);

                        for(bn::sprite_ptr& sprite : slot_sprites[slot])
                        {
                            sprite.set_y(sprite.y() + recipe_row_step);
                        }
                    }

                    build_row(0, offset);
                }
            }
            else
            {
                // 한 쪽(8줄) 넘기기: 전부 지우고 한 프레임 쉰 뒤(타일 반환) 다시 만든다
                for(int slot = 0; slot < recipe_visible_rows; ++slot)
                {
                    slot_sprites[slot].clear();
                }

                bn::core::update();

                for(int slot = 0; slot < recipe_visible_rows; ++slot)
                {
                    build_row(slot, offset + slot);
                }
            }

            build_arrows();
            shown_offset = offset;
        }

        bn::core::update();
    }

    fade_screen(false);
}

// ─── 고양이 이벤트 ──────────────────────────────────────────────────────
// 강화 배경 위에 창이 열리고, 창 안으로 도트 고양이가 걸어 들어와 앉아 꼬리를 흔든 뒤, 같은 창에 고양이 사진이 나오며 야옹.
// 창 아래에 "물어왔다" → 받은 것(검 또는 방지권) 메시지. 장면은 A로 넘긴다(깜빡이는 ▼). 그림 위치는 tools/make_cat_assets.py의 WINDOW_POS(77, 41)와 맞춘다.
constexpr int cat_interior_left = 80 + 16 - 120;        // 방 안쪽 왼쪽 끝에 놓은 고양이 가운데 x
constexpr int cat_sit_x = 0;
constexpr int cat_y = 4;                                // 고양이 가운데 y (발이 방 바닥 위에 닿는다)
constexpr int cat_walk_start = 24;                      // 고양이가 나타나 걷기 시작하는 프레임
constexpr int cat_walk_frames = 44;
constexpr int cat_message_delay = 14;                   // 사진이 나온 뒤 첫 메시지까지
constexpr int cat_message_y = 44;                       // 창 아래 두 줄 메시지 (가운데 y와 줄 간격)
constexpr int cat_message_step = 14;

void run_cat_event(hs::economy& eco, bn::seed_random& random)
{
    const hs::cat_gift gift = eco.cat_visit(random);

    bn::sprite_text_generator center_generator(hs::font_outline);
    center_generator.set_center_alignment();

    bn::regular_bg_ptr background = forge_bg_item(hs::current_time_band()).create_bg(title_bg_x, title_bg_y);
    bn::regular_bg_ptr room = bn::regular_bg_items::cat_room.create_bg(title_bg_x, title_bg_y);
    bn::regular_bg_ptr photo = bn::regular_bg_items::cat_photo.create_bg(title_bg_x, title_bg_y);
    room.set_priority(1);
    photo.set_priority(1);
    room.set_visible(false);
    photo.set_visible(false);

    bn::sprite_ptr anvil = bn::sprite_items::anvil.create_sprite(0, anvil_y);
    anvil.set_bg_priority(2);               // 창(배경 우선순위 1) 뒤로

    bn::sprite_ptr cat = bn::sprite_items::cat.create_sprite(cat_interior_left, cat_y, 0);
    cat.set_bg_priority(0);                 // 창 앞으로
    cat.set_visible(false);
    cat.set_blending_enabled(true);

    bn::vector<bn::sprite_ptr, 40> message_sprites;

    auto show_message = [&](const bn::string_view& first, const bn::string_view& second)
    {
        message_sprites.clear();
        center_generator.generate(0, cat_message_y, first, message_sprites);
        center_generator.generate(0, cat_message_y + cat_message_step, second, message_sprites);

        // 모루 그림(배경 우선순위 2)에 글자가 가려지지 않게 맨 앞으로
        for(bn::sprite_ptr& sprite : message_sprites)
        {
            sprite.set_bg_priority(0);
        }
    };

    auto set_flash = [&](bn::fixed level)
    {
        bn::bg_palettes::set_fade(bn::color(31, 31, 31), level);
        bn::sprite_palettes::set_fade(bn::color(31, 31, 31), level);
    };

    bn::music_items::bgm_event_cat.play(bn::fixed(0.9));
    fade_screen(true);

    // 컷씬 대화처럼 깜빡이는 ▼ 로 버튼을 기다린다 (A로 다음 장면). 음악은 계속 반복된다.
    bn::sprite_text_generator arrow_generator(hs::font);        // 컷씬 대화의 ▼ 와 같은 글꼴·위치 (오른쪽 아래 구석)
    arrow_generator.set_left_alignment();
    bn::vector<bn::sprite_ptr, 1> arrow;
    arrow_generator.generate(indicator_x, second_line_y, "▼", arrow);
    arrow[0].set_bg_priority(0);
    arrow[0].set_visible(false);

    // 장면: 0 고양이가 걸어와 앉아 꼬리를 흔든다 → 1 사진이 나오고 "물어왔다" → 2 받은 것 → 끝
    int page = 0;
    int page_frame = 0;         // 지금 장면에 들어온 뒤 프레임
    int photo_countdown = -1;   // A를 누르면 야옹을 먼저 틀고, 소리가 들리는 때(3프레임 뒤)에 사진이 나온다
    int flash = 0;
    int frame = 0;

    while(true)
    {
        if(frame == 0)
        {
            room.set_visible(true);
            bn::sound_items::sfx_event.play();      // 이벤트 발생 알림 (다른 이벤트도 같이 쓴다)
            flash = 8;
        }

        bool ready = false;

        if(page == 0 && frame >= cat_walk_start)
        {
            const int u = frame - cat_walk_start;
            cat.set_visible(true);
            bn::blending::set_transparency_alpha(u >= 12 ? bn::fixed(1) : bn::fixed(u) / 12);       // 왼쪽에서 스르륵 나타난다

            if(u < cat_walk_frames)
            {
                cat.set_x(cat_interior_left + (cat_sit_x - cat_interior_left) * u / cat_walk_frames);
                cat.set_item(bn::sprite_items::cat, (u / 5) % 4);
            }
            else
            {
                const int sit_time = u - cat_walk_frames;
                cat.set_x(cat_sit_x);
                cat.set_item(bn::sprite_items::cat, 4 + (sit_time < 10 ? 1 : (sit_time / 8) % 3));       // 꼬리 흔들기는 계속 반복
                ready = sit_time >= 24 && photo_countdown < 0;
            }
        }
        else if(page == 1)
        {
            if(photo_countdown > 0)
            {
                --photo_countdown;
            }
            else if(photo_countdown == 0)
            {
                photo_countdown = -1;
                room.set_visible(false);
                cat.set_visible(false);
                photo.set_visible(true);
                flash = 8;
                page_frame = 0;
            }

            if(photo_countdown < 0 && photo.visible())
            {
                if(page_frame == cat_message_delay)
                {
                    show_message("귀여운 고양이가", "뭔가를 물어왔다!");
                }

                ready = page_frame >= cat_message_delay + 12;
            }
        }
        else
        {
            ready = page_frame >= 12;
        }

        arrow[0].set_visible(ready && (frame / indicator_blink_frames) % 2 == 0);

        if(ready && bn::keypad::a_pressed())
        {
            if(page == 0)
            {
                page = 1;
                page_frame = 0;
                photo_countdown = sfx_latency_frames;       // 소리는 3프레임 늦게 들리니 미리 튼다
                bn::sound_items::sfx_cat.play();
            }
            else if(page == 1)
            {
                page = 2;
                page_frame = 0;

                if(gift.is_sword)
                {
                    show_message(sword_label(gift.level), "창고로 들어갔다");
                }
                else
                {
                    bn::string<32> first("방지권 ");
                    first += bn::to_string<4>(gift.tickets);
                    first += "장을";
                    show_message(first, "받았다");
                }
            }
            else
            {
                break;
            }
        }

        if(flash > 0)
        {
            set_flash(bn::fixed(flash) / 14);
            --flash;

            if(flash == 0)
            {
                set_flash(0);
            }
        }

        ++frame;
        ++page_frame;
        bn::core::update();
    }

    set_flash(0);
    fade_screen(false, true);
    bn::music::stop();
    bn::blending::set_transparency_alpha(1);
}

// ─── 도둑 이벤트 (밤) ───────────────────────────────────────────────────────
// 밤 장면 창이 열리고 도트 도둑이 살금살금 들어와 받침대의 검(또는 금화)을 집으면 → A → 같은 창에 도둑 정면 사진이
// 번쩍이며 "히히히" → 도난 메시지. 흐름은 고양이 이벤트와 같다 (그림 위치는 tools/make_thief_assets.py의 창 (77, 41)).
constexpr int thief_left_x = 3 - 27;                    // 방 안쪽 왼쪽 끝에서 시작: 창 안 좌표 3 → 가운데 x = 3 + 16 + 77 - 120
constexpr int thief_stop_x = 34 - 27;                   // 받침대 앞에서 멈춘다
constexpr int thief_y = 2;                              // 가운데 y (발이 바닥에 닿는다)
constexpr int thief_walk_start = 24;
constexpr int thief_walk_frames = 40;
constexpr int thief_peek_frames = 26;
constexpr int thief_ready_after_grab = 24;
constexpr int thief_message_delay = 14;

void run_thief_event(hs::economy& eco, bn::seed_random& random)
{
    const hs::thief_loss loss = eco.steal(random);

    bn::sprite_text_generator center_generator(hs::font_outline);
    center_generator.set_center_alignment();

    bn::regular_bg_ptr background = forge_bg_item(hs::current_time_band()).create_bg(title_bg_x, title_bg_y);
    bn::regular_bg_ptr room_loot = (loss.is_sword ? bn::regular_bg_items::thief_room : bn::regular_bg_items::thief_room_coins)
                                       .create_bg(title_bg_x, title_bg_y);
    bn::regular_bg_ptr room_empty = bn::regular_bg_items::thief_room_empty.create_bg(title_bg_x, title_bg_y);
    bn::regular_bg_ptr photo = bn::regular_bg_items::thief_photo.create_bg(title_bg_x, title_bg_y);
    room_loot.set_priority(1);
    room_empty.set_priority(1);
    photo.set_priority(1);
    room_loot.set_visible(false);
    room_empty.set_visible(false);
    photo.set_visible(false);

    bn::sprite_ptr anvil = bn::sprite_items::anvil.create_sprite(0, anvil_y);
    anvil.set_bg_priority(2);

    bn::sprite_ptr thief = bn::sprite_items::thief.create_sprite(thief_left_x, thief_y, 0);
    thief.set_bg_priority(0);
    thief.set_visible(false);
    thief.set_blending_enabled(true);

    bn::vector<bn::sprite_ptr, 40> message_sprites;

    auto show_message = [&](const bn::string_view& first, const bn::string_view& second)
    {
        message_sprites.clear();
        center_generator.generate(0, cat_message_y, first, message_sprites);
        center_generator.generate(0, cat_message_y + cat_message_step, second, message_sprites);

        for(bn::sprite_ptr& sprite : message_sprites)
        {
            sprite.set_bg_priority(0);
        }
    };

    auto set_flash = [&](bn::fixed level)
    {
        bn::bg_palettes::set_fade(bn::color(31, 31, 31), level);
        bn::sprite_palettes::set_fade(bn::color(31, 31, 31), level);
    };

    bn::music_items::bgm_event_thief.play(bn::fixed(0.9));
    fade_screen(true);

    bn::sprite_text_generator arrow_generator(hs::font);
    arrow_generator.set_left_alignment();
    bn::vector<bn::sprite_ptr, 1> arrow;
    arrow_generator.generate(indicator_x, second_line_y, "▼", arrow);
    arrow[0].set_bg_priority(0);
    arrow[0].set_visible(false);

    const int grab_frame = thief_walk_start + thief_walk_frames + thief_peek_frames;
    int page = 0;
    int page_frame = 0;
    int photo_countdown = -1;
    int flash = 0;
    int frame = 0;

    while(true)
    {
        if(frame == 0)
        {
            room_loot.set_visible(true);
            bn::sound_items::sfx_event.play();      // 이벤트 발생 알림 (고양이 이벤트와 같은 공용 소리)
            flash = 8;
        }

        bool ready = false;

        if(page == 0 && frame >= thief_walk_start)
        {
            const int u = frame - thief_walk_start;
            thief.set_visible(true);
            bn::blending::set_transparency_alpha(u >= 12 ? bn::fixed(1) : bn::fixed(u) / 12);

            if(u < thief_walk_frames)
            {
                thief.set_x(thief_left_x + (thief_stop_x - thief_left_x) * u / thief_walk_frames);
                thief.set_item(bn::sprite_items::thief, (u / 6) % 4);
            }
            else if(frame < grab_frame)
            {
                // 두리번거린다: 두 걸음 자세를 번갈아
                thief.set_x(thief_stop_x + ((frame / 8) % 2));
                thief.set_item(bn::sprite_items::thief, (frame / 8) % 2 ? 3 : 1);
            }
            else
            {
                // 받침대의 것을 집는다: 방이 빈 방으로 바뀌고 도둑은 손을 뻗은 모습
                if(frame == grab_frame)
                {
                    room_loot.set_visible(false);
                    room_empty.set_visible(true);
                }

                thief.set_x(thief_stop_x + 6);
                thief.set_item(bn::sprite_items::thief, 4);
                ready = frame >= grab_frame + thief_ready_after_grab && photo_countdown < 0;
            }
        }
        else if(page == 1)
        {
            if(photo_countdown > 0)
            {
                --photo_countdown;
            }
            else if(photo_countdown == 0)
            {
                photo_countdown = -1;
                room_empty.set_visible(false);
                thief.set_visible(false);
                photo.set_visible(true);
                flash = 8;
                page_frame = 0;
            }

            if(photo_countdown < 0 && photo.visible())
            {
                if(page_frame == thief_message_delay)
                {
                    show_message("수상한 그림자가", loss.is_sword ? "창고를 뒤졌다!" : "돈주머니를 뒤졌다!");
                }

                ready = page_frame >= thief_message_delay + 12;
            }
        }
        else
        {
            ready = page_frame >= 12;
        }

        arrow[0].set_visible(ready && (frame / indicator_blink_frames) % 2 == 0);

        if(ready && bn::keypad::a_pressed())
        {
            if(page == 0)
            {
                page = 1;
                page_frame = 0;
                photo_countdown = sfx_latency_frames;
                bn::sound_items::sfx_thief.play();      // 히히히
            }
            else if(page == 1)
            {
                page = 2;
                page_frame = 0;

                if(loss.is_sword)
                {
                    show_message(sword_label(loss.level), "도둑맞았다...");
                }
                else
                {
                    show_message(format_money(loss.money), "도둑맞았다...");
                }
            }
            else
            {
                break;
            }
        }

        if(flash > 0)
        {
            set_flash(bn::fixed(flash) / 14);
            --flash;

            if(flash == 0)
            {
                set_flash(0);
            }
        }

        ++frame;
        ++page_frame;
        bn::core::update();
    }

    set_flash(0);
    fade_screen(false, true);
    bn::music::stop();
    bn::blending::set_transparency_alpha(1);
}

// ─── 수상한 상인 이벤트 (노을, 판매할 때) ───────────────────────────────────
// 노을 방 창이 열리고 상인이 그림자(실루엣)로 웅크려 살금살금 걸어 들어오다 색이 돌아오고, 좌우를 살핀다 → A → 사진이 번쩍이며
// 받침대에 돈뭉치를 내려놓는다 (도둑이 검을 집는 것과 같은 자리: 방이 금화 더미 방으로 바뀌고 상인은 주머니를 내민 모습) → A →
// 사진이 번쩍이며 "쉿... 비밀이오." → A → **같은 사진 그대로** "대금의 절반을 더 얹어 주었다!" + 금액 → A → 끝.
// 도둑·고양이 이벤트와 같은 틀이다 (마지막 페이지에도 장면을 새로 바꾸지 않고 사진 위의 메시지만 바꾼다).
// 소리는 아직 임시: 알림 sfx_event, 웃음 sfx_thief, 금화 sfx_sell_2, 장면 음악 bgm_event_thief (전용 소리가 생기면 바꾼다).
constexpr int merchant_walk_start = 24;
constexpr int merchant_walk_frames = 56;
constexpr int merchant_silhouette_frames = 28;          // 이 동안은 검은 그림자, 그 뒤 16프레임에 걸쳐 색이 돌아온다
constexpr int merchant_look_frames = 18;                // 좌우를 살피는 주기
constexpr int merchant_look_total = 36;                 // 좌우를 살피는 시간 (두 번)
constexpr int merchant_ready_after_drop = 24;           // 돈뭉치를 내려놓은 뒤 A를 받기까지
constexpr int merchant_message_delay = 14;

void run_merchant_event(hs::economy& eco)
{
    const int32_t bonus = eco.pay_merchant();      // 방금 판 금액의 50%를 더 받는다

    bn::sprite_text_generator center_generator(hs::font_outline);
    center_generator.set_center_alignment();

    bn::regular_bg_ptr background = forge_bg_item(hs::current_time_band()).create_bg(title_bg_x, title_bg_y);
    bn::regular_bg_ptr room = bn::regular_bg_items::merchant_room.create_bg(title_bg_x, title_bg_y);
    bn::regular_bg_ptr room_coins = bn::regular_bg_items::merchant_room_coins.create_bg(title_bg_x, title_bg_y);
    bn::regular_bg_ptr photo = bn::regular_bg_items::merchant_photo.create_bg(title_bg_x, title_bg_y);
    room.set_priority(1);
    room_coins.set_priority(1);
    photo.set_priority(1);
    room_coins.set_visible(false);
    photo.set_visible(false);

    bn::sprite_ptr anvil = bn::sprite_items::anvil.create_sprite(0, anvil_y);
    anvil.set_bg_priority(2);

    bn::sprite_ptr merchant = bn::sprite_items::merchant.create_sprite(thief_left_x, thief_y, 0);
    merchant.set_bg_priority(0);
    merchant.set_visible(false);
    merchant.set_blending_enabled(true);
    bn::sprite_palette_ptr merchant_palette = merchant.palette();
    merchant_palette.set_fade(bn::color(3, 2, 6), 1);           // 처음에는 검은 그림자

    bn::sprite_palette_ptr gold_palette = create_two_tone_font_palette(gold_body_color, gold_outline_color);
    bn::vector<bn::sprite_ptr, 40> message_sprites;
    bn::vector<bn::sprite_ptr, 16> bonus_sprites;

    auto show_message = [&](const bn::string_view& first, const bn::string_view& second, const bn::string_view& third)
    {
        message_sprites.clear();
        bonus_sprites.clear();
        center_generator.generate(0, cat_message_y, first, message_sprites);
        center_generator.generate(0, cat_message_y + cat_message_step, second, message_sprites);

        if(! third.empty())
        {
            center_generator.generate(0, cat_message_y + cat_message_step * 2, third, bonus_sprites);
            set_sprites_palette(bonus_sprites, gold_palette);
        }

        for(bn::sprite_ptr& sprite : message_sprites)
        {
            sprite.set_bg_priority(0);
        }

        for(bn::sprite_ptr& sprite : bonus_sprites)
        {
            sprite.set_bg_priority(0);
        }
    };

    auto set_flash = [&](bn::fixed level)
    {
        bn::bg_palettes::set_fade(bn::color(31, 31, 31), level);
        bn::sprite_palettes::set_fade(bn::color(31, 31, 31), level);
    };

    bn::music_items::bgm_event_thief.play(bn::fixed(0.9));
    fade_screen(true);

    bn::sprite_text_generator arrow_generator(hs::font);
    arrow_generator.set_left_alignment();
    bn::vector<bn::sprite_ptr, 1> arrow;
    arrow_generator.generate(indicator_x, second_line_y, "▼", arrow);
    arrow[0].set_bg_priority(0);
    arrow[0].set_visible(false);

    const int drop_frame = merchant_walk_start + merchant_walk_frames + merchant_look_total;
    int page = 0;
    int page_frame = 0;
    int photo_countdown = -1;
    int flash = 0;
    int frame = 0;

    while(true)
    {
        if(frame == 0)
        {
            bn::sound_items::sfx_event.play();      // 이벤트 발생 알림 (공용 소리)
            flash = 8;
        }

        bool ready = false;

        if(page == 0 && frame >= merchant_walk_start)
        {
            const int u = frame - merchant_walk_start;
            merchant.set_visible(true);
            bn::blending::set_transparency_alpha(u >= 12 ? bn::fixed(1) : bn::fixed(u) / 12);

            // 그림자 → 색이 돌아온다
            const int color_u = u - merchant_silhouette_frames;
            merchant_palette.set_fade(bn::color(3, 2, 6), color_u <= 0 ? bn::fixed(1) : color_u >= 16 ? bn::fixed(0) : bn::fixed(1) - bn::fixed(color_u) / 16);

            if(u < merchant_walk_frames)
            {
                merchant.set_x(thief_left_x + (thief_stop_x - thief_left_x) * u / merchant_walk_frames);
                merchant.set_item(bn::sprite_items::merchant, (u / 8) % 4);
            }
            else if(frame < drop_frame)
            {
                // 좌우를 살핀다 (뒤돌아봄)
                merchant.set_x(thief_stop_x);
                merchant.set_item(bn::sprite_items::merchant, 4);
                merchant.set_horizontal_flip(((u - merchant_walk_frames) / merchant_look_frames) % 2 == 1);
            }
            else
            {
                // 받침대에 돈뭉치를 내려놓는다: 방이 금화 더미 방으로 바뀌고 상인은 주머니를 내민 모습 (도둑이 검을 집는 것과 같은 자리)
                if(frame == drop_frame)
                {
                    room.set_visible(false);
                    room_coins.set_visible(true);
                    bn::sound_items::sfx_sell_0.play();         // 임시: 동전 몇 닢
                }

                merchant.set_horizontal_flip(false);
                merchant.set_x(thief_stop_x + 6);
                merchant.set_item(bn::sprite_items::merchant, 5);
                ready = frame >= drop_frame + merchant_ready_after_drop && photo_countdown < 0;
            }
        }
        else if(page == 1)
        {
            if(photo_countdown > 0)
            {
                --photo_countdown;
            }
            else if(photo_countdown == 0)
            {
                photo_countdown = -1;
                room_coins.set_visible(false);
                merchant.set_visible(false);
                photo.set_visible(true);
                flash = 8;
                page_frame = 0;
            }

            if(photo_countdown < 0 && photo.visible())
            {
                if(page_frame == merchant_message_delay)
                {
                    show_message("쉿... 비밀이오.", "이건 덤이오...", "");
                }

                ready = page_frame >= merchant_message_delay + 12;
            }
        }
        else
        {
            ready = page_frame >= 12;
        }

        arrow[0].set_visible(ready && (frame / indicator_blink_frames) % 2 == 0);

        if(ready && bn::keypad::a_pressed())
        {
            if(page == 0)
            {
                page = 1;
                page_frame = 0;
                photo_countdown = sfx_latency_frames;
                bn::sound_items::sfx_thief.play();      // 임시: 상인 웃음소리 전용 효과음이 생기면 바꾼다
            }
            else if(page == 1)
            {
                page = 2;
                page_frame = 0;

                // 사진은 그대로 두고 보상 메시지로 바꾼다 (도둑·고양이 이벤트와 같다)
                bn::sound_items::sfx_sell_2.play();

                bn::string<24> bonus_line("+");
                bonus_line += format_money(bonus);
                show_message("수상한 상인이 대금의", "절반을 더 얹어 주었다!", bonus_line);
            }
            else
            {
                break;
            }
        }

        if(flash > 0)
        {
            set_flash(bn::fixed(flash) / 14);
            --flash;

            if(flash == 0)
            {
                set_flash(0);
            }
        }

        ++frame;
        ++page_frame;
        bn::core::update();
    }

    set_flash(0);
    fade_screen(false, true);
    bn::music::stop();
    bn::blending::set_transparency_alpha(1);
}

int main()
{
    bn::core::init();
    run_splash();

    while(true)     // 일시정지 메뉴의 Quit이 타이틀로 돌아오는 바깥 고리
    {
#ifdef HS_DEBUG
        const bool hard_unlocked = true;
#else
        const bool hard_unlocked = hs::save::hard_unlocked();   // 스토리 클리어 시 hs::save::unlock_hard() (엔딩과 함께 연결 예정)
#endif

        unsigned seed = 0;
        title_choice choice = run_title(hard_unlocked, seed);

        if(choice == title_choice::OPTION)
        {
            bn::core::update();     // 타이틀의 스프라이트 타일을 먼저 비운다
            run_option();
            bn::core::update();
            continue;
        }

        hs::economy eco(choice == title_choice::HARD ? hs::difficulty::HARD : hs::difficulty::EASY);

        // NEW GAME / 저장 슬롯을 고른다. 불러오면 컷씬 없이 바로 시작하고, NEW GAME은 STORY만 오프닝 컷씬부터.
        start_result start = run_start_select(eco, seed);
        bn::core::update();     // 시작 화면의 스프라이트 타일을 먼저 비운다

        if(start == start_result::BACK)
        {
            continue;       // 타이틀로
        }

        if(start == start_result::NEW_GAME && choice == title_choice::STORY)
        {
            run_opening_cutscene();
        }

        bn::seed_random random(seed);
        eco.ensure_cat_target(random);      // 새 게임과 예전 저장(목표가 없는)에 고양이 목표를 정한다
        bn::string_view message = "";
        bool quit = false;

        cut_forge_bgm();

        while(! quit)
        {
            forge_exit exit = run_forge(eco, random, message);
            message = "";

            // 스프라이트 타일은 한 프레임 업데이트가 지나야 반환된다. 강화 화면이 타일을 많이 쓰므로 다음 화면 전에 비운다.
            bn::core::update();

            switch(exit)
            {

            case forge_exit::SHOP:
                run_shop(eco, random);
                break;

            case forge_exit::SWORDS:
                run_storage(eco, true);
                break;

            case forge_exit::BAG:
                run_storage(eco, false);
                break;

            case forge_exit::FURNACE:
                run_crafting(eco);
                break;

            case forge_exit::PAUSE:
                quit = run_pause(eco) == pause_choice::QUIT;

                if(quit)
                {
                    cut_forge_bgm();
                }
                break;

            case forge_exit::ENDING:
                cut_forge_bgm();
                run_ending(eco.diff());
                quit = true;        // 타이틀로
                break;

            case forge_exit::GAME_OVER:
                cut_forge_bgm();
                run_bankrupt_cutscene();        // 스미스의 독백 → 새 GAME OVER 화면
                run_game_over();
                quit = true;        // 타이틀로
                break;

            case forge_exit::CAT:
                cut_forge_bgm();
                run_cat_event(eco, random);
                break;

            case forge_exit::RECIPE:
                run_recipe(eco);
                break;

            case forge_exit::THIEF:
                cut_forge_bgm();
                run_thief_event(eco, random);
                break;

            case forge_exit::MERCHANT:
                cut_forge_bgm();
                run_merchant_event(eco);
                break;

            default:
                break;
            }

            bn::core::update();
        }
    }
}
