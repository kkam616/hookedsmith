#include "economy.h"

#include "bn_assert.h"
#include "bn_seed_random.h"

namespace hs
{

namespace
{
    constexpr int16_t material_cap = 999;
}

economy::economy(difficulty diff) :
    _diff(diff),
    _money(diff == difficulty::HARD ? start_money_hard : start_money_easy)
{
    _stats.peak_money = uint32_t(_money);       // 시작 자금도 "가장 많이 가졌던 소지금"에 든다
}

bool economy::can_afford_materials() const
{
    const level_row& current = row();

    if(current.cost_sword > 0)
    {
        return stored_count(current.cost_sword) >= current.cost_qty;
    }

    if(current.cost_material != material::NONE)
    {
        return _materials[int(current.cost_material)] >= current.cost_qty;
    }

    return true;
}

int economy::stored_count(int level) const
{
    int count = 0;

    for(int8_t stored : _stored_swords)
    {
        if(stored == level)
        {
            ++count;
        }
    }

    return count;
}

void economy::_spend_materials()
{
    const level_row& current = row();

    if(current.cost_sword > 0)
    {
        for(int remaining = current.cost_qty; remaining > 0; --remaining)
        {
            for(int index = 0; index < _stored_swords.size(); ++index)
            {
                if(_stored_swords[index] == current.cost_sword)
                {
                    _stored_swords.erase(_stored_swords.begin() + index);
                    break;
                }
            }
        }
    }
    else if(current.cost_material != material::NONE)
    {
        _materials[int(current.cost_material)] -= current.cost_qty;
    }
}

bool economy::upgrade(bn::seed_random& random)
{
    BN_ASSERT(! at_max_level() && can_afford_upgrade(), "Invalid upgrade");

    _money -= row().cost_usd;
    _spend_materials();
    ++_stats.upgrades;

    if(random.get_int(100) < success_pct())
    {
        ++_level;
        ++_stats.successes;
        _note_best();
        return true;
    }

    ++_stats.fails;
    ++_fail_count;      // 방지권으로 막은 실패도, 파괴된 실패도 센다
    return false;
}

void economy::protect()
{
    BN_ASSERT(can_protect(), "Invalid protect");

    _tickets -= row().protect_cost;
    _stats.tickets_used = uint16_t(_stats.tickets_used + row().protect_cost);
}

drop economy::destroy(bn::seed_random& random)
{
    drop result;
    result.item = row().fail_drop;

    if(result.item != material::NONE)
    {
        int roll = random.get_int(100);

        for(int pct : drop_count_pct)
        {
            if(roll < pct)
            {
                break;
            }

            roll -= pct;
            ++result.quantity;
        }

        int16_t& stock = _materials[int(result.item)];
        int total = stock + result.quantity;
        stock = int16_t(total > material_cap ? material_cap : total);
    }

    ++_stats.broken;
    _level = 0;
    return result;
}

int32_t economy::sell()
{
    BN_ASSERT(can_sell(), "Invalid sell");

    int32_t earned = row().sell_usd;
    _add_money(earned);
    _last_sale = earned;
    _level = 0;
    return earned;
}

int32_t economy::price(const shop_item& item) const
{
    return _diff == difficulty::HARD ? item.hard_usd : item.easy_usd;
}

buy_result economy::buy(const shop_item& item, bn::seed_random& random)
{
    int32_t cost = price(item);

    if(_money < cost)
    {
        return buy_result::NOT_ENOUGH_MONEY;
    }

    if(item.effect == shop_effect::WARP && _level != 0)
    {
        return buy_result::WARP_NEEDS_PLUS_ZERO;
    }

    _money -= cost;

    if(item.effect == shop_effect::PROTECT)
    {
        _tickets += item.amount;
        return buy_result::OK;
    }

    if(random.get_int(100) < warp_success_pct)
    {
        _level = item.amount;
        _note_best();
        return buy_result::WARP_SUCCESS;
    }

    return buy_result::WARP_FAILED;
}

int economy::required_quantity(const recipe& recipe) const
{
    return _diff == difficulty::HARD ? recipe.hard_qty : recipe.easy_qty;
}

int economy::output_amount(const recipe& recipe) const
{
    return _diff == difficulty::HARD ? recipe.hard_amount : recipe.easy_amount;
}

craft_result economy::craft(const recipe& recipe)
{
    int16_t& stock = _materials[int(recipe.input)];
    int required = required_quantity(recipe);

    if(stock < required)
    {
        return craft_result::NOT_ENOUGH_MATERIAL;
    }

    if(recipe.output == craft_output::SWORD && _stored_swords.full())
    {
        return craft_result::STORAGE_FULL;
    }

    stock -= required;

    if(recipe.output == craft_output::PROTECT)
    {
        _tickets += output_amount(recipe);
    }
    else
    {
        _stored_swords.push_back(int8_t(output_amount(recipe)));
    }

    return craft_result::OK;
}

void economy::take_out_sword(int index)
{
    BN_ASSERT(index >= 0 && index < _stored_swords.size(), "Invalid sword index: ", index);

    int stored_level = _stored_swords[index];

    if(_level == 0)
    {
        _stored_swords.erase(_stored_swords.begin() + index);
    }
    else
    {
        _stored_swords[index] = int8_t(_level);
    }

    _level = stored_level;
    _note_best();
}

void economy::store_sword()
{
    BN_ASSERT(can_store_sword(), "Invalid store");

    _stored_swords.push_back(int8_t(_level));
    _level = 0;
}

bool economy::is_bankrupt() const
{
    if(at_max_level() || can_afford_upgrade() || can_sell() || ! _stored_swords.empty())
    {
        return false;
    }

    for(const shop_item& item : shop_items)
    {
        if(_money >= price(item))
        {
            return false;
        }
    }

    for(const recipe& entry : recipes)
    {
        bool storage_blocked = entry.output == craft_output::SWORD && _stored_swords.full();

        if(! storage_blocked && _materials[int(entry.input)] >= required_quantity(entry))
        {
            return false;
        }
    }

    return true;
}

economy::snapshot economy::take_snapshot() const
{
    snapshot state;
    state.diff = _diff;
    state.money = _money;
    state.level = _level;
    state.best_level = _best_level;
    state.fail_count = _fail_count;
    state.cat_target = _cat_target;
    state.tickets = _tickets;
    state.materials = _materials;
    state.stored_swords = _stored_swords;
    return state;
}

bool economy::restore(const snapshot& state)
{
    if(state.diff != difficulty::EASY && state.diff != difficulty::HARD)
    {
        return false;
    }

    if(state.money < 0 || state.money > money_cap || state.level < 0 || state.level > max_level ||
       state.tickets < 0 || state.tickets > 32767)      // 세이브에 int16으로 저장한다
    {
        return false;
    }

    bool target_ok = state.cat_target == 0;

    for(int target : cat_targets)
    {
        target_ok |= state.cat_target == target;
    }

    if(! target_ok || state.fail_count < 0 || state.fail_count > 63)
    {
        return false;
    }

    for(int16_t stock : state.materials)
    {
        if(stock < 0 || stock > material_cap)
        {
            return false;
        }
    }

    for(int8_t stored : state.stored_swords)
    {
        if(stored < 1 || stored > max_level)
        {
            return false;
        }
    }

    _diff = state.diff;
    _money = state.money;
    _level = state.level;
    _best_level = state.best_level < state.level ? state.level : state.best_level;
    _fail_count = state.fail_count;
    _cat_target = state.cat_target;
    _tickets = state.tickets;
    _materials = state.materials;
    _stored_swords = state.stored_swords;
    return true;
}

bool economy::thief_has_loot() const
{
    for(int8_t level : _stored_swords)
    {
        if(level >= thief_min_sword_level)
        {
            return true;
        }
    }

    return int64_t(_money) * thief_money_pct / 100 >= 1;
}

int32_t economy::pay_merchant()
{
    const int32_t before = _money;
    _add_money(_last_sale);
    _merchant_cooldown = merchant_cooldown_sales;
    return _money - before;
}

thief_loss economy::steal(bn::seed_random& random)
{
    thief_loss loss;
    int candidates[max_stored_swords];
    int count = 0;

    for(int index = 0; index < _stored_swords.size(); ++index)
    {
        if(_stored_swords[index] >= thief_min_sword_level)
        {
            candidates[count++] = index;
        }
    }

    if(count > 0)
    {
        const int chosen = candidates[random.get_int(count)];
        loss.is_sword = true;
        loss.level = _stored_swords[chosen];
        _stored_swords.erase(_stored_swords.begin() + chosen);
    }
    else
    {
        loss.money = int32_t(int64_t(_money) * thief_money_pct / 100);
        _money -= loss.money;
    }

    _thief_cooldown = thief_cooldown_actions;
    return loss;
}

void economy::ensure_cat_target(bn::seed_random& random)
{
    if(_cat_target == 0)
    {
        _cat_target = cat_targets[random.get_int(3)];
        _fail_count = 0;
    }
}

cat_gift economy::cat_visit(bn::seed_random& random)
{
    cat_gift gift;

    if(! _stored_swords.full())
    {
        gift.is_sword = true;
        gift.level = cat_gift_levels[random.get_int(4)];
        _stored_swords.push_back(int8_t(gift.level));
    }
    else
    {
        gift.tickets = cat_gift_tickets;
        _tickets += gift.tickets;
    }

    _cat_target = 0;
    ensure_cat_target(random);
    return gift;
}

#ifdef HS_DEBUG
void economy::debug_grant()
{
    _add_money(100000);
    _tickets += 5;

    for(int8_t level : { 19, 21, 21, 22 })
    {
        if(! _stored_swords.full())
        {
            _stored_swords.push_back(level);
        }
    }

    for(int16_t& stock : _materials)
    {
        stock = int16_t(stock + 5 > material_cap ? material_cap : stock + 5);
    }
}

void economy::debug_bankrupt()
{
    _money = 0;
    _level = 0;
    _tickets = 0;
    _materials = {};
    _stored_swords.clear();
}
#endif

void economy::_add_money(int32_t amount)
{
    _money = amount > money_cap - _money ? money_cap : _money + amount;

    if(uint32_t(_money) > _stats.peak_money)
    {
        _stats.peak_money = uint32_t(_money);
    }
}

}
