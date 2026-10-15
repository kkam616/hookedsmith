#include "bn_core.h"
#include "bn_array.h"
#include "bn_assert.h"
#include "bn_bg_palettes.h"
#include "bn_sprite_palettes.h"
#include "bn_color.h"
#include "bn_fixed.h"
#include "bn_keypad.h"
#include "bn_regular_bg_ptr.h"
#include "bn_seed_random.h"
#include "bn_sprite_palette_ptr.h"
#include "bn_sprite_ptr.h"
#include "bn_sprite_text_generator.h"
#include "bn_span.h"
#include "bn_string.h"
#include "bn_time.h"
#include "bn_vector.h"

#include "bn_regular_bg_items_title_day.h"
#include "bn_regular_bg_items_title_night.h"
#include "bn_regular_bg_items_title_sunset.h"
#include "bn_regular_bg_items_splash_studio.h"
#include "bn_regular_bg_items_cutscene_textbox.h"
#include "bn_regular_bg_items_scene1_bedside.h"
#include "bn_regular_bg_items_scene2_anvil.h"
#include "bn_regular_bg_items_scene3_father_close.h"
#include "bn_regular_bg_items_scene4_forge_door.h"
#include "bn_sprite_items_anvil.h"
#include "bn_sprite_items_icon_craft.h"
#include "bn_sprite_items_icon_sell.h"
#include "bn_sprite_items_icon_shop.h"
#include "bn_sprite_items_icon_storage.h"
#include "bn_sprite_items_icon_upgrade.h"
#include "bn_sprite_items_sword.h"
#include "economy.h"
#include "hs_font.h"

namespace
{

const char* sword_name(int level)
{
    if(level == 0)
    {
        return "녹슨 검";
    }

    if(level <= 5)
    {
        return "쓸만한 검";
    }

    if(level <= 10)
    {
        return "소문난 검";
    }

    if(level <= 15)
    {
        return "왕실의 검";
    }

    return "전설의 검";
}

void set_sword_tint(bn::sprite_palette_ptr& palette, int level)
{
    if(level == 0)
    {
        palette.set_fade(bn::color(18, 9, 3), bn::fixed(0.45));
    }
    else if(level <= 5)
    {
        palette.set_fade_intensity(0);
    }
    else if(level <= 10)
    {
        palette.set_fade(bn::color(12, 22, 31), bn::fixed(0.25));
    }
    else if(level <= 15)
    {
        palette.set_fade(bn::color(31, 25, 6), bn::fixed(0.35));
    }
    else
    {
        palette.set_fade(bn::color(24, 10, 31), bn::fixed(0.35));
    }
}

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

bn::string<32> sword_label(int level)
{
    bn::string<32> result("+");
    result += bn::to_string<4>(level);
    result += " ";
    result += sword_name(level);
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
        _generator.generate(_x, _y, text, _sprites);
    }

private:
    bn::sprite_text_generator& _generator;
    int _x;
    int _y;
    bn::vector<bn::sprite_ptr, 16> _sprites;
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

// 잠긴 메뉴·쓸 수 없는 버튼 글자를 회색으로 보이게 하는 전용 팔레트
bn::sprite_palette_ptr create_locked_palette()
{
    bn::sprite_palette_ptr result = create_private_font_palette();
    result.set_fade(bn::color(12, 12, 12), bn::fixed(0.6));
    return result;
}

void set_sprites_palette(bn::ivector<bn::sprite_ptr>& sprites, const bn::sprite_palette_ptr& palette)
{
    for(bn::sprite_ptr& sprite : sprites)
    {
        sprite.set_palette(palette);
    }
}

// 타이틀 그림은 하루 24시간을 8시간씩 셋으로 나눠 바꾼다 (카트리지 RTC 시계 기준)
// 낮 06~14시, 노을 14~22시, 밤 22~06시. 시계가 없으면 낮.
const bn::regular_bg_item& title_item_for_now()
{
    if(bn::time::active())
    {
        if(bn::optional<bn::time> now = bn::time::current())
        {
            int hour = now->hour();

            if(hour >= 6 && hour < 14)
            {
                return bn::regular_bg_items::title_day;
            }

            if(hour >= 14 && hour < 22)
            {
                return bn::regular_bg_items::title_sunset;
            }

            return bn::regular_bg_items::title_night;
        }
    }

    return bn::regular_bg_items::title_day;
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
    // 256x256 BMP를 화면 중앙에 맞춰 배치 (256→240: 8픽셀 오프셋)
    bn::regular_bg_ptr studio_bg = bn::regular_bg_items::splash_studio.create_bg(8, 48);

    // 페이드 인
    for(int f = 0; f <= SPLASH_FADE; ++f)
    {
        bn::bg_palettes::set_fade(bn::color(0, 0, 0), bn::fixed(SPLASH_FADE - f) / SPLASH_FADE);
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
        bn::core::update();
    }
}

// in이면 검정→화면, 아니면 화면→검정
void fade_screen(bool in)
{
    for(int f = 0; f <= SPLASH_FADE; ++f)
    {
        bn::fixed intensity = bn::fixed(in ? SPLASH_FADE - f : f) / SPLASH_FADE;
        bn::bg_palettes::set_fade(bn::color(0, 0, 0), intensity);
        bn::sprite_palettes::set_fade(bn::color(0, 0, 0), intensity);
        bn::core::update();
    }
}

// 256x256 그림의 왼쪽 위 240x160이 화면에 오도록 옮긴다
constexpr int title_bg_x = 8;
constexpr int title_bg_y = 48;

enum class title_choice
{
    STORY,
    INFINITE,
    OPTION,
};

constexpr int menu_count = 3;
constexpr const char* menu_labels[menu_count] = { "STORY", "INFINITE", "OPTION" };
constexpr int menu_x = -28;
constexpr int menu_cursor_x = menu_x - 12;
constexpr int menu_top_y = 40;
constexpr int menu_step_y = 16;

// infinite_unlocked: 스토리를 전부 클리어하면 true (세이브 연동은 나중에)
// seed: 메뉴를 고른 순간의 프레임 수. 강화 난수 시드로 쓴다.
title_choice run_title(bool infinite_unlocked, unsigned& seed)
{
    bn::sprite_text_generator left_generator(hs::font);
    left_generator.set_left_alignment();

    bn::sprite_text_generator center_generator(hs::font);
    center_generator.set_center_alignment();

    bn::bg_palettes::set_transparent_color(bn::color(4, 3, 3));

    const bn::regular_bg_item* title_item = &title_item_for_now();
    bn::regular_bg_ptr bg = title_item->create_bg(title_bg_x, title_bg_y);

    auto enabled = [infinite_unlocked](int index)
    {
        return index != int(title_choice::INFINITE) || infinite_unlocked;
    };

    bn::sprite_palette_ptr locked_palette = create_locked_palette();
    bn::vector<bn::sprite_ptr, 16> menu_sprites;

    for(int index = 0; index < menu_count; ++index)
    {
        int first = menu_sprites.size();
        left_generator.generate(menu_x, menu_top_y + index * menu_step_y, menu_labels[index], menu_sprites);

        if(! enabled(index))
        {
            for(int sprite_index = first; sprite_index < menu_sprites.size(); ++sprite_index)
            {
                menu_sprites[sprite_index].set_palette(locked_palette);
            }
        }
    }

    bn::vector<bn::sprite_ptr, 1> cursor_sprites;
    left_generator.generate(menu_cursor_x, menu_top_y, ">", cursor_sprites);
    bn::sprite_ptr& cursor = cursor_sprites[0];
    bn::fixed_point cursor_origin = cursor.position();

    text_line notice_text(center_generator, 0, -72);
    int selected = int(title_choice::STORY);
    int notice_frames = 0;
    unsigned frame = 0;

    auto move_cursor = [&](int direction)
    {
        do
        {
            selected = (selected + direction + menu_count) % menu_count;
        }
        while(! enabled(selected));
    };

    fade_screen(true);

    while(true)
    {
        ++frame;

        if(bn::keypad::up_pressed())
        {
            move_cursor(-1);
        }
        else if(bn::keypad::down_pressed())
        {
            move_cursor(1);
        }

        if(bn::keypad::a_pressed() || bn::keypad::start_pressed())
        {
            if(selected == int(title_choice::OPTION))
            {
                notice_text.set_text("OPTION - COMING SOON");
                notice_frames = 90;
            }
            else
            {
                seed = frame;
                fade_screen(false);
                return title_choice(selected);
            }
        }

        if(notice_frames > 0 && --notice_frames == 0)
        {
            notice_text.set_text("");
        }

        // 커서는 살짝 좌우로 흔들린다
        cursor.set_position(cursor_origin.x() - int(frame / 16) % 2, cursor_origin.y() + selected * menu_step_y);

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
};

enum class forge_exit
{
    SHOP,
    STORAGE,
    CRAFTING,
};

constexpr int hammer_frames = 40;
constexpr int success_frames = 24;
constexpr int fail_frames = 24;
// 검(그림 52줄)은 화면 y 22부터, 모루(tools/make_anvil.py, 60x34)는 검 폼멜 바로 아래 y 74부터
constexpr int sword_y = -26;
constexpr int anvil_y = 26;
constexpr int info_x = -116;    // 왼쪽 정보 줄. 이름이 모루 왼쪽 끝(화면 x 90) 앞에서 끝나야 한다

forge_exit run_forge(hs::economy& eco, bn::seed_random& random, const bn::string_view& first_message)
{
    bn::sprite_text_generator left_generator(hs::font);
    left_generator.set_left_alignment();

    bn::sprite_text_generator center_generator(hs::font);
    center_generator.set_center_alignment();

    bn::sprite_text_generator right_generator(hs::font);
    right_generator.set_right_alignment();

    bn::bg_palettes::set_transparent_color(bn::color(4, 3, 3));


    // 버튼 아이콘(tools/make_icons.py, 모두 40px)과 아이콘 오른쪽 아래 구석의 키 글자.
    // 아이콘은 z_order를 1로 두어 키 글자가 그 위에 그려지게 한다.
    struct button_layout
    {
        int x;
        int y;
        int size;
    };

    constexpr button_layout storage_button = { -96, -56, 40 };    // 왼쪽 위
    constexpr button_layout shop_button = { 96, -56, 40 };        // 오른쪽 위
    constexpr button_layout sell_button = { 96, 4, 40 };          // 오른쪽, 강화와 대각선
    constexpr button_layout upgrade_button = { 56, 44, 40 };
    constexpr button_layout craft_button = { 0, 60, 40 };         // 아래 가운데 (아랫변이 화면 끝)

    auto create_icon = [](const bn::sprite_item& item, const button_layout& layout)
    {
        bn::sprite_ptr icon = item.create_sprite(layout.x, layout.y);
        icon.set_z_order(1);
        return icon;
    };

    auto generate_badge = [&](const button_layout& layout, const char* key, bn::ivector<bn::sprite_ptr>& sprites)
    {
        int half = layout.size / 2;
        // 조합 아이콘은 화면 맨 아래에 붙어 있어 글자가 잘리지 않게 조금 올린다
        int offset_y = layout.y + half > 72 ? half - 7 : half - 5;
        center_generator.generate(layout.x + half - 4, layout.y + offset_y, key, sprites);
    };

    bn::sprite_ptr storage_icon = create_icon(bn::sprite_items::icon_storage, storage_button);
    bn::sprite_ptr shop_icon = create_icon(bn::sprite_items::icon_shop, shop_button);
    bn::sprite_ptr sell_icon = create_icon(bn::sprite_items::icon_sell, sell_button);
    bn::sprite_ptr upgrade_icon = create_icon(bn::sprite_items::icon_upgrade, upgrade_button);
    bn::sprite_ptr craft_icon = create_icon(bn::sprite_items::icon_craft, craft_button);

    bn::vector<bn::sprite_ptr, 8> label_sprites;
    generate_badge(storage_button, "L", label_sprites);
    generate_badge(shop_button, "R", label_sprites);
    generate_badge(craft_button, "↓", label_sprites);
    left_generator.generate(info_x, 72, "돈", label_sprites);

    // 판매·강화는 못 쓸 때 아이콘과 키 글자를 회색으로 바꾼다
    bn::vector<bn::sprite_ptr, 1> sell_badge;
    generate_badge(sell_button, "B", sell_badge);

    bn::vector<bn::sprite_ptr, 1> upgrade_badge;
    generate_badge(upgrade_button, "A", upgrade_badge);

    bn::sprite_palette_ptr normal_palette = sell_badge[0].palette();
    bn::sprite_palette_ptr locked_palette = create_locked_palette();
    bn::sprite_palette_ptr sell_icon_palette = sell_icon.palette();
    bn::sprite_palette_ptr upgrade_icon_palette = upgrade_icon.palette();

    auto set_button_enabled = [&](bn::sprite_palette_ptr& icon_palette, bn::ivector<bn::sprite_ptr>& badge,
                                  bool enabled)
    {
        icon_palette.set_grayscale_intensity(enabled ? 0 : 1);
        icon_palette.set_fade(bn::color(4, 3, 3), enabled ? 0 : bn::fixed(0.45));
        set_sprites_palette(badge, enabled ? normal_palette : locked_palette);
    };

    text_line message_text(center_generator, 0, -68);
    text_line cost_text(left_generator, info_x, -26);
    text_line price_text(left_generator, info_x, -10);
    text_line name_text(left_generator, info_x, 8);
    text_line chance_text(left_generator, info_x, 24);
    text_line ticket_text(left_generator, info_x, 54);
    text_line money_text(right_generator, 114, 72);

    // 모루는 z_order 1로 검 뒤에 둔다 (폼멜이 모루 윗면에 1px 겹친다)
    bn::sprite_ptr anvil = bn::sprite_items::anvil.create_sprite(0, anvil_y);
    anvil.set_z_order(1);
    bn::sprite_ptr sword = bn::sprite_items::sword.create_sprite(0, sword_y);
    bn::sprite_palette_ptr sword_palette = sword.palette();

    phase current_phase = phase::IDLE;
    int phase_frame = 0;
    bool upgrade_succeeded = false;     // 판정은 바로 하고, 결과는 망치질이 끝난 뒤 보여준다

    auto refresh_status = [&]()
    {
        const hs::level_row& row = eco.row();

        bn::string<32> cost_line("강화비 ");
        cost_line += eco.at_max_level() ? bn::string<24>("-") : format_money(row.cost_usd);
        cost_text.set_text(cost_line);

        bn::string<32> price_line("판매가 ");
        price_line += format_money(row.sell_usd);
        price_text.set_text(price_line);

        name_text.set_text(sword_label(eco.level()));

        bn::string<32> chance_line;

        if(eco.at_max_level())
        {
            chance_line = "최고 단계";
        }
        else
        {
            chance_line = "성공률 ";
            chance_line += bn::to_string<4>(row.success_pct);
            chance_line += "%";
        }

        chance_text.set_text(chance_line);

        bn::string<24> ticket_line("방지권 ");
        ticket_line += bn::to_string<8>(eco.tickets());
        ticket_text.set_text(ticket_line);

        money_text.set_text(format_money(eco.money()));

        bool idle = current_phase == phase::IDLE;
        set_button_enabled(sell_icon_palette, sell_badge, idle && eco.can_sell());
        set_button_enabled(upgrade_icon_palette, upgrade_badge,
                           idle && ! eco.at_max_level() && eco.can_afford_upgrade());
        set_sword_tint(sword_palette, eco.level());
    };

    // +0으로 돌아왔는데 강화비도 없으면 이웃이 돈을 준다
    auto check_loan = [&]()
    {
        if(eco.needs_loan())
        {
            eco.take_loan();
            message_text.set_text("이웃이 빵값을 줬다");
        }
    };

    auto destroy_sword = [&]()
    {
        hs::drop dropped = eco.destroy(random);

        if(dropped.quantity > 0)
        {
            bn::string<48> drop_line("파괴! ");
            drop_line += hs::material_names[int(dropped.item)];
            drop_line += " x";
            drop_line += bn::to_string<4>(dropped.quantity);
            message_text.set_text(drop_line);
        }
        else
        {
            message_text.set_text("검이 부서졌다...");
        }

        current_phase = phase::IDLE;
        check_loan();
        refresh_status();
    };

    refresh_status();
    message_text.set_text(first_message);
    check_loan();
    refresh_status();
    fade_screen(true);

    while(true)
    {
        switch(current_phase)
        {

        case phase::IDLE:
            if(bn::keypad::a_pressed())
            {
                if(eco.at_max_level())
                {
                    message_text.set_text("이미 전설이 되었다");
                }
                else if(! eco.can_afford_upgrade())
                {
                    message_text.set_text("돈이 모자란다");
                }
                else
                {
                    upgrade_succeeded = eco.upgrade(random);
                    current_phase = phase::HAMMERING;
                    phase_frame = 0;
                    refresh_status();
                    message_text.set_text("땅! 땅! 땅!");
                }
            }
            else if(bn::keypad::b_pressed())
            {
                if(! eco.can_sell())
                {
                    message_text.set_text("녹슨 검은 안 팔린다");
                }
                else
                {
                    bn::string<48> sold_line("판매 +");
                    sold_line += format_money(eco.sell());
                    message_text.set_text(sold_line);
                    check_loan();
                    refresh_status();
                }
            }
            else if(bn::keypad::l_pressed())
            {
                fade_screen(false);
                return forge_exit::STORAGE;
            }
            else if(bn::keypad::r_pressed())
            {
                fade_screen(false);
                return forge_exit::SHOP;
            }
            else if(bn::keypad::down_pressed())
            {
                fade_screen(false);
                return forge_exit::CRAFTING;
            }
#ifdef HS_DEBUG
            else if(bn::keypad::select_pressed())
            {
                eco.debug_grant();
                message_text.set_text("DEBUG +$100,000");
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
                    current_phase = phase::SUCCESS;
                    refresh_status();

                    bn::string<32> success_line("강화 성공! +");
                    success_line += bn::to_string<4>(eco.level());
                    message_text.set_text(success_line);
                }
                else
                {
                    current_phase = phase::FAIL;
                    message_text.set_text("강화 실패!");
                }
            }
            break;

        case phase::SUCCESS:
            ++phase_frame;
            sword_palette.set_fade(bn::color(31, 31, 31), bn::fixed(success_frames - phase_frame) / success_frames);
            sword.set_y(sword_y - (phase_frame < success_frames / 2 ? phase_frame / 2 : (success_frames - phase_frame) / 2));

            if(phase_frame >= success_frames)
            {
                sword.set_y(sword_y);
                current_phase = phase::IDLE;
                refresh_status();
            }
            break;

        case phase::FAIL:
            ++phase_frame;
            sword.set_x(phase_frame >= fail_frames ? 0 : ((phase_frame / 2) % 2 == 0 ? -3 : 3));

            if(phase_frame >= fail_frames)
            {
                if(eco.can_protect())
                {
                    bn::string<32> ask_line("방지권 ");
                    ask_line += bn::to_string<4>(eco.row().protect_cost);
                    ask_line += "장 쓸까?";
                    message_text.set_text(ask_line);
                    name_text.set_text("A 쓴다");
                    chance_text.set_text("B 안 쓴다");
                    current_phase = phase::PROTECT_ASK;
                }
                else
                {
                    destroy_sword();
                }
            }
            break;

        case phase::PROTECT_ASK:
            if(bn::keypad::a_pressed())
            {
                eco.protect();
                current_phase = phase::IDLE;
                message_text.set_text("방지권으로 지켰다");
                refresh_status();
            }
            else if(bn::keypad::b_pressed())
            {
                destroy_sword();
            }
            break;

        default:
            break;
        }

        bn::core::update();
    }
}

// ─── 목록 화면 공통 ──────────────────────────────────────────────────

constexpr int list_cursor_x = -114;
constexpr int list_name_x = -100;
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

// ─── 상점 (R) ────────────────────────────────────────────────────────

void run_shop(hs::economy& eco, bn::seed_random& random)
{
    bn::sprite_text_generator left_generator(hs::font);
    left_generator.set_left_alignment();

    bn::sprite_text_generator right_generator(hs::font);
    right_generator.set_right_alignment();

    constexpr int top_y = -50;
    constexpr int step_y = 16;

    text_line title_text(left_generator, -114, -72);
    text_line money_text(right_generator, 114, -72);
    text_line status_text(left_generator, -114, 50);
    text_line footer_text(left_generator, -114, 68);

    bn::vector<bn::sprite_ptr, 48> row_sprites;

    for(int index = 0; index < hs::shop_item_count; ++index)
    {
        const hs::shop_item& item = hs::shop_items[index];
        left_generator.generate(list_name_x, top_y + index * step_y, item.name, row_sprites);
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

// ─── 창고 (L): 인벤토리 — 잡템 / 보관한 검 ──────────────────────────

constexpr int list_top_y = -52;
constexpr int list_step_y = 14;

void run_storage(hs::economy& eco)
{
    bn::sprite_text_generator left_generator(hs::font);
    left_generator.set_left_alignment();

    bn::sprite_text_generator right_generator(hs::font);
    right_generator.set_right_alignment();

    constexpr int quantity_x = 0;

    text_line title_text(left_generator, -114, -72);
    text_line ticket_text(right_generator, 114, -72);
    text_line footer_text(left_generator, -114, 68);

    bn::vector<bn::sprite_ptr, 80> row_sprites;
    list_cursor cursor(left_generator, list_top_y, list_step_y);

    bool item_tab = true;
    int selected = 0;
    int frame = 0;
    int notice = 0;

    // 잡템 탭은 보기만 하고(쓰는 곳은 조합소), 보관한 검 탭에서만 고를 수 있다
    auto selectable_count = [&]()
    {
        return item_tab ? 0 : eco.stored_swords().size();
    };

    auto footer_hint = [&]()
    {
        footer_text.set_text(item_tab ? "B 돌아가기  L/R 탭" : "A 꺼내기  B 돌아가기  L/R 탭");
    };

    auto refresh = [&]()
    {
        row_sprites.clear();

        bn::string<32> ticket_line("방지권 ");
        ticket_line += bn::to_string<8>(eco.tickets());
        ticket_text.set_text(ticket_line);

        int row = 0;

        if(item_tab)
        {
            title_text.set_text("창고 - 잡템");

            for(int index = 0; index < hs::material_count; ++index)
            {
                int quantity = eco.material_quantity(hs::material(index));

                if(quantity > 0)
                {
                    int y = list_top_y + row * list_step_y;
                    bn::string<16> quantity_line("x");
                    quantity_line += bn::to_string<8>(quantity);
                    left_generator.generate(list_name_x, y, hs::material_names[index], row_sprites);
                    left_generator.generate(quantity_x, y, quantity_line, row_sprites);
                    ++row;
                }
            }
        }
        else
        {
            bn::string<48> title_line("창고 - 보관한 검  지금 +");
            title_line += bn::to_string<4>(eco.level());
            title_text.set_text(title_line);

            for(int8_t level : eco.stored_swords())
            {
                left_generator.generate(list_name_x, list_top_y + row * list_step_y, sword_label(level), row_sprites);
                ++row;
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

        if(bn::keypad::l_pressed() || bn::keypad::r_pressed())
        {
            item_tab = ! item_tab;
            selected = 0;
            notice = 0;
            refresh();
            footer_hint();
        }
        else if(count > 0 && bn::keypad::up_pressed())
        {
            selected = (selected + count - 1) % count;
        }
        else if(count > 0 && bn::keypad::down_pressed())
        {
            selected = (selected + 1) % count;
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

// ─── 조합소 (↓): 잡템 → 방지권 / 검(창고로) ──────────────────────────

void run_crafting(hs::economy& eco)
{
    bn::sprite_text_generator left_generator(hs::font);
    left_generator.set_left_alignment();

    bn::sprite_text_generator right_generator(hs::font);
    right_generator.set_right_alignment();

    constexpr int count_x = 4;
    constexpr int output_x = 14;

    text_line title_text(left_generator, -114, -72);
    text_line ticket_text(right_generator, 114, -72);
    text_line footer_text(left_generator, -114, 68);

    bn::vector<bn::sprite_ptr, 80> row_sprites;
    list_cursor cursor(left_generator, list_top_y, list_step_y);

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
            int y = list_top_y + index * list_step_y;

            bn::string<16> count_line = bn::to_string<16>(eco.material_quantity(recipe.input));
            count_line += "/";
            count_line += bn::to_string<4>(eco.required_quantity(recipe));

            bn::string<32> output_line;

            if(recipe.output == hs::craft_output::PROTECT)
            {
                output_line = "방지권 x";
                output_line += bn::to_string<4>(eco.output_amount(recipe));
            }
            else
            {
                output_line = "+";
                output_line += bn::to_string<4>(eco.output_amount(recipe));
                output_line += " 검";
            }

            left_generator.generate(list_name_x, y, hs::material_names[int(recipe.input)], row_sprites);
            right_generator.generate(count_x, y, count_line, row_sprites);
            left_generator.generate(output_x, y, output_line, row_sprites);
        }
    };

    title_text.set_text("조합소");
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

    for(const cutscene_scene& scene : opening_scenes)
    {
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

        fade_screen(false);
    }
}

}

int main()
{
    bn::core::init();
    run_splash();

#ifdef HS_DEBUG
    constexpr bool infinite_unlocked = true;
#else
    constexpr bool infinite_unlocked = false;   // 스토리 클리어 시 해금 (세이브 연동은 나중에)
#endif

    unsigned seed = 0;
    title_choice choice = run_title(infinite_unlocked, seed);

    // STORY = 이지(오프닝 컷씬부터), INFINITE = 하드 (스토리 클리어 후 해금)
    if(choice == title_choice::STORY)
    {
        run_opening_cutscene();
    }

    hs::economy eco(choice == title_choice::STORY ? hs::difficulty::EASY : hs::difficulty::HARD);
    bn::seed_random random(seed);
    bn::string_view message = "+20까지 강화하자";

    while(true)
    {
        forge_exit exit = run_forge(eco, random, message);
        message = "";

        switch(exit)
        {

        case forge_exit::SHOP:
            run_shop(eco, random);
            break;

        case forge_exit::STORAGE:
            run_storage(eco);
            break;

        case forge_exit::CRAFTING:
            run_crafting(eco);
            break;

        default:
            break;
        }
    }
}
