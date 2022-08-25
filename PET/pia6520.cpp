#include "pia6520.hpp"

u8 PIA6520::load8(u16 offset)
{
    switch (offset)
    {
    case 0: return PA;
    case 2: return PB;
    }

    return 0xCD;
}

void PIA6520::store8(u16 offset, u8 data)
{
    switch (offset)
    {
    case 0:
        PA = data;
        break;
    }
}
