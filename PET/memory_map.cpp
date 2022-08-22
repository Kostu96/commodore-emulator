#include "memory_map.hpp"

struct AddressRange
{
    uint16_t start;
    uint16_t size;

    bool contains(uint16_t address, uint16_t& offset) const
    {
        offset = address - start;
        return (address >= start) && (address < start + size);
    }
};

static const AddressRange KERNAL_RANGE{ 0xF000, 4096 };

u8 MemoryMap::load8(u16 address)
{
    u16 offset;
    if (KERNAL_RANGE.contains(address, offset))
        return kernal2[offset];

    return 0;
}

u16 MemoryMap::load16(u16 address)
{
    u16 offset;
    if (KERNAL_RANGE.contains(address, offset))
    {
        u8 b0 = kernal2[offset];
        u8 b1 = kernal2[offset + 1];

        return b0 | (b1 << 8);
    }

    return 0;
}
