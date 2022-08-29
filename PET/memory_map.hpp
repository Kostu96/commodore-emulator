#pragma once
#include "type_aliases.hpp"
#include "io.hpp"

#define ROM4 0

typedef void(*UpdateScreenFunc)(u16, u8);

class MemoryMap
{
public:
    MemoryMap(UpdateScreenFunc func) : updateScreen{ func } {}

    void setCPU(CPU6502& cpu) { io.setCPU(cpu); }

    u8 load8(u16 address);
    u16 load16(u16 address);
    void store8(u16 address, u8 data);
    void store16(u16 address, u16 data);

    void clock();
private:
    u8 ram[0x1000]{};
    u8 vram[0x400]{};
    IO io;

#if ROM4
#include "roms/basic4.inl"
#include "roms/editor4.inl"
#include "roms/kernal4.inl"
#else
#include "roms/basic2.inl"
#include "roms/editor2.inl"
#include "roms/kernal2.inl"
#endif

    UpdateScreenFunc updateScreen;
};
