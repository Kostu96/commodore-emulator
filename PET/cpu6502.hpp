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
    void op_ASL_ACC();
    void op_BPL();
    void op_JSR();
    void op_BIT_ABS();
    void op_SEI();
    void op_STA_ZPG();
    void op_STA_ABS();
    void op_STX_ABS();
    void op_STY_ZPX();
    void op_STA_ZPX();
    void op_TSX();
    void op_LDY_IMM();
    void op_LDX_IMM();
    void op_LDA_IMM();
    void op_DEX();
    void op_CLD();
    void op_CPX_IMM();

    MemoryMap& m_memoryMap;

    union Flags
    {
        struct Bits
        {
            u8 N : 1;
            u8 V : 1;
            u8 unused : 1;
            u8 B : 1;
            u8 D : 1;
            u8 I : 1;
            u8 Z : 1;
            u8 C : 1;
        };

        Bits bits;
        u8 byte;
    };

    u16 PC;
    u8 ACC;
    Flags F;
    u8 X;
    u8 Y;
    u8 SP;
};
