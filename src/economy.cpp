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

    if(random.get_int(100) < row().success_pct)
    {
        ++_level;
        return true;
    }

    return false;
}

void economy::protect()
{
    BN_ASSERT(can_protect(), "Invalid protect");

    _tickets -= row().protect_cost;
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

    _level = 0;
    return result;
}

int32_t economy::sell()
{
    BN_ASSERT(can_sell(), "Invalid sell");

    int32_t earned = row().sell_usd;
    _add_money(earned);
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
}

void economy::store_sword()
{
    BN_ASSERT(can_store_sword(), "Invalid store");

    _stored_swords.push_back(int8_t(_level));
    _level = 0;
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
#endif

void economy::_add_money(int32_t amount)
{
    _money = amount > money_cap - _money ? money_cap : _money + amount;
}

}
