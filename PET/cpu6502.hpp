#pragma once
#include "type_aliases.hpp"

class MemoryMap;

class CPU6502
{
public:
    explicit CPU6502(MemoryMap& memoryMap) : m_memoryMap{ memoryMap } { reset(); }

    void reset();
    void clock();
private:
    MemoryMap& m_memoryMap;

    u16 PC;
};
