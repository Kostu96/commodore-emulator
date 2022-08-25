#include "cpu6502.hpp"

#include "memory_map.hpp"

#include <iostream>
#include <iomanip>

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
    case 0x00:           op_BRK(); break;
    case 0x09: am_IMM(); op_ORA(); break;
    case 0x0A: op_ASL_ACC(); break;
    case 0x10:           op_BPL(); break;
    case 0x18:           op_CLC(); break;
    case 0x20:           op_JSR(); break;
    case 0x24: am_ZPG(); op_BIT(); break;
    case 0x29: am_IMM(); op_AND(); break;
    case 0x2C: am_ABS(); op_BIT(); break;
    case 0x30:           op_BMI(); break;
    case 0x38:           op_SEC(); break;
    case 0x46: am_ZPG(); op_LSR(); break;
    case 0x48:           op_PHA(); break;
    case 0x4A: am_ACC(); op_LSR(); break;
    case 0x4C: op_JMP_ABS(); break;
    case 0x58:           op_CLI(); break;
    case 0x60:           op_RTS(); break;
    case 0x65: am_ZPG(); op_ADC(); break;
    case 0x68:           op_PLA(); break;
    case 0x69: am_IMM(); op_ADC(); break;
    case 0x78:           op_SEI(); break;
    case 0x84: am_ZPG(); op_STY(); break;
    case 0x85: am_ZPG(); op_STA(); break;
    case 0x86: am_ZPG(); op_STX(); break;
    case 0x88:           op_DEY(); break;
    case 0x8A:           op_TXA(); break;
    case 0x8D: am_ABS(); op_STA(); break;
    case 0x8E: am_ABS(); op_STX(); break;
    case 0x90:           op_BCC(); break;
    case 0x91: am_INY(); op_STA(); break;
    case 0x94: am_ZPX(); op_STY(); break;
    case 0x95: am_ZPX(); op_STA(); break;
    case 0x98:           op_TYA(); break;
    case 0x99: am_ABY(); op_STA(); break;
    case 0x9A:           op_TXS(); break;
    case 0x9D: am_ABX(); op_STA(); break;
    case 0xA0: am_IMM(); op_LDY(); break;
    case 0xA2: am_IMM(); op_LDX(); break;
    case 0xA4: am_ZPG(); op_LDY(); break;
    case 0xA5: am_ZPG(); op_LDA(); break;
    case 0xA6: am_ZPG(); op_LDX(); break;
    case 0xA8:           op_TAY(); break;
    case 0xA9: am_IMM(); op_LDA(); break;
    case 0xAA:           op_TAX(); break;
    case 0xAD: am_ABS(); op_LDA(); break;
    case 0xB0:           op_BCS(); break;
    case 0xB1: am_INY(); op_LDA(); break;
    case 0xB4: am_ZPX(); op_LDY(); break;
    case 0xB5: am_ZPX(); op_LDA(); break;
    case 0xBA:           op_TSX(); break;
    case 0xBD: am_ABX(); op_LDA(); break;
    case 0xC0: am_IMM(); op_CPY(); break;
    case 0xC4: am_ZPG(); op_CPY(); break;
    case 0xC5: am_ZPG(); op_CMP(); break;
    case 0xC6: am_ZPG(); op_DEC(); break;
    case 0xC8:           op_INY(); break;
    case 0xCA:           op_DEX(); break;
    case 0xC9: am_IMM(); op_CMP(); break;
    case 0xD0:           op_BNE(); break;
    case 0xD1: am_INY(); op_CMP(); break;
    case 0xD8:           op_CLD(); break;
    case 0xDD: am_ABX(); op_CMP(); break;
    case 0xE0: am_IMM(); op_CPX(); break;
    case 0xE5: am_ZPG(); op_SBC(); break;
    case 0xE6: am_ZPG(); op_INC(); break;
    case 0xE8:           op_INX(); break;
    case 0xE9: am_IMM(); op_SBC(); break;
    case 0xF0:           op_BEQ(); break;
    case 0xF6: am_ZPX(); op_INC(); break;
    default:
        std::cerr << "Unhandled instruction: " << std::hex << std::setw(2) << std::setfill('0') << (u16)instruction << '\n';
    }
}

void CPU6502::push8(u8 data)
{
    m_memoryMap.store8(0x100 + SP, data);
    SP--;
}

void CPU6502::push16(u16 data)
{
    m_memoryMap.store16(0x100 + SP - 1, data);
    SP -= 2;
}

u8 CPU6502::pop8()
{
    SP++;
    return m_memoryMap.load8(0x100 + SP);
}

u16 CPU6502::pop16()
{
    SP += 2;
    return m_memoryMap.load16(0x100 + SP - 1);
}

#pragma region AdressingModes
void CPU6502::am_ACC()
{
    // takes 0 cycles

    m_isACCAddressing = true;
}

void CPU6502::am_IMM()
{
    // takes 0 cycles

    m_absoluteAddress = PC++;
}

void CPU6502::am_ZPG()
{
    // takes 1 cycle

    m_absoluteAddress = m_memoryMap.load8(PC++);
}

void CPU6502::am_ZPX()
{
    // takes 2 cycles

    m_absoluteAddress = m_memoryMap.load8(PC++) + X;
    m_absoluteAddress &= 0x00FF;
}

void CPU6502::am_ZPY()
{
    // takes 2 cycles

    m_absoluteAddress = m_memoryMap.load8(PC++) + Y;
    m_absoluteAddress &= 0x00FF;
}

void CPU6502::am_ABS()
{
    // takes 2 cycles

    m_absoluteAddress = m_memoryMap.load16(PC);
    PC += 2;
}

void CPU6502::am_ABX()
{
    // takes 3 cycles

    m_absoluteAddress = m_memoryMap.load16(PC) + X;
    PC += 2;
}

void CPU6502::am_ABY()
{
    // takes 3 cycles

    m_absoluteAddress = m_memoryMap.load16(PC) + Y;
    PC += 2;
}

void CPU6502::am_IND()
{
    u16 ptr = m_memoryMap.load16(PC);
    PC += 2;

    m_absoluteAddress = m_memoryMap.load8(((ptr & 0xFF) == 0xFF) ? ptr & 0xFF00 : ptr + 1) << 8;
    m_absoluteAddress |= m_memoryMap.load8(ptr);

}

void CPU6502::am_INX()
{
    u8 ptr = m_memoryMap.load8(PC++) + X;

    m_absoluteAddress = m_memoryMap.load8(ptr + 1) << 8;
    m_absoluteAddress |= m_memoryMap.load8(ptr);
}

void CPU6502::am_INY()
{
    u8 ptr = m_memoryMap.load8(PC++);

    m_absoluteAddress = m_memoryMap.load16(ptr) + Y;
}
#pragma endregion

#pragma region BranchInstructions
void CPU6502::op_JMP_ABS()
{
    // takes 3 cycles

    PC = m_memoryMap.load16(PC);
}

void CPU6502::op_BPL()
{
    // takes 2** cycles

    s8 offset = m_memoryMap.load8(PC++);
    if (F.bits.N == 0)
        PC += offset;
}

void CPU6502::op_BMI()
{
    // takes 2** cycles

    s8 offset = m_memoryMap.load8(PC++);
    if (F.bits.N == 1)
        PC += offset;
}

void CPU6502::op_BEQ()
{
    // takes 2** cycles

    s8 offset = m_memoryMap.load8(PC++);
    if (F.bits.Z == 1)
        PC += offset;
}

void CPU6502::op_BNE()
{
    // takes 2** cycles

    s8 offset = m_memoryMap.load8(PC++);
    if (F.bits.Z == 0)
        PC += offset;
}

void CPU6502::op_BCS()
{
    // takes 2** cycles

    s8 offset = m_memoryMap.load8(PC++);
    if (F.bits.C == 1)
        PC += offset;
}

void CPU6502::op_BCC()
{
    // takes 2** cycles

    s8 offset = m_memoryMap.load8(PC++);
    if (F.bits.C == 0)
        PC += offset;
}

void CPU6502::op_JSR()
{
    // takes 6 cycles

    u16 addr = m_memoryMap.load16(PC);
    PC++; // Return address is incremented on RTS
    push16(PC);
    PC = addr;
}

void CPU6502::op_RTS()
{
    // takes 6 cycles

    PC = pop16() + 1;
}
#pragma endregion

#pragma region LoadInstructions
void CPU6502::op_LDA()
{
    // takes 2 cycles

    ACC = m_memoryMap.load8(m_absoluteAddress);

    F.bits.Z = (ACC == 0);
    F.bits.N = (ACC >> 7);
}

void CPU6502::op_LDX()
{
    // takes 2 cycles

    X = m_memoryMap.load8(m_absoluteAddress);

    F.bits.Z = (X == 0);
    F.bits.N = (X >> 7);
}

void CPU6502::op_LDY()
{
    // takes 2 cycles

    Y = m_memoryMap.load8(m_absoluteAddress);

    F.bits.Z = (Y == 0);
    F.bits.N = (Y >> 7);
}
#pragma endregion

#pragma region StoreInstructions
void CPU6502::op_STA()
{
    // takes 2 cycles

    m_memoryMap.store8(m_absoluteAddress, ACC);
}

void CPU6502::op_STX()
{
    // takes 2 cycles

    m_memoryMap.store8(m_absoluteAddress, X);
}

void CPU6502::op_STY()
{
    // takes 2 cycles

    m_memoryMap.store8(m_absoluteAddress, Y);
}
#pragma endregion

#pragma region TransferInstructions
void CPU6502::op_TXA()
{
    // takes 2 cycles

    ACC = X;

    F.bits.Z = (ACC == 0);
    F.bits.N = (ACC >> 7);
}

void CPU6502::op_TAX()
{
    // takes 2 cycles

    X = ACC;

    F.bits.Z = (X == 0);
    F.bits.N = (X >> 7);
}

void CPU6502::op_TYA()
{
    // takes 2 cycles

    ACC = Y;

    F.bits.Z = (ACC == 0);
    F.bits.N = (ACC >> 7);
}

void CPU6502::op_TAY()
{
    // takes 2 cycles

    Y = ACC;

    F.bits.Z = (Y == 0);
    F.bits.N = (Y >> 7);
}

void CPU6502::op_TSX()
{
    // takes 2 cycles

    X = SP;
}

void CPU6502::op_TXS()
{
    // takes 2 cycles

    SP = X;
}
#pragma endregion

#pragma region StackInstructions
void CPU6502::op_PHA()
{
    // takes 3 cycles

    push8(ACC);
}

void CPU6502::op_PLA()
{
    // takes 4 cycles

    ACC = pop8();

    F.bits.Z = (ACC == 0);
    F.bits.N = (ACC >> 7);
}
#pragma endregion

#pragma region ALUInstructions
void CPU6502::op_ADC()
{
    u16 memory = m_memoryMap.load8(m_absoluteAddress);
    u16 temp = ACC + memory;
    ACC = temp & 0xFF;

    F.bits.C = (temp >> 8);
    F.bits.Z = (ACC == 0);
    F.bits.N = (ACC >> 7);
    F.bits.V = (~((u16)ACC ^ memory) & ((u16)ACC ^ temp)) >> 7;
}

void CPU6502::op_SBC()
{
    u16 memory = m_memoryMap.load8(m_absoluteAddress);
    memory ^= 0x00FF;
    u16 temp = ACC + memory + F.bits.C;
    ACC = temp & 0xFF;

    F.bits.C = (temp >> 8);
    F.bits.Z = (ACC == 0);
    F.bits.N = (ACC >> 7);
    F.bits.V = (~((u16)ACC ^ memory) & ((u16)ACC ^ temp)) >> 7;
}

void CPU6502::op_ORA()
{
    // takes 2 cycles

    ACC |= m_memoryMap.load8(m_absoluteAddress);

    F.bits.Z = (ACC == 0);
    F.bits.N = (ACC >> 7);
}

void CPU6502::op_AND()
{
    // takes 2 cycles

    ACC &= m_memoryMap.load8(m_absoluteAddress);

    F.bits.Z = (ACC == 0);
    F.bits.N = (ACC >> 7);
}

void CPU6502::op_LSR()
{
    // takes 2 cycles

    u8 data;
    if (m_isACCAddressing)
    {
        ACC >>= 1;
        ACC &= 0x7F;
        
        m_isACCAddressing = false;
        data = ACC;
    }
    else
    {
        data = m_memoryMap.load8(m_absoluteAddress);
        data >>= 1;
        data &= 0x7F;
        m_memoryMap.store8(m_absoluteAddress, data);
    }

    F.bits.C = (data & 1);
    F.bits.Z = (data == 0);
    F.bits.N = 0;
}

void CPU6502::op_ASL_ACC()
{
    // takes 2 cycles

    F.bits.C = (ACC >> 7);
    ACC <<= 1;

    F.bits.Z = (ACC == 0);
    F.bits.N = (ACC >> 7);
}

void CPU6502::op_BIT()
{
    // takes 2 cycles

    u8 temp = m_memoryMap.load8(m_absoluteAddress);
    F.bits.Z = ((ACC & temp) == 0);
    F.bits.N = (temp >> 7);
    F.bits.V = (temp >> 6) & 1;
}

void CPU6502::op_INC()
{
    // takes 4 cycles

    u8 data = m_memoryMap.load8(m_absoluteAddress);
    data++;
    m_memoryMap.store8(m_absoluteAddress, data);

    F.bits.Z = (data == 0);
    F.bits.N = (data >> 7);
}

void CPU6502::op_INX()
{
    // takes 2 cycles

    X++;

    F.bits.Z = (X == 0);
    F.bits.N = (X >> 7);
}

void CPU6502::op_INY()
{
    // takes 2 cycles

    Y++;

    F.bits.Z = (Y == 0);
    F.bits.N = (Y >> 7);
}

void CPU6502::op_DEC()
{
    // takes 4 cycles

    u8 data = m_memoryMap.load8(m_absoluteAddress);
    data--;
    m_memoryMap.store8(m_absoluteAddress, data);

    F.bits.Z = (data == 0);
    F.bits.N = (data >> 7);
}

void CPU6502::op_DEX()
{
    // takes 2 cycles

    X--;

    F.bits.Z = (X == 0);
    F.bits.N = (X >> 7);
}

void CPU6502::op_DEY()
{
    // takes 2 cycles

    Y--;

    F.bits.Z = (Y == 0);
    F.bits.N = (Y >> 7);
}

void CPU6502::op_CMP()
{
    // takes 2 cycles

    u16 memory = m_memoryMap.load8(m_absoluteAddress);
    u16 temp = static_cast<u16>(ACC) - memory;
    F.bits.C = ACC >= memory ? 1 : 0;
    F.bits.Z = ((temp & 0xFF) == 0);
    F.bits.N = ((temp & 0xFF) >> 7);
}

void CPU6502::op_CPX()
{
    // takes 2 cycles

    u16 memory = m_memoryMap.load8(m_absoluteAddress);
    u16 temp = static_cast<u16>(X) - memory;
    F.bits.C = X >= memory ? 1 : 0;
    F.bits.Z = ((temp & 0xFF) == 0);
    F.bits.N = ((temp & 0xFF) >> 7);
}

void CPU6502::op_CPY()
{
    // takes 2 cycles

    u16 memory = m_memoryMap.load8(m_absoluteAddress);
    u16 temp = static_cast<u16>(Y) - memory;
    F.bits.C = Y >= memory ? 1 : 0;
    F.bits.Z = ((temp & 0xFF) == 0);
    F.bits.N = ((temp & 0xFF) >> 7);
}
#pragma endregion

#pragma region FlagsInstructions
void CPU6502::op_SEC()
{
    // takes 2 cycles

    F.bits.C = 1;
}

void CPU6502::op_CLC()
{
    // takes 2 cycles

    F.bits.C = 0;
}

void CPU6502::op_SEI()
{
    // takes 2 cycles

    F.bits.I = 1;
}

void CPU6502::op_CLI()
{
    // takes 2 cycles

    F.bits.I = 0;
}

void CPU6502::op_CLD()
{
    // takes 2 cycles

    F.bits.D = 0;
}
#pragma endregion

#pragma region Interrupts
void CPU6502::op_BRK()
{
    // takes 7 cycles

    push16(PC + 2);
    F.bits.I = 1; // ?
    push8(F.byte);
    PC = m_memoryMap.load16(0xFFFE); // IRQ vector
}
#pragma endregion
