#include "save.h"

#include "bn_sram.h"

namespace hs::save
{

namespace
{
    constexpr uint32_t header_magic = 0x4D534B48;    // "HKSM"
    constexpr uint32_t slot_magic = 0x544F4C53;      // "SLOT"
    constexpr uint8_t format_version = 1;

    struct header
    {
        uint32_t magic;
        uint8_t version;
        uint8_t hard_unlocked;
        uint16_t reserved;
        uint32_t checksum;
    };

    struct slot_data
    {
        uint32_t magic;
        uint8_t version;
        uint8_t diff;
        uint8_t level;
        uint8_t stored_count;
        int32_t money;
        int16_t tickets;
        int16_t materials[material_count];
        int8_t stored[max_stored_swords];
        uint8_t best_level;       // 예전 저장 파일은 이 자리가 0이라 현재 단계가 최고기록이 된다 (예전에는 uint16, 위쪽 바이트는 0)
        uint8_t cat_state;        // 아래쪽 6비트 = 강화 실패 횟수, 위쪽 2비트 = 고양이 목표 번호(0 없음, 1~3 = cat_targets[번호-1]). 예전 저장은 0
        uint32_t checksum;
    };

    static_assert(sizeof(slot_data) <= 64, "슬롯은 64바이트 칸에 들어가야 한다");

    // 기록 블록: 슬롯·머리글과 따로 두어 기존 저장을 건드리지 않는다
    constexpr uint32_t record_magic = 0x44434552;     // "RECD"
    constexpr uint8_t record_version = 1;
    constexpr int record_offset = 256;

    struct record_data
    {
        uint32_t magic;
        uint8_t version;
        uint8_t best_level;       // 0 = 기록 없음
        uint8_t gallery;          // 아래쪽 6비트 = 엔딩 도감 (모드*3 + 시간대)
        uint8_t reserved;
        uint16_t upgrades;
        uint16_t successes;
        uint16_t fails;
        uint16_t broken;
        uint16_t tickets_used;
        uint16_t reserved2;
        uint32_t peak_money;      // 가장 많이 가졌던 소지금
        uint32_t checksum;
    };

    static_assert(sizeof(record_data) <= 64, "기록 블록은 64바이트 안에 들어가야 한다");

    constexpr int header_offset = 0;
    constexpr int slot_stride = 64;
    constexpr int slots_offset = 16;

    constexpr int slot_offset(int slot)
    {
        return slots_offset + slot * slot_stride;
    }

    // 검사합: 마지막 checksum 칸을 뺀 모든 바이트
    template<typename Type>
    uint32_t checksum_of(const Type& value)
    {
        const uint8_t* bytes = reinterpret_cast<const uint8_t*>(&value);
        uint32_t hash = 2166136261u;

        for(int index = 0; index < int(sizeof(Type) - sizeof(uint32_t)); ++index)
        {
            hash = (hash ^ bytes[index]) * 16777619u;
        }

        return hash;
    }

    bool read_header(header& out)
    {
        bn::sram::read_offset(out, header_offset);
        return out.magic == header_magic && out.version == format_version && out.checksum == checksum_of(out);
    }

    void write_header(bool hard_unlocked)
    {
        header out = {};
        out.magic = header_magic;
        out.version = format_version;
        out.hard_unlocked = hard_unlocked ? 1 : 0;
        out.checksum = checksum_of(out);
        bn::sram::write_offset(out, header_offset);
    }

    bool read_slot(int slot, economy::snapshot& state)
    {
        slot_data data;
        bn::sram::read_offset(data, slot_offset(slot));

        if(data.magic != slot_magic || data.version != format_version || data.checksum != checksum_of(data) ||
           data.stored_count > max_stored_swords)
        {
            return false;
        }

        state.diff = difficulty(data.diff);
        state.money = data.money;
        state.level = data.level;
        state.best_level = data.best_level > max_level ? data.level : data.best_level;
        state.fail_count = data.cat_state & 0x3F;
        state.cat_target = (data.cat_state >> 6) == 0 ? 0 : cat_targets[(data.cat_state >> 6) - 1];
        state.tickets = data.tickets;

        for(int index = 0; index < material_count; ++index)
        {
            state.materials[index] = data.materials[index];
        }

        state.stored_swords.clear();

        for(int index = 0; index < data.stored_count; ++index)
        {
            state.stored_swords.push_back(data.stored[index]);
        }

        return true;
    }
}

slot_info info(int slot)
{
    slot_info result;
    economy::snapshot state;

    if(read_slot(slot, state))
    {
        // 값이 말이 되는지는 economy가 검사한다
        economy check(state.diff);

        if(check.restore(state))
        {
            result.used = true;
            result.diff = state.diff;
            result.level = state.level;
            result.money = state.money;
        }
    }

    return result;
}

bool any_used()
{
    for(int slot = 0; slot < slot_count; ++slot)
    {
        if(info(slot).used)
        {
            return true;
        }
    }

    return false;
}

void write(int slot, const economy& eco)
{
    header head;

    if(! read_header(head))
    {
        write_header(false);    // 처음 쓰는 카트리지: 머리글부터 만든다
    }

    economy::snapshot state = eco.take_snapshot();
    slot_data data = {};
    data.magic = slot_magic;
    data.version = format_version;
    data.diff = uint8_t(state.diff);
    data.level = uint8_t(state.level);
    data.best_level = uint8_t(state.best_level);

    int target_index = 0;

    for(int index = 0; index < 3; ++index)
    {
        if(state.cat_target == cat_targets[index])
        {
            target_index = index + 1;
        }
    }

    data.cat_state = uint8_t((state.fail_count > 63 ? 63 : state.fail_count) | (target_index << 6));
    data.stored_count = uint8_t(state.stored_swords.size());
    data.money = state.money;
    data.tickets = int16_t(state.tickets > 32767 ? 32767 : state.tickets);

    for(int index = 0; index < material_count; ++index)
    {
        data.materials[index] = state.materials[index];
    }

    for(int index = 0; index < state.stored_swords.size(); ++index)
    {
        data.stored[index] = state.stored_swords[index];
    }

    data.checksum = checksum_of(data);
    bn::sram::write_offset(data, slot_offset(slot));
}

bool load(int slot, economy& eco)
{
    economy::snapshot state;
    return read_slot(slot, state) && eco.restore(state);
}

namespace
{
    bool read_record(record_data& out)
    {
        bn::sram::read_offset(out, record_offset);
        return out.magic == record_magic && out.version == record_version && out.checksum == checksum_of(out);
    }

    void write_record(record_data& value)
    {
        value.magic = record_magic;
        value.version = record_version;
        value.checksum = checksum_of(value);
        bn::sram::write_offset(value, record_offset);
    }
}

best_record read_best()
{
    best_record result;
    record_data data;

    if(read_record(data) && data.best_level > 0)
    {
        result.valid = true;
        result.level = data.best_level;
        result.stats.upgrades = data.upgrades;
        result.stats.successes = data.successes;
        result.stats.fails = data.fails;
        result.stats.broken = data.broken;
        result.stats.tickets_used = data.tickets_used;
        result.stats.peak_money = data.peak_money;
    }

    return result;
}

void write_best(int level, const run_stats& stats)
{
    record_data data = {};

    if(! read_record(data))
    {
        data = {};
    }

    data.best_level = uint8_t(level);
    data.upgrades = stats.upgrades;
    data.successes = stats.successes;
    data.fails = stats.fails;
    data.broken = stats.broken;
    data.tickets_used = stats.tickets_used;
    data.peak_money = stats.peak_money;
    write_record(data);
}

void clear_best()
{
    record_data data = {};

    if(! read_record(data))
    {
        return;
    }

    const uint8_t gallery = data.gallery;
    data = {};
    data.gallery = gallery;
    write_record(data);
}

bool ending_seen(int index)
{
    record_data data;
    return read_record(data) && (data.gallery & (1 << index)) != 0;
}

void mark_ending(int index)
{
    record_data data = {};

    if(! read_record(data))
    {
        data = {};
    }

    data.gallery = uint8_t(data.gallery | (1 << index));
    write_record(data);
}

bool hard_unlocked()
{
    header head;
    return read_header(head) && head.hard_unlocked != 0;
}

void unlock_hard()
{
    write_header(true);
}

}
