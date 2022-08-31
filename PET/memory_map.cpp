#include "memory_map.hpp"

#include <iostream>
#include <iomanip>

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

static const AddressRange RAM_RANGE{    0x0000, 0x2000 };
static const AddressRange VRAM_RANGE{   0x8000, 0x0400 };

#if defined(SERIES4)
static const AddressRange BASIC_RANGE{  0xB000, 0x3000 };
#else
static const AddressRange BASIC_RANGE{  0xC000, 0x2000 };
#endif

static const AddressRange EDITOR_RANGE{ 0xE000, 0x0800 };
static const AddressRange IO_RANGE{     0xE800, 0x0050 };
static const AddressRange KERNAL_RANGE{ 0xF000, 0x1000 };

#if not TEST
u8 MemoryMap::load8(u16 address)
{
    u16 offset;
    if (RAM_RANGE.contains(address, offset))
        return ram[offset];
    if (VRAM_RANGE.contains(address, offset))
        return vram[offset];
    if (BASIC_RANGE.contains(address, offset))
        return basic[offset];
    if (EDITOR_RANGE.contains(address, offset))
        return editor[offset];
    if (IO_RANGE.contains(address, offset))
        return io.load8(offset);
    if (KERNAL_RANGE.contains(address, offset))
        return kernal[offset];

    std::cerr << "Unhandled load8 at: " << std::hex << std::setw(4) << std::setfill('0') << address << '\n';

    return 0;
}

u16 MemoryMap::load16(u16 address)
{
    u16 offset;
    if (RAM_RANGE.contains(address, offset))
        return *reinterpret_cast<const u16*>(ram + offset);
    if (BASIC_RANGE.contains(address, offset))
        return *reinterpret_cast<const u16*>(basic + offset);
    if (EDITOR_RANGE.contains(address, offset))
        return *reinterpret_cast<const u16*>(editor + offset);
    if (KERNAL_RANGE.contains(address, offset))
        return *reinterpret_cast<const u16*>(kernal + offset);

    std::cerr << "Unhandled load16 at: " << std::hex << std::setw(4) << std::setfill('0') << address << '\n';
    return 0;
}

void MemoryMap::store8(u16 address, u8 data)
{
    u16 offset;
    if (RAM_RANGE.contains(address, offset))
    {
        ram[offset] = data;
        return;
    }
    if (VRAM_RANGE.contains(address, offset))
    {
        vram[offset] = data;
        return;
    }
    if (IO_RANGE.contains(address, offset))
    {
        io.store8(offset, data);
        return;
    }

    std::cerr << "Unhandled store8 at: " << std::hex << std::setw(4) << std::setfill('0') << address
              << " with: " << std::hex << std::setw(2) << std::setfill('0') << static_cast<u16>(data) << '\n';
}

void MemoryMap::store16(u16 address, u16 data)
{
    u16 offset;
    if (RAM_RANGE.contains(address, offset))
    {
        ram[offset] = data & 0xFF;
        ram[offset + 1] = data >> 8;
        return;
    }

    std::cerr << "Unhandled store16 at: " << std::hex << std::setw(4) << std::setfill('0') << address
              << " with: " << std::hex << std::setw(2) << std::setfill('0') << data << '\n';
}
#endif

void MemoryMap::clock()
{
    io.clock();
}
