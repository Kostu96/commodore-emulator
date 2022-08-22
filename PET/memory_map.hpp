#pragma once
#include "type_aliases.hpp"

class MemoryMap
{
public:
    u8 load8(u16 address);
    u16 load16(u16 address);
private:
#include "kernal2.inl"
};
