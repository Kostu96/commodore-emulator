#include "via6522.hpp"

u8 VIA6522::load8(u16 offset)
{
	switch (offset)
	{
	case 0: break;
	}

    return 0;
}

void VIA6522::store8(u16 offset, u8 data)
{
	switch (offset)
	{
	case 0x0: PB = data; break;
	case 0x2: DDRB = data; break;
	case 0xE: IER = data; break;
	}
}
