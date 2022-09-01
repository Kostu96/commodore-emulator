#pragma once
#include "type_aliases.hpp"

class PPU2C02
{
public:
    u8 read(u16 offset);
    void write(u16 offset, u8 data);

    PPU2C02() = default;
private:

};
