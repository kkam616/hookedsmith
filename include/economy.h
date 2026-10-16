#ifndef HS_ECONOMY_H
#define HS_ECONOMY_H

#include "bn_array.h"
#include "bn_vector.h"

#include "economy_data.h"

namespace bn
{
    class seed_random;
}

namespace hs
{

struct drop
{
    material item = material::NONE;
    int quantity = 0;
};

enum class buy_result
{
    OK,
    NOT_ENOUGH_MONEY,
    WARP_NEEDS_PLUS_ZERO,
    WARP_SUCCESS,
    WARP_FAILED,
};

enum class craft_result
{
    OK,
    NOT_ENOUGH_MATERIAL,
    STORAGE_FULL,
};

// 세이브에 들어갈 상태 전부 (난이도, 소지금, 단계, 방지권, 잡템, 보관 검)
class economy
{

public:
    explicit economy(difficulty diff);

    [[nodiscard]] difficulty diff() const { return _diff; }
    [[nodiscard]] int32_t money() const { return _money; }
    [[nodiscard]] int level() const { return _level; }
    [[nodiscard]] int tickets() const { return _tickets; }
    [[nodiscard]] int material_quantity(material item) const { return _materials[int(item)]; }
    [[nodiscard]] const bn::ivector<int8_t>& stored_swords() const { return _stored_swords; }

    [[nodiscard]] const level_row& row() const { return level_table(_diff)[_level]; }

    // 시간대 보정: 성공률에서 이 %p를 뺀다 (낮 0 / 노을 2 / 밤 4, time_band.h). 저장하지 않고 시계에서 매번 정한다.
    void set_time_penalty(int pct) { _time_penalty = pct; }
    [[nodiscard]] int time_penalty() const { return _time_penalty; }

    // 보정을 적용한 실제 성공률(%). 0 아래로는 내려가지 않는다.
    [[nodiscard]] int success_pct() const
    {
        int pct = row().success_pct - _time_penalty;
        return pct < 0 ? 0 : pct;
    }

    // 강화: 비용을 내고 판정한다. 실패해도 단계는 그대로이고, 호출한 쪽이 protect() 또는 destroy()를 고른다.
    [[nodiscard]] bool at_max_level() const { return _level == max_level; }
    [[nodiscard]] bool can_afford_money() const { return _money >= row().cost_usd; }
    [[nodiscard]] bool can_afford_materials() const;
    [[nodiscard]] bool can_afford_upgrade() const { return can_afford_money() && can_afford_materials(); }
    [[nodiscard]] bool upgrade(bn::seed_random& random);

    [[nodiscard]] bool protect_available() const { return row().protect_cost > 0; }
    [[nodiscard]] bool can_protect() const { return protect_available() && _tickets >= row().protect_cost; }
    void protect();
    drop destroy(bn::seed_random& random);

    [[nodiscard]] bool can_sell() const { return row().sell_usd > 0; }
    int32_t sell();

    // 파산(게임 오버): 강화할 수 없고, 팔 검이 없고(현재 검 판매 불가 + 창고 비어 있음),
    // 상점에서 가장 싼 물건도 못 사고, 화구에서 어떤 교환도 못 하는 상태. 최종 검(+29)에서는 아니다.
    [[nodiscard]] bool is_bankrupt() const;

    [[nodiscard]] int32_t price(const shop_item& item) const;
    buy_result buy(const shop_item& item, bn::seed_random& random);

    [[nodiscard]] int required_quantity(const recipe& recipe) const;
    [[nodiscard]] int output_amount(const recipe& recipe) const;
    craft_result craft(const recipe& recipe);

    // 보관한 검을 꺼내 현재 검과 바꾼다. 현재 검이 +0이면 버리고 꺼낸다.
    void take_out_sword(int index);

    // 현재 검을 보관함에 넣고 +0으로 돌아간다 (+0 검이나 가득 찬 보관함은 불가)
    [[nodiscard]] bool can_store_sword() const { return _level > 0 && ! _stored_swords.full(); }
    void store_sword();

    // 보관한 검 중 해당 단계의 개수
    [[nodiscard]] int stored_count(int level) const;

    // 세이브용: 상태 전부를 꺼내거나 되돌린다. restore는 값이 말이 안 되면(깨진 세이브) false를 돌려주고 아무것도 바꾸지 않는다.
    struct snapshot
    {
        difficulty diff = difficulty::EASY;
        int32_t money = 0;
        int level = 0;
        int tickets = 0;
        bn::array<int16_t, material_count> materials = {};
        bn::vector<int8_t, max_stored_swords> stored_swords;
    };

    [[nodiscard]] snapshot take_snapshot() const;
    [[nodiscard]] bool restore(const snapshot& state);

#ifdef HS_DEBUG
    // 테스트용: 돈 $100,000, 방지권 5장, 잡템 각 5개, 보관 검 +19 ×1·+21 ×2·+22 ×1
    void debug_grant();

    // 테스트용: 돈 0, +0 검, 방지권·잡템·보관 검 모두 비움 (게임 오버 확인용)
    void debug_bankrupt();
#endif

private:
    difficulty _diff;
    int32_t _money = 0;
    int _level = 0;
    int _tickets = 0;
    bn::array<int16_t, material_count> _materials = {};
    bn::vector<int8_t, max_stored_swords> _stored_swords;
    int _time_penalty = 0;

    void _add_money(int32_t amount);
    void _spend_materials();
};

}

#endif
