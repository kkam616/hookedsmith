#ifndef HS_SAVE_H
#define HS_SAVE_H

#include "economy.h"

// SRAM 세이브: 슬롯 3개 + 공통 머리글(HARD 해금). 슬롯마다 검사합을 두어 깨진 슬롯은 빈 슬롯으로 본다.
namespace hs::save
{

constexpr int slot_count = 3;

struct slot_info
{
    bool used = false;
    difficulty diff = difficulty::EASY;
    int level = 0;
    int32_t money = 0;
};

[[nodiscard]] slot_info info(int slot);
[[nodiscard]] bool any_used();

void write(int slot, const economy& eco);

// 슬롯을 읽어 eco에 넣는다. 비었거나 깨졌으면 false (eco는 그대로)
[[nodiscard]] bool load(int slot, economy& eco);

[[nodiscard]] bool hard_unlocked();
void unlock_hard();

}

#endif
