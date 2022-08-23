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
    void op_BRK();
    void op_JSR();
    void op_SEI();
    void op_STAAbs();
    void op_TSX();
    void op_LDXImm();
    void op_LDAImm();
    void op_CLD();

    MemoryMap& m_memoryMap;

    u16 PC;
    u8 ACC;
    u8 Flags;
    u8 X;
    u8 Y;
    u8 SP;
};
