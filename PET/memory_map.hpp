#pragma once
#include "type_aliases.hpp"

class MemoryMap
{
public:
    MemoryMap() = default;

    u8 load8(u16 address);
    u16 load16(u16 address);

    void store8(u16 address, u8 data);
    void store16(u16 address, u16 data);
private:
    u8 stack[256];
    u8 display[1000];
#include "basica.inl"
#include "basicb.inl"
#include "basicc.inl"
#include "editor4.inl"
#include "kernal4.inl"
};
