#include "ppu2C02.hpp"

#include <cassert>

u8 PPU2C02::read(u16 offset)
{
    u8 data = 0;

    switch (offset & 0x0007)
    {
    case 0: break;
    default:
        assert(false);
    }

    return data;
}

void PPU2C02::write(u16 offset, u8 /*data*/)
{
    switch (offset & 0x0007)
    {
    case 0: break;
    default:
        assert(false);
    }
}
