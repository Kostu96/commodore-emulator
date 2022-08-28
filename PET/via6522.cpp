#include "via6522.hpp"

u8 VIA6522::load8(u16 offset)
{
	switch (offset)
	{
	case 0: return PortB;
	}

    return 0;
}

void VIA6522::store8(u16 offset, u8 data)
{
	switch (offset)
	{
	case 0x0: PortB = data; break;
	case 0x2: DataDirectionB = data; break;
	case 0x5: Timer1H = data; break;
	case 0xC: PeripheralControl = data; break;
	case 0xE: {
		InterruptEnable = data;
		if (InterruptEnable)
			InterruptEnable |= 0x80;
	} break;
	}
}

void VIA6522::clock()
{
	if (m_cycles++ == 50)
		PortB |= 0x20;
	else
		PortB &= ~0x20;
}
