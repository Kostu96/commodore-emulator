#include "pia6520.hpp"

u8 PIA6520::load8(u16 offset)
{
    switch (offset)
    {
    case 0: return PortA;
    case 2: return PortB;
    }

    return 0xCD;
}

void PIA6520::store8(u16 offset, u8 data)
{
    switch (offset)
    {
    case 0: {
        if (ControlA.bits.PortControl)
            PortA = data;
        else
            DataDirectionA = data;
    } break;
    case 1:
        ControlA.byte = data;
        break;
    case 2: {
        if (ControlB.bits.PortControl)
            PortB = data;
        else
            DataDirectionB = data;
    } break;
    case 3:
        ControlB.byte = data;
        break;
    }
}
