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
#include "bn_regular_bg_items_scene4_forge_door.h"
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

bn::string<32> sword_label(int level)
{
    bn::string<32> result("+");
    result += bn::to_string<4>(level);
    result += " ";
    result += hs::sword_names[level];
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
        _sprites.clear();
        _offset_x = 0;
        _offset_y = 0;
        _generator.generate(_x, _y, text, _sprites);

        if(_sticky_palette)
        {
            set_palette(*_sticky_palette);
        }

        apply_visible();
    }

    // 글자 전체의 팔레트를 바꾼다. (Butano는 글자 여러 개를 스프라이트 하나로 묶어 그리므로, 일부 글자만 색을 바꾸려면 글을 나눠 만들어야 한다)
    void set_palette(const bn::sprite_palette_ptr& palette)
    {
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
        _x = x;
    }

    // 다음 set_text부터 적용되는 y 좌표
    void set_y(int y)
    {
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

    text_sprites.clear();

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
constexpr int menu_icon_y = 28;
constexpr int menu_label_y = 51;

// hard_unlocked: 스토리를 전부 클리어하면 true (세이브 연동은 나중에)
// seed: 메뉴를 고른 순간의 프레임 수. 강화 난수 시드로 쓴다.
title_choice run_title(bool hard_unlocked, unsigned& seed)
{
    bn::sprite_text_generator left_generator(hs::font);
    left_generator.set_left_alignment();

    bn::sprite_text_generator center_generator(hs::font);
    center_generator.set_center_alignment();

    bn::bg_palettes::set_transparent_color(bn::color(4, 3, 3));

    // 타이틀 음악 (assets/audio/title_chiptune.wav → tools/make_music.py가 루프 XM으로 변환). 게임을 시작하면 끈다.
    bn::music_items::title_chiptune.play(1);

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
        }
        else if(bn::keypad::right_pressed())
        {
            selected = (selected + 1) % menu_count;
        }

        if(selected != styled)
        {
            style_menu();
        }

        if(bn::keypad::a_pressed() || bn::keypad::start_pressed())
        {
            if(selected == int(title_choice::OPTION))
            {
                notice_text.set_text("OPTION - COMING SOON");
                notice_frames = 90;
            }
            else if(! enabled(selected))
            {
                notice_text.set_text("HARD - LOCKED");
                notice_frames = 90;
            }
            else
            {
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
};

// 검 스프라이트 프레임 (tools/make_swords.py가 한 장에 4개를 만든다)
constexpr int frame_whole = 0;
constexpr int frame_cracked = 1;
constexpr int frame_upper = 2;
constexpr int frame_lower = 3;

enum class forge_exit
{
    SHOP,       // L
    FURNACE,    // R (화구: 잡템 조합)
    SWORDS,     // ← (창고)
    BAG,        // ↓ (잡화)
    PAUSE,      // START (일시정지 메뉴)
    GAME_OVER,  // 파산
};

constexpr int hammer_frames = 40;
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
// 검(그림 53줄)은 화면 y 40부터(성공률 줄 아래), 모루(tools/make_anvil.py, 60x34)는 검 폼멜 바로 아래 y 92부터
constexpr int sword_y = -8;
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
constexpr int message_single_y = 60;
constexpr int message_first_y = 54;
constexpr int message_second_y = 66;
constexpr int message_frames = 90;      // 메시지가 이름 자리를 차지하는 시간

// SELECT 도움말: 버튼을 누르고 있는 동안 위쪽 가운데 세 줄에 설명을 보여준다 (줄 폭 150px 이하, 한글 11자 안팎).
// 순서: 강화(A), 판매(B), 상점(L), 화구(R), 창고(←), 잡화(↓).
constexpr int help_count = 6;
constexpr const char* help_labels[help_count] = { "강화", "판매", "상점", "화구", "창고", "잡화" };
constexpr const char* help_texts[help_count][3] = {
    { "무기를 강화해 더", "강력한 검에", "도전하자." },
    { "강화 중인 무기를", "판매하여 돈을", "벌자." },
    { "돈을 사용해 강화에", "필요한 아이템을", "구매하자." },
    { "재료를 모아 특별한", "무기와 아이템을", "제작하자." },
    { "보유한 무기와", "아이템을 보관하고", "관리하자." },
    { "강화 실패로 얻은", "재료를 모으고", "관리하자." },
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
    bn::regular_bg_ptr background = bn::regular_bg_items::forge_bg.create_bg(title_bg_x, title_bg_y);

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
    bn::bg_palette_ptr background_palette = background.palette();
    bn::sprite_palette_ptr anvil_palette = anvil.palette();
    text_line help_line_1(center_generator, 0, name_y);
    text_line help_line_2(center_generator, 0, chance_y);
    text_line help_line_3(center_generator, 0, chance_y + 16);
    bn::vector<bn::sprite_ptr, 16> help_label_sprites;
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

        bool idle = current_phase == phase::IDLE;
        set_button_enabled(shop_icon_palette, shop_badge, eco.level() == 0);
        set_button_enabled(furnace_icon_palette, furnace_badge, eco.level() == 0);
        set_button_enabled(sell_icon_palette, sell_badge, idle && eco.can_sell());
        set_button_enabled(upgrade_icon_palette, upgrade_badge,
                           idle && ! eco.at_max_level() && eco.can_afford_upgrade());

        // 검 그림은 단계가 바뀔 때 새로 만든다. 망치질 중에는 결과를 아직 보여주지 않는다.
        if(current_phase != phase::HAMMERING && ! reveal_hold && eco.level() != sword_level)
        {
            sword_level = eco.level();
            sword = sword_items[sword_level]->create_sprite(0, sword_y);
            sword.set_z_order(0);
            sword_palette = sword.palette();
        }
    };

    auto set_help = [&](bool on)
    {
        help_mode = on;
        help_key = -1;
        help_line_1.set_text("");
        help_line_2.set_text("");
        help_line_3.set_text("");
        help_label_sprites.clear();

        bn::fixed gray = on ? 1 : 0;
        background_palette.set_grayscale_intensity(gray);
        anvil_palette.set_grayscale_intensity(gray);
        sword_palette.set_grayscale_intensity(gray);
        sword.set_visible(! on);        // 설명 셋째 줄이 검 끝과 겹치므로 도움말 동안은 검을 숨긴다

        name_prefix_text.set_visible(! on);
        name_rest_text.set_visible(! on);
        chance_text.set_visible(! on);
        cost_label.set_visible(! on);
        cost_value.set_visible(! on);
        cost_qty_text.set_visible(! on);
        message_line_1.set_visible(! on);
        message_line_2.set_visible(! on);

        if(cost_icon)
        {
            cost_icon->set_visible(! on);
        }

        sell_label.set_visible(! on);
        sell_value.set_visible(! on);
        money_text.set_visible(! on);

        if(on)
        {
            for(int index = 0; index < help_count; ++index)
            {
                center_generator.generate(help_buttons[index]->x, help_buttons[index]->y, help_labels[index],
                                          help_label_sprites);
            }

            // 판매·강화는 못 쓰는 상태여도 도움말에서는 컬러로 보여준다
            set_button_enabled(sell_icon_palette, sell_badge, true);
            set_button_enabled(upgrade_icon_palette, upgrade_badge, true);
            set_button_enabled(shop_icon_palette, shop_badge, true);
            set_button_enabled(furnace_icon_palette, furnace_badge, true);
        }
        else
        {
            refresh_status();
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
    bn::optional<bn::sprite_ptr> piece_upper;
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
        sword.set_visible(false);
        sword_palette.set_fade(bn::color(31, 31, 31), 0);

        upper_position = bn::fixed_point(0, sword_y);
        lower_position = bn::fixed_point(0, sword_y);
        upper_velocity = bn::fixed_point(-1.2, -2.2);      // 위쪽 조각은 왼쪽 위로 튀고
        lower_velocity = bn::fixed_point(0.8, -0.8);       // 아래쪽 조각은 살짝 오른쪽으로 떨어진다
        piece_upper = item.create_sprite(upper_position, frame_upper);
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

                if(t == grand_peak)
                {
                    // 가장 눈부신 순간에 비로소 +29 검과 이름, 성공 메시지가 드러난다 (실패 낚시와 끝까지 구분되지 않게)
                    reveal_hold = false;
                    refresh_status();
                    show_message("강화 성공!", &blue_tint);
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

    while(true)
    {
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
        }

        switch(current_phase)
        {

        case phase::IDLE:
            // 파산: 메시지(판매·파괴 결과)를 다 보여 준 뒤 게임 오버
            if(message_timer == 0 && ! help_mode && eco.is_bankrupt())
            {
                fade_screen(false);
                return forge_exit::GAME_OVER;
            }

            if(bn::keypad::select_pressed())
            {
                set_help(! help_mode);
            }
            else if(help_mode)
            {
                int key = bn::keypad::a_held() ? 0 : bn::keypad::b_held() ? 1 : bn::keypad::l_held() ? 2 :
                          bn::keypad::r_held() ? 3 : bn::keypad::left_held() ? 4 : bn::keypad::down_held() ? 5 : -1;

                if(key != help_key)
                {
                    help_key = key;
                    help_line_1.set_text(key < 0 ? "" : help_texts[key][0]);
                    help_line_2.set_text(key < 0 ? "" : help_texts[key][1]);
                    help_line_3.set_text(key < 0 ? "" : help_texts[key][2]);
                }
            }
            else if(bn::keypad::a_pressed())
            {
                if(eco.at_max_level())
                {
                    // 최종 검(+29): 엔딩으로 가는 안내 (화려한 색이 도는 글자가 흔들린다). 엔딩 화면은 아직 없다.
                    show_message("엔딩으로..", &money_palette);
                    ending_shake = true;
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
                    current_phase = phase::HAMMERING;
                    phase_frame = 0;
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
                    bn::string<48> sold_line("판매 +");
                    sold_line += format_money_short(eco.sell());
                    message_text(sold_line);
                    refresh_status();
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
                        constexpr int jump_levels[9] = { 5, 16, 21, 22, 23, 24, 25, 27, 28 };
                        eco.debug_set_level(jump_levels[debug_jump_index]);
                        debug_jump_index = (debug_jump_index + 1) % 9;
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
            sword.set_y(sword_y + ((phase_frame / 4) % 2 == 0 ? 0 : 2));

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
            piece_upper->set_position(upper_position);
            piece_upper->set_rotation_angle(bn::fixed(360) - phase_frame * 3);      // 위쪽 조각은 왼쪽으로 돌며
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
                piece_upper.reset();
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
        right_generator.generate(110, top_y + index * step_y, format_money(eco.price(item)), row_sprites);
    }

    list_cursor cursor(left_generator, top_y, step_y);
    int selected = 0;
    int frame = 0;
    int notice = 0;

    auto refresh_status = [&]()
    {
        money_text.set_text(format_money(eco.money()));

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
        }
        else if(bn::keypad::down_pressed())
        {
            selected = (selected + 1) % hs::shop_item_count;
        }
        else if(bn::keypad::a_pressed())
        {
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
        }
        else if(count > 0 && bn::keypad::down_pressed())
        {
            selected = (selected + 1) % count;
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

    constexpr int count_x = 6;              // 필요 수량(오른쪽 맞춤)
    constexpr int output_icon_x = 22;       // 결과 아이콘 가운데
    constexpr int output_text_x = 33;       // 결과 글자

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

            bn::string<16> count_line = bn::to_string<16>(eco.material_quantity(recipe.input));
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
        }
        else if(bn::keypad::down_pressed())
        {
            selected = (selected + 1) % hs::recipe_count;
        }
        else if(bn::keypad::a_pressed())
        {
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

// 슬롯 3줄(with_new면 맨 위에 "NEW GAME" 한 줄 더)을 그리고, 고른 줄만 위아래로 흔든다.
// 줄 번호는 with_new일 때 0 = NEW GAME, 1~3 = 슬롯 1~3, 아니면 0~2 = 슬롯 1~3.
class slot_list
{

public:
    slot_list(bn::sprite_text_generator& center_generator, int top_y, int step_y, bool with_new = false) :
        _generator(center_generator),
        _top_y(top_y),
        _step_y(step_y),
        _first_slot_line(with_new ? 1 : 0)
    {
        refresh();
    }

    [[nodiscard]] int line_count() const { return _first_slot_line + hs::save::slot_count; }

    void refresh()
    {
        if(_first_slot_line > 0)
        {
            _lines[0].clear();
            _generator.generate(0, _top_y, "NEW GAME", _lines[0]);
        }

        for(int slot = 0; slot < hs::save::slot_count; ++slot)
        {
            bn::ivector<bn::sprite_ptr>& line = _lines[_first_slot_line + slot];
            line.clear();
            _generator.generate(0, _top_y + (_first_slot_line + slot) * _step_y, slot_label(slot), line);
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
    constexpr int item_count = 3;
    constexpr const char* item_names[item_count] = { "Resume", "Save", "Quit" };
    constexpr int title_y = -44;
    constexpr int top_y = -8;
    constexpr int step_y = 20;
    constexpr const char* default_hint = "A 선택  B 돌아가기";

    // 강화 화면 배경을 흑백으로 어둡게 깔고, 글자는 검은 외곽선이 있는 폰트(hs::font_outline)로 쓴다
    bn::regular_bg_ptr background = bn::regular_bg_items::forge_bg.create_bg(title_bg_x, title_bg_y);
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

    slot_list slots(center_generator, top_y, step_y);
    slots.set_visible(false);
    text_line footer_text(left_generator, -114, 68);

    bool slot_screen = false;
    bool confirming = false;     // 덮어쓰기 확인 중
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

        if(! slot_screen)
        {
            if(bn::keypad::up_pressed())
            {
                selected = (selected + item_count - 1) % item_count;
                frame = 8;
            }
            else if(bn::keypad::down_pressed())
            {
                selected = (selected + 1) % item_count;
                frame = 8;
            }
            else if(bn::keypad::start_pressed() || bn::keypad::b_pressed())
            {
                fade_screen(false);
                return pause_choice::RESUME;
            }
            else if(bn::keypad::a_pressed())
            {
                if(selected == 0)
                {
                    fade_screen(false);
                    return pause_choice::RESUME;
                }

                if(selected == 1)
                {
                    slot_screen = true;
                    slot = 0;
                    frame = 8;
                    show_items(false);
                    slots.set_visible(true);
                }
                else
                {
                    fade_screen(false);
                    return pause_choice::QUIT;
                }
            }

            animate_items();
        }
        else if(confirming)
        {
            if(bn::keypad::a_pressed())
            {
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
                frame = 8;
            }
            else if(bn::keypad::down_pressed())
            {
                slot = (slot + 1) % hs::save::slot_count;
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

    slot_list list(center_generator, top_y, step_y, true);
    text_line footer_text(left_generator, -114, 68);

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
            frame = 8;
        }
        else if(bn::keypad::down_pressed())
        {
            selected = (selected + 1) % list.line_count();
            frame = 8;
        }
        else if(bn::keypad::b_pressed())
        {
            fade_screen(false);
            return start_result::BACK;
        }
        else if(bn::keypad::a_pressed())
        {
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
    { "아버지", "우리 가문은... 시공간을 초월하는 전설의 대장장이 집안이었다." },
    { "아버지", "어떻게서든... 이름을 알려라. 가문의 명예를 높이던, 부자가 되던..." },
};

constexpr dialogue_line scene2_lines[] = {
    { "아버지", "이 모루와 함께라면..." },
};

constexpr dialogue_line scene3_lines[] = {
    { "스미스", "아버지...!!" },
};

constexpr dialogue_line scene4_lines[] = {
    { "스미스", "아버지의 소원을 이루겠다." },
    { "스미스", "기필코... 반드시." },
};

constexpr cutscene_scene opening_scenes[] = {
    { bn::regular_bg_items::scene1_bedside, scene1_lines },
    { bn::regular_bg_items::scene2_anvil, scene2_lines },
    { bn::regular_bg_items::scene3_father_close, scene3_lines },
    { bn::regular_bg_items::scene4_forge_door, scene4_lines },
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

void run_opening_cutscene()
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
    bn::music_items::story_chiptune_sad.play(1);
    int scene_number = 0;

    for(const cutscene_scene& scene : opening_scenes)
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

        fade_screen(false, scene_number == int(sizeof(opening_scenes) / sizeof(opening_scenes[0])));
    }

    bn::music::stop();
}

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
        bn::string_view message = "";
        bool quit = false;

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
                break;

            case forge_exit::GAME_OVER:
                run_game_over();
                quit = true;        // 타이틀로
                break;

            default:
                break;
            }

            bn::core::update();
        }
    }
}
