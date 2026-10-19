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

// ─ 기록 (슬롯과 별개의 독립 블록): HARD 최고 도달 기록 + 엔딩 도감 ─
struct best_record
{
    bool valid = false;         // 아직 기록이 없으면 false
    int level = 0;              // 도달한 최고 단계
    run_stats stats;            // 그 단계에 닿은 순간까지의 숫자
    int band = -1;              // 그 순간의 시간대 (0 낮 / 1 노을 / 2 밤, -1 = 모름: 예전 기록)
};

[[nodiscard]] best_record read_best();
void write_best(int level, const run_stats& stats, int band);        // 새 최고 단계에 닿았을 때 (band = 그 순간의 시간대)
void clear_best();                                         // 최고 도달 기록만 지운다 (엔딩 도감은 그대로)

// 엔딩 도감: 번호 = 모드*3 + 시간대 (모드 0 STORY / 1 HARD, 시간대 0 낮 / 1 노을 / 2 밤)
[[nodiscard]] bool ending_seen(int index);
void mark_ending(int index);

}

#endif
