#pragma once
#include "type_aliases.hpp"

typedef void(*UpdateScreenFunc)(u16, u8);

class MemoryMap
{
public:
    MemoryMap(UpdateScreenFunc func) : updateScreen{ func } {}

    u8 load8(u16 address);
    u16 load16(u16 address);

    void store8(u16 address, u8 data);
    void store16(u16 address, u16 data);
private:
    u8 stack[256]{};
    u8 display[1000]{};
#include "roms/basica.inl"
#include "roms/basicb.inl"
#include "roms/basicc.inl"
#include "roms/editor4.inl"
#include "roms/kernal4.inl"

    UpdateScreenFunc updateScreen;
};
