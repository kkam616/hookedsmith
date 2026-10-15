#include "bn_core.h"
#include "bn_bg_palettes.h"
#include "bn_sprite_palettes.h"
#include "bn_color.h"
#include "bn_fixed.h"
#include "bn_keypad.h"
#include "bn_random.h"
#include "bn_regular_bg_ptr.h"
#include "bn_sprite_palette_ptr.h"
#include "bn_sprite_ptr.h"
#include "bn_sprite_text_generator.h"
#include "bn_string.h"
#include "bn_time.h"
#include "bn_vector.h"

#include "bn_regular_bg_items_title_day.h"
#include "bn_regular_bg_items_title_night.h"
#include "bn_regular_bg_items_title_sunset.h"
#include "bn_regular_bg_items_splash_studio.h"
#include "bn_regular_bg_items_game_frame.h"
#include "bn_sprite_items_sword.h"
#include "hs_font.h"

namespace
{

constexpr int max_level = 20;
constexpr int start_gold = 300;
constexpr int loan_gold = 100;

// 프로토타입용 임시 수치. 단계별 확률·비용표는 다음 작업에서 다듬는다.
struct level_info
{
    int success_pct;    // 이 단계에서 다음 단계로 갈 확률
    int destroy_pct;    // 실패했을 때 파괴될 확률
    int cost;           // 강화 비용
    int price;          // 이 단계 검의 판매가
};

constexpr level_info levels[max_level + 1] = {
    { 95,   0,     10,      0 },   // +0
    { 90,   0,     20,     30 },
    { 85,   0,     30,     70 },
    { 80,  10,     50,    130 },
    { 75,  20,     80,    220 },
    { 70,  30,    120,    360 },   // +5
    { 65,  40,    180,    560 },
    { 60,  50,    260,    850 },
    { 55,  55,    380,   1300 },
    { 50,  60,    550,   1950 },
    { 45,  65,    800,   2900 },   // +10
    { 40,  70,   1150,   4300 },
    { 35,  75,   1650,   6400 },
    { 30,  80,   2400,   9500 },
    { 27,  85,   3500,  14000 },
    { 24,  90,   5000,  21000 },   // +15
    { 21,  90,   7200,  31000 },
    { 18,  95,  10000,  46000 },
    { 15,  95,  14000,  68000 },
    { 12,  95,  20000, 100000 },
    {  0,   0,      0, 200000 },   // +20
};

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
    bn::string<24> result;
    int size = digits.size();

    for(int index = 0; index < size; ++index)
    {
        if(index > 0 && (size - index) % 3 == 0)
        {
            result.push_back(',');
        }

        result.push_back(digits[index]);
    }

    result.append("원");
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

enum class phase
{
    IDLE,
    HAMMERING,
    SUCCESS,
    FAIL,
    DESTROYED,
};

constexpr int hammer_frames = 40;
constexpr int success_frames = 24;
constexpr int fail_frames = 24;
constexpr int sword_y = -28;

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
// fade_level: 0 = 완전 검정(불투명), 16 = 완전 표시
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
title_choice run_title(bool infinite_unlocked)
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

    // 잠긴 메뉴는 전용 팔레트를 회색 쪽으로 바래게 한다 (다른 글자 팔레트는 그대로)
    bn::sprite_palette_ptr locked_palette = bn::sprite_palette_ptr::create_new(hs::font.item().palette_item());
    locked_palette.set_fade(bn::color(12, 12, 12), bn::fixed(0.6));

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
    int frame = 0;

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
                fade_screen(false);
                return title_choice(selected);
            }
        }

        if(notice_frames > 0 && --notice_frames == 0)
        {
            notice_text.set_text("");
        }

        // 커서는 살짝 좌우로 흔들린다
        cursor.set_position(cursor_origin.x() - (frame / 16) % 2, cursor_origin.y() + selected * menu_step_y);

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

void run_game()
{
    bn::sprite_text_generator left_generator(hs::font);
    left_generator.set_left_alignment();

    bn::sprite_text_generator center_generator(hs::font);
    center_generator.set_center_alignment();

    bn::sprite_text_generator right_generator(hs::font);
    right_generator.set_right_alignment();

    // 박스 테두리는 배경 그림(tools/make_game_frame.py), 글자는 그 박스 좌표에 맞춘다
    bn::bg_palettes::set_transparent_color(bn::color(4, 3, 3));
    bn::regular_bg_ptr frame_bg = bn::regular_bg_items::game_frame.create_bg(title_bg_x, title_bg_y);

    // 고정 라벨: 창고(L) / 상점(R) / 판매(B) / 강화(A)
    bn::vector<bn::sprite_ptr, 16> label_sprites;
    center_generator.generate(-92, -61, "창고", label_sprites);
    center_generator.generate(-92, -46, "(L키)", label_sprites);
    center_generator.generate(92, -61, "상점", label_sprites);
    center_generator.generate(92, -46, "(R키)", label_sprites);
    center_generator.generate(93, 10, "판매", label_sprites);
    center_generator.generate(93, 25, "B", label_sprites);
    center_generator.generate(57, 40, "강화", label_sprites);
    center_generator.generate(57, 55, "A", label_sprites);
    left_generator.generate(-114, 70, "돈", label_sprites);

    text_line message_text(center_generator, 0, -68);
    text_line cost_text(left_generator, -114, -16);
    text_line price_text(left_generator, -114, 0);
    text_line name_text(center_generator, 0, 18);
    text_line chance_text(center_generator, 0, 34);
    text_line ticket_text(left_generator, -114, 52);
    text_line money_text(right_generator, 114, 70);

    bn::sprite_ptr sword = bn::sprite_items::sword.create_sprite(0, sword_y);
    bn::sprite_palette_ptr sword_palette = sword.palette();

    bn::random random;
    int gold = start_gold;
    int level = 0;
    int tickets = 0;    // 깨짐 방지권 — 상점이 생기면 늘어난다
    phase current_phase = phase::IDLE;
    int phase_frame = 0;

    auto refresh_status = [&]()
    {
        const level_info& info = levels[level];
        bool destroyed = current_phase == phase::DESTROYED;

        bn::string<32> cost_line("강화비용 ");
        cost_line += level == max_level ? bn::string<24>("-") : format_money(info.cost);
        cost_text.set_text(cost_line);

        bn::string<32> price_line("판매가격 ");
        price_line += format_money(destroyed ? 0 : info.price);
        price_text.set_text(price_line);

        bn::string<48> name_line;

        if(destroyed)
        {
            name_line = "부서진 검";
        }
        else
        {
            name_line = "+";
            name_line += bn::to_string<4>(level);
            name_line += " ";
            name_line += sword_name(level);
        }

        name_text.set_text(name_line);

        bn::string<32> chance_line;

        if(destroyed)
        {
            chance_line = "A 새 검 받기";
        }
        else if(level == max_level)
        {
            chance_line = "최고 단계";
        }
        else
        {
            chance_line = "성공률 ";
            chance_line += bn::to_string<4>(info.success_pct);
            chance_line += "%";
        }

        chance_text.set_text(chance_line);

        bn::string<24> ticket_line("방지권 ");
        ticket_line += bn::to_string<8>(tickets);
        ticket_text.set_text(ticket_line);

        money_text.set_text(format_money(gold));

        sword.set_visible(! destroyed);
        set_sword_tint(sword_palette, level);
    };

    refresh_status();
    message_text.set_text("+20까지 강화하자");
    fade_screen(true);

    while(true)
    {
        // 입력 타이밍을 난수에 섞는다
        random.update();

        switch(current_phase)
        {

        case phase::IDLE:
            if(bn::keypad::a_pressed())
            {
                if(level == max_level)
                {
                    message_text.set_text("이미 전설이 되었다");
                }
                else if(gold < levels[level].cost)
                {
                    message_text.set_text("돈이 모자란다");
                }
                else
                {
                    gold -= levels[level].cost;
                    current_phase = phase::HAMMERING;
                    phase_frame = 0;
                    refresh_status();
                    message_text.set_text("땅! 땅! 땅!");
                }
            }
            else if(bn::keypad::b_pressed())
            {
                if(level == 0)
                {
                    message_text.set_text("녹슨 검은 안 팔린다");
                }
                else
                {
                    bn::string<48> sold_line("판매 +");
                    sold_line += format_money(levels[level].price);
                    gold += levels[level].price;
                    level = 0;
                    refresh_status();
                    message_text.set_text(sold_line);
                }
            }
            else if(bn::keypad::l_pressed())
            {
                message_text.set_text("창고는 준비 중");
            }
            else if(bn::keypad::r_pressed())
            {
                message_text.set_text("상점은 준비 중");
            }
            break;

        case phase::HAMMERING:
            ++phase_frame;
            sword.set_y(sword_y + ((phase_frame / 4) % 2 == 0 ? 0 : 2));

            if(phase_frame >= hammer_frames)
            {
                sword.set_y(sword_y);
                phase_frame = 0;

                const level_info& info = levels[level];

                if(random.get_int(100) < info.success_pct)
                {
                    ++level;
                    current_phase = phase::SUCCESS;
                    refresh_status();

                    bn::string<32> success_line("강화 성공! +");
                    success_line += bn::to_string<4>(level);
                    message_text.set_text(success_line);
                }
                else if(random.get_int(100) < info.destroy_pct)
                {
                    level = 0;
                    current_phase = phase::DESTROYED;
                    refresh_status();
                    message_text.set_text("검이 부서졌다...");
                }
                else
                {
                    current_phase = phase::FAIL;
                    message_text.set_text("실패! 단계 유지");
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
                current_phase = phase::IDLE;
            }
            break;

        case phase::DESTROYED:
            if(bn::keypad::a_pressed() || bn::keypad::b_pressed())
            {
                current_phase = phase::IDLE;
                refresh_status();

                if(gold < levels[0].cost)
                {
                    gold += loan_gold;
                    refresh_status();
                    message_text.set_text("이웃이 빵값을 줬다");
                }
                else
                {
                    message_text.set_text("새 녹슨 검을 꺼냈다");
                }
            }
            break;

        default:
            break;
        }

        bn::core::update();
    }
}

}

int main()
{
    bn::core::init();
    run_splash();

    // 지금은 STORY만 고를 수 있다 (INFINITE는 잠김, OPTION은 안내만 띄운다)
    run_title(false);
    run_game();
}
