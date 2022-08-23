#include "cpu6502.hpp"

#include "memory_map.hpp"

#include <iostream>

void CPU6502::reset()
{
    // takes 7 cycles as all interupts

    PC = m_memoryMap.load16(0xFFFC); // RESET vector
    SP = 0xFD;
}

void CPU6502::clock()
{
    u8 instruction = m_memoryMap.load8(PC++);
    
    switch (instruction)
    {
    case 0x00: op_BRK(); break;
    case 0x20: op_JSR(); break;
    case 0x78: op_SEI(); break;
    case 0x8D: op_STAAbs(); break;
    case 0x9A: op_TSX(); break;
    case 0xA2: op_LDXImm(); break;
    case 0xA9: op_LDAImm(); break;
    case 0xD8: op_CLD(); break;
    default:
        std::cerr << "Unhandled instruction: " << std::hex << (u16)instruction << '\n';
    }
}

void CPU6502::op_BRK()
{
    // takes 7 cycles

    PC += 2;
    m_memoryMap.store16(0x100 + SP, PC);
    SP -= 2;
    // TODO: push Flags
    PC = m_memoryMap.load16(0xFFFE); // IRQ vector
}

void CPU6502::op_JSR()
{
    // takes 6 cycles

    u16 addr = m_memoryMap.load16(PC);
    PC += 2;
    // TODO: push PC
    PC = addr;
}

void CPU6502::op_SEI()
{
    // takes 2 cycles
    
    Flags |= 1 << 2; // TODO: flag bits
}

void CPU6502::op_STAAbs()
{
    // takes 4 cycles

    u16 addr = m_memoryMap.load16(PC);
    PC += 2;
    m_memoryMap.store8(addr, ACC);
}

void CPU6502::op_TSX()
{
    // takes 2 cycles

    SP = X;
}

void CPU6502::op_LDXImm()
{
    // takes 2 cycles
    // TODO: Flags

    u8 imm = m_memoryMap.load8(PC++);
    X = imm;
}

void CPU6502::op_LDAImm()
{
    // takes 2 cycles
    // TODO: Flags

    u8 imm = m_memoryMap.load8(PC++);
    ACC = imm;
}

void CPU6502::op_CLD()
{
    // takes 2 cycles

    Flags &= ~(1 << 3); // TODO: flag bits
}
