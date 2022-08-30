#pragma once
#include "type_aliases.hpp"
#include "io.hpp"

#define SERIES1

class MemoryMap
{
public:
    MemoryMap() = default;

    void init(CPU6502& cpu, UpdateScreenFunc func) { io.init(cpu, func, vram); }

    u8 load8(u16 address);
    u16 load16(u16 address);
    void store8(u16 address, u8 data);
    void store16(u16 address, u16 data);

    void clock();
private:
    u8 ram[0x2000]{};
    u8 vram[0x400]{};
    IO io;

#if defined(SERIES1)
#include "roms/basic1_C000.inl"
#include "roms/editor1_E000.inl"
#include "roms/kernal1_F000.inl"
#endif
#if defined(SERIES2)
#include "roms/basic2_C000.inl"
#include "roms/editor2_E000.inl"
#include "roms/kernal2_F000.inl"
#endif
#if defined(SERIES4)
#include "roms/basic4_B000.inl"
#include "roms/editor4_E000.inl"
#include "roms/kernal4_F000.inl"
#endif
};
