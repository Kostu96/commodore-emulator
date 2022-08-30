#pragma once
#include "type_aliases.hpp"

class MemoryMap;

class CPU6502
{
public:
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

    explicit CPU6502(MemoryMap& memoryMap);

    Flags getFlags() const { return F; }

    void reset();
    void clock();
    void IRQ();
    void NMI();
private:
    void push8(u8 data);
    void push16(u16 data);
    u8 pop8();
    u16 pop16();

    // Addressing Modes:
    void am_ACC(); void am_IMM();
    void am_ZPG(); void am_ZPX(); void am_ZPY();
    void am_ABS(); void am_ABX(); void am_ABY();
    void am_IND(); void am_INX(); void am_INY();

    void op_NOP();

    // Branch Instructions:
    void op_JMP();
    void op_BPL(); void op_BMI();
    void op_BEQ(); void op_BNE();
    void op_BCS(); void op_BCC();
    void op_JSR(); void op_RTS();
    
    // Load Instructions:
    void op_LDA(); void op_LDX(); void op_LDY();

    // Store Instructions:
    void op_STA(); void op_STX(); void op_STY();

    // Transfer Instructions:
    void op_TXA(); void op_TAX();
    void op_TYA(); void op_TAY();
    void op_TSX(); void op_TXS();

    // Stack Instructions:
    void op_PHA(); void op_PLA();
    void op_PHP(); void op_PLP();

    // ALU Instructions:
    void op_ADC(); void op_SBC();
    void op_ORA();
    void op_AND();
    void op_EOR();
    void op_LSR();
    void op_ASL();
    void op_ROL(); void op_ROR();
    void op_BIT();
    void op_INC(); void op_INX(); void op_INY();
    void op_DEC(); void op_DEX(); void op_DEY();
    void op_CMP(); void op_CPX(); void op_CPY();

    // Flags Instructions:
    void op_SEC(); void op_CLC();
    void op_SEI(); void op_CLI();
    void op_CLD();

    // Interrupts:
    void op_BRK();
    void op_RTI();

    MemoryMap& m_memoryMap;

    u16 PC;
    u8 ACC;
    Flags F;
    u8 X;
    u8 Y;
    u8 SP;

    // helper variables
    u16 m_absoluteAddress;
    bool m_isACCAddressing = false;
};
