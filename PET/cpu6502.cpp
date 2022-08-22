#include "cpu6502.hpp"

#include "memory_map.hpp"

void CPU6502::reset()
{
    // takes 8 cycles as all interupts
    // SP to 0xFD
    PC = 0xFFFC;
    PC = m_memoryMap.load16(PC);
}

void CPU6502::clock()
{
    u8 instruction = m_memoryMap.load8(PC++);
    (void)instruction;
}
