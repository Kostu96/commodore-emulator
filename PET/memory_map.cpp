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

static const AddressRange TAPEBUFFERPTR_RANGE{ 0x00D6, 2 };
static const AddressRange IOSTARTPTR_RANGE{    0x00FB, 2 };

static const AddressRange ZEROPAGE_RANGE{  0x0000, 256 };
static const AddressRange STACK_RANGE{     0x0100, 256 };
static const AddressRange INPUT_RANGE{     0x0200, 80 };
static const AddressRange DISPLAY_RANGE{   0x8000, 1000 };
static const AddressRange DISPLAYEX_RANGE{ 0x83E8, 1000 };
static const AddressRange BASICA_RANGE{    0xB000, 4096 };
static const AddressRange BASICB_RANGE{    0xC000, 4096 };
static const AddressRange BASICC_RANGE{    0xD000, 4096 };
static const AddressRange EDITOR_RANGE{    0xE000, 2048 };
static const AddressRange PIA1_RANGE{      0xE810, 4 };
static const AddressRange PIA2_RANGE{      0xE820, 4 };
static const AddressRange VIA_RANGE{       0xE840, 16 };
static const AddressRange KERNAL_RANGE{    0xF000, 4096 };

u8 MemoryMap::load8(u16 address)
{
    u16 offset;
    if (ZEROPAGE_RANGE.contains(address, offset))
    {
        std::cerr << "Unhandled load8 from ZEROPAGE\n";
        return 0;
    }
    if (STACK_RANGE.contains(address, offset))
        return stack[offset];
    if (INPUT_RANGE.contains(address, offset))
    {
        std::cerr << "Unhandled load8 from System INPUT Buffer\n";
        return 0;
    }
    if (BASICA_RANGE.contains(address, offset))
        return basica[offset];
    if (BASICB_RANGE.contains(address, offset))
        return basicb[offset];
    if (BASICC_RANGE.contains(address, offset))
        return basicc[offset];
    if (EDITOR_RANGE.contains(address, offset))
        return editor4[offset];
    if (PIA1_RANGE.contains(address, offset))
        return pia1.load8(offset);
    if (PIA2_RANGE.contains(address, offset))
        return pia2.load8(offset);
    if (KERNAL_RANGE.contains(address, offset))
        return kernal4[offset];

    std::cerr << "Unhandled load8 at: " << std::hex << std::setw(4) << std::setfill('0') << address << '\n';
    return 0;
}

u16 MemoryMap::load16(u16 address)
{
    u16 offset;
    if (TAPEBUFFERPTR_RANGE.contains(address, offset))
    {
        std::cerr << "Unhandled load16 from Pointer: Start of Tape Buffer\n";
        return 0;
    }
    if (IOSTARTPTR_RANGE.contains(address, offset))
    {
        std::cerr << "Unhandled load16 from Pointer: I/O Start\n";
        return 0;
    }
    if (STACK_RANGE.contains(address, offset))
        return *reinterpret_cast<const u16*>(stack + offset);
    if (BASICA_RANGE.contains(address, offset))
        return *reinterpret_cast<const u16*>(basica + offset);
    if (BASICB_RANGE.contains(address, offset))
        return *reinterpret_cast<const u16*>(basicb + offset);
    if (BASICC_RANGE.contains(address, offset))
        return *reinterpret_cast<const u16*>(basicc + offset);
    if (EDITOR_RANGE.contains(address, offset))
        return *reinterpret_cast<const u16*>(editor4 + offset);
    if (KERNAL_RANGE.contains(address, offset))
        return *reinterpret_cast<const u16*>(kernal4 + offset);

    std::cerr << "Unhandled load16 at: " << std::hex << std::setw(4) << std::setfill('0') << address << '\n';
    return 0;
}

void MemoryMap::store8(u16 address, u8 data)
{
    u16 offset;
    if (ZEROPAGE_RANGE.contains(address, offset))
    {
        std::cerr << "Unhandled store8 to ZEROPAGE\n";
        return;
    }
    if (STACK_RANGE.contains(address, offset))
    {
        stack[offset] = data;
        return;
    }
    if (INPUT_RANGE.contains(address, offset))
    {
        std::cerr << "Unhandled store8 to System INPUT Buffer\n";
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
              << " with: " << std::hex << std::setw(2) << std::setfill('0') << (u16)data << '\n';
}

void MemoryMap::store16(u16 address, u16 data)
{
    u16 offset;
    if (STACK_RANGE.contains(address, offset))
    {
        stack[offset] = data & 0xFF;
        stack[offset + 1] = data >> 8;
        return;
    }

    std::cerr << "Unhandled store16 at: " << std::hex << std::setw(4) << std::setfill('0') << address
              << " with: " << std::hex << std::setw(2) << std::setfill('0') << (u16)data << '\n';
}
