#pragma once
#include "type_aliases.hpp"

class VIA6522
{
public:
	VIA6522() = default;

	u8 load8(u16 offset);
	void store8(u16 offset, u8 data);
private:
	u8 PB;
	u8 DDRB;
	u8 IER = 0x80;
};
