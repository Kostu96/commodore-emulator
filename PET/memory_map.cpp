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

static const AddressRange RAM_RANGE{       0x0000, 4096 };
static const AddressRange DISPLAY_RANGE{   0x8000, 1000 };
static const AddressRange DISPLAYEX_RANGE{ 0x83E8, 1000 };
static const AddressRange BASIC_RANGE{     0xC000, 8192 };
static const AddressRange EDITOR_RANGE{    0xE000, 2048 };
static const AddressRange PIA1_RANGE{      0xE810, 4 };
static const AddressRange PIA2_RANGE{      0xE820, 4 };
static const AddressRange VIA_RANGE{       0xE840, 16 };
static const AddressRange KERNAL_RANGE{    0xF000, 4096 };

u8 MemoryMap::load8(u16 address)
{
    u16 offset;
    if (RAM_RANGE.contains(address, offset))
        return ram[offset];
    if (BASIC_RANGE.contains(address, offset))
        return basic2[offset];
    if (EDITOR_RANGE.contains(address, offset))
        return editor2[offset];
    if (PIA1_RANGE.contains(address, offset))
        return pia1.load8(offset);
    if (PIA2_RANGE.contains(address, offset))
        return pia2.load8(offset);
    if (VIA_RANGE.contains(address, offset))
        return via.load8(offset);
    if (KERNAL_RANGE.contains(address, offset))
        return kernal2[offset];

    std::cerr << "Unhandled load8 at: " << std::hex << std::setw(4) << std::setfill('0') << address << '\n';
    return 0xFF;
}

u16 MemoryMap::load16(u16 address)
{
    u16 offset;
    if (RAM_RANGE.contains(address, offset))
        return *reinterpret_cast<const u16*>(ram + offset);
    if (BASIC_RANGE.contains(address, offset))
        return *reinterpret_cast<const u16*>(basic2 + offset);
    if (EDITOR_RANGE.contains(address, offset))
        return *reinterpret_cast<const u16*>(editor2 + offset);
    if (KERNAL_RANGE.contains(address, offset))
        return *reinterpret_cast<const u16*>(kernal2 + offset);

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
    if (DISPLAY_RANGE.contains(address, offset))
    {
        display[offset] = data;
        updateScreen(offset, data);
        return;
    }
    if (DISPLAYEX_RANGE.contains(address, offset))
    {
        std::cerr << "Unhandled store8 to DISPLAYEX\n";
        return;
    }
    if (PIA1_RANGE.contains(address, offset))
    {
        pia1.store8(offset, data);
        return;
    }
    if (PIA2_RANGE.contains(address, offset))
    {
        pia2.store8(offset, data);
        return;
    }
    if (VIA_RANGE.contains(address, offset))
    {
        via.store8(offset, data);
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

void MemoryMap::clock()
{
    via.clock();
}
