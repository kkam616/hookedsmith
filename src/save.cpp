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
        uint16_t reserved;
        uint32_t checksum;
    };

    static_assert(sizeof(slot_data) <= 64, "슬롯은 64바이트 칸에 들어가야 한다");

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
