#pragma once
#include "type_aliases.hpp"

class VIA6522
{
public:
	VIA6522() = default;

	u8 load8(u16 offset);
	void store8(u16 offset, u8 data);

	void clock();
private:
	u8 PortB;
	u8 DataDirectionB;
	u8 Timer1H;
	u8 PeripheralControl;
	u8 InterruptEnable = 0x80;

	u32 m_cycles = 0;
};
