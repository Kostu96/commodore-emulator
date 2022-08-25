#pragma once
#include "type_aliases.hpp"

class PIA6520
{
public:
	PIA6520() = default;

	u8 load8(u16 offset);
	void store8(u16 offset, u8 data);
private:
    union Control
    {
        struct Bits
        {
            u8 Cx1ActiveTransition : 1;
            u8 Cx2ActiveTransition : 1;
            u8 Cx2Direction : 1;
            u8 Cx2Control : 2;
            u8 PortControl : 1;
            u8 Cx1Control : 2;
        };

        Bits bits;
        u8 byte = 0;
    };

	u8 PortA;
	u8 DataDirectionA;
    Control ControlA{};
	u8 PortB = 0xFF;
	u8 DataDirectionB;
    Control ControlB{};
};
