#pragma once
#include "type_aliases.hpp"

class PIA6520
{
public:
	PIA6520() = default;

	u8 load8(u16 offset);
	void store8(u16 offset, u8 data);
private:
	u8 PA;
	u8 DDRA;

	u8 PB = 0xFF;
	u8 DDRB;
};
