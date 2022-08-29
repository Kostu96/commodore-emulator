#pragma once
#include "type_aliases.hpp"

class CPU6502;

class IO
{
public:
    IO() = default;

    void setCPU(CPU6502& cpu);

    u8 load8(u16 offset);
    void store8(u16 offset, u8 data);

    void clock();
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

    u8 PIA1PortA = 0xFF;
    u8 PIA1DataDirectionA;
    Control PIA1ControlA{};
    u8 PIA1PortB = 0xFF;
    u8 PIA1DataDirectionB;
    Control PIA1ControlB{};

    u8 PIA2PortA = 0xFF;
    u8 PIA2DataDirectionA;
    Control PIA2ControlA{};
    u8 PIA2PortB = 0xFF;
    u8 PIA2DataDirectionB;
    Control PIA2ControlB{};

    u8 VIAPortB;
    u8 VIADataDirectionB;
    u8 VIATimer1H;
    u8 VIAPeripheralControl;
    u8 VIAInterruptEnable = 0x80;

    u32 m_cycles = 0;

    CPU6502* m_cpu;
};
