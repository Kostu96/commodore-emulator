#include "mapper_000.hpp"

u16 Mapper000::map(u16 address) const
{
    address -= 0x4020;
    return address;
}
