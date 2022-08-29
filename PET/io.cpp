#include "io.hpp"

#include "cpu6502.hpp"

#include <iostream>
#include <iomanip>

void IO::setCPU(CPU6502& cpu)
{
    m_cpu = &cpu;
}

u8 IO::load8(u16 offset)
{
    switch (offset)
    {
    case 0x10: return PIA1PortA;
    case 0x12: return PIA1PortB;
    case 0x20: return PIA2PortA;
    case 0x22: return PIA2PortB;
    case 0x40: return VIAPortB;
    }

    std::cerr << "Unhandled load8 at IO: " << std::hex << std::setw(2) << std::setfill('0') << offset << '\n';
    return 0xCD;
}

void IO::store8(u16 offset, u8 data)
{
    switch (offset)
    {
    case 0x10: {
        if (PIA1ControlA.bits.PortControl)
            PIA1PortA = data;
        else
            PIA1DataDirectionA = data;
    } return;
    case 0x11:
        PIA1ControlA.byte = data;
        return;
    case 0x12: {
        if (PIA1ControlB.bits.PortControl)
            PIA1PortB = data;
        else
            PIA1DataDirectionB = data;
    } return;
    case 0x13:
        PIA1ControlB.byte = data;
        return;
    case 0x20: {
        if (PIA2ControlA.bits.PortControl)
            PIA2PortA = data;
        else
            PIA2DataDirectionA = data;
    } return;
    case 0x21:
        PIA2ControlA.byte = data;
        return;
    case 0x22: {
        if (PIA2ControlB.bits.PortControl)
            PIA2PortB = data;
        else
            PIA2DataDirectionB = data;
    } return;
    case 0x23:
        PIA2ControlB.byte = data;
        return;
    case 0x40: VIAPortB = data; return;
    case 0x42: VIADataDirectionB = data; return;
    case 0x45: VIATimer1H = data; return;
    case 0x4C: VIAPeripheralControl = data; return;
    case 0x4E: {
        VIAInterruptEnable = data;
        if (VIAInterruptEnable)
            VIAInterruptEnable |= 0x80;
    } return;
    }

    std::cerr << "Unhandled store8 at IO: " << std::hex << std::setw(2) << std::setfill('0') << offset
        << " with: " << std::hex << std::setw(2) << std::setfill('0') << static_cast<u16>(data) << '\n';
}

void IO::clock()
{
    if (m_cycles++ == 50)
    {
        m_cycles = 0;
        VIAPortB |= 0x20;
        m_cpu->IRQ();
    }
    else
        VIAPortB &= ~0x20;
}
