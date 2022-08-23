#include "cpu6502.hpp"

#include "memory_map.hpp"

#include <iostream>

void CPU6502::reset()
{
    // takes 7 cycles as all interupts

    PC = m_memoryMap.load16(0xFFFC); // RESET vector
    SP = 0xFD;
    F.byte = 0x0004;
}

void CPU6502::clock()
{
    u8 instruction = m_memoryMap.load8(PC++);
    
    switch (instruction)
    {
    case 0x00: op_BRK(); break;
    case 0x0A: op_ASL_ACC(); break;
    case 0x10: op_BPL(); break;
    case 0x20: op_JSR(); break;
    case 0x2C: op_BIT_ABS(); break;
    case 0x78: op_SEI(); break;
    case 0x85: op_STA_ZPG(); break;
    case 0x8D: op_STA_ABS(); break;
    case 0x8E: op_STX_ABS(); break;
    case 0x94: op_STY_ZPX(); break;
    case 0x95: op_STA_ZPX(); break;
    case 0x9A: op_TSX(); break;
    case 0xA0: op_LDY_IMM(); break;
    case 0xA2: op_LDX_IMM(); break;
    case 0xA9: op_LDA_IMM(); break;
    case 0xCA: op_DEX(); break;
    case 0xD8: op_CLD(); break;
    case 0xE0: op_CPX_IMM(); break;
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

void CPU6502::op_ASL_ACC()
{
    // takes 2 cycles

    F.bits.C = (ACC >> 7);
    ACC <<= 1;

    F.bits.Z = (ACC == 0);
    F.bits.N = (ACC >> 7);
}

void CPU6502::op_BPL()
{
    // takes 2** cycles

    s8 offset = m_memoryMap.load8(PC++);
    if (F.bits.N == 0)
        PC += offset;
}

void CPU6502::op_JSR()
{
    // takes 6 cycles

    u16 addr = m_memoryMap.load16(PC);
    PC += 2;
    // TODO: push PC
    PC = addr;
}

void CPU6502::op_BIT_ABS()
{
    // takes 4 cycles
    
    u16 addr = m_memoryMap.load16(PC);
    PC += 2;

    u8 temp = m_memoryMap.load8(addr);
    F.bits.Z = ((ACC & temp) == 0);
    F.bits.N = (temp >> 7);
    F.bits.V = (temp >> 6) & 1;
}

void CPU6502::op_SEI()
{
    // takes 2 cycles
    
    F.bits.I = 1;
}

void CPU6502::op_STA_ZPG()
{
    // takes 3 cycles

    u16 addr = m_memoryMap.load8(PC++);
    m_memoryMap.store8(addr, ACC);
}

void CPU6502::op_STA_ABS()
{
    // takes 4 cycles

    u16 addr = m_memoryMap.load16(PC);
    PC += 2;
    m_memoryMap.store8(addr, ACC);
}

void CPU6502::op_STX_ABS()
{
    // takes 4 cycles

    u16 addr = m_memoryMap.load16(PC);
    PC += 2;
    m_memoryMap.store8(addr, X);
}

void CPU6502::op_STY_ZPX()
{
    // takes 4 cycles

    u16 addr = m_memoryMap.load8(PC++) + X;
    addr &= 0x00FF;
    m_memoryMap.store8(addr, Y);
}

void CPU6502::op_STA_ZPX()
{
    // takes 4 cycles

    u16 addr = m_memoryMap.load8(PC++) + X;
    addr &= 0x00FF;
    m_memoryMap.store8(addr, ACC);
}

void CPU6502::op_TSX()
{
    // takes 2 cycles

    SP = X;
}

void CPU6502::op_LDY_IMM()
{
    // takes 2 cycles

    u8 imm = m_memoryMap.load8(PC++);
    Y = imm;

    F.bits.Z = (Y == 0);
    F.bits.N = (Y >> 7);
}

void CPU6502::op_LDX_IMM()
{
    // takes 2 cycles

    u8 imm = m_memoryMap.load8(PC++);
    X = imm;

    F.bits.Z = (X == 0);
    F.bits.N = (X >> 7);
}

void CPU6502::op_LDA_IMM()
{
    // takes 2 cycles

    u8 imm = m_memoryMap.load8(PC++);
    ACC = imm;

    F.bits.Z = (ACC == 0);
    F.bits.N = (ACC >> 7);
}

void CPU6502::op_DEX()
{
    // takes 2 cycles

    X--;

    F.bits.Z = (X == 0);
    F.bits.N = (X >> 7);
}

void CPU6502::op_CLD()
{
    // takes 2 cycles

    F.bits.D = 0;
}

void CPU6502::op_CPX_IMM()
{
    // takes 2 cycles

    u16 imm = m_memoryMap.load8(PC++);
    u16 temp = static_cast<u16>(X) - imm;
    F.bits.C = X >= imm ? 1 : 0;
    F.bits.Z = ((temp & 0xFF) == 0);
    F.bits.N = ((temp & 0xFF) >> 7);
}
