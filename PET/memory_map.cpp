#include "memory_map.hpp"

#include <iostream>

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

static const AddressRange STACK_RANGE{  0x0100, 256 };
static const AddressRange EDITOR_RANGE{ 0xE000, 2048 };
static const AddressRange KERNAL_RANGE{ 0xF000, 4096 };

u8 MemoryMap::load8(u16 address)
{
    u16 offset;
    if (EDITOR_RANGE.contains(address, offset))
        return editor4[offset];
    else if (KERNAL_RANGE.contains(address, offset))
        return kernal4[offset];

    std::cerr << "Unhandled load8 at: " << std::hex << address << '\n';
    return 0;
}

u16 MemoryMap::load16(u16 address)
{
    u16 offset;
    if (EDITOR_RANGE.contains(address, offset))
    {
        u8 b0 = editor4[offset];
        u8 b1 = editor4[offset + 1];

        return b0 | (b1 << 8);
    }
    else if (KERNAL_RANGE.contains(address, offset))
    {
        u8 b0 = kernal4[offset];
        u8 b1 = kernal4[offset + 1];

        return b0 | (b1 << 8);
    }

    std::cerr << "Unhandled load16 at: " << std::hex << address << '\n';
    return 0;
}

void MemoryMap::store8(u16 address, u8 data)
{

    std::cerr << "Unhandled store8 at: " << std::hex << address
              << " with: " << std::hex << (u16)data << '\n';
}

void MemoryMap::store16(u16 address, u16 data)
{
    u16 offset;
    if (STACK_RANGE.contains(address, offset))
    {
        stack[offset] = data & 0xFF;
        stack[offset + 1] = data >> 8;
    }

    std::cerr << "Unhandled store16 at: " << std::hex << address
              << " with: " << std::hex << (u16)data << '\n';
}
