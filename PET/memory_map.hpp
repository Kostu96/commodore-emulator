#pragma once
#include "type_aliases.hpp"
#include "io.hpp"

#define SERIES1
#define TEST 0

#if TEST
#include <fstream>
#endif

class MemoryMap
{
public:
#if TEST
    MemoryMap()
    {
        std::ifstream fin{ "assets/6502_functional_test.bin", std::ios::binary };

        if (fin.is_open())
        {
            fin.read((char*)memory, 0x10000);
            fin.close();
        }
    }
#else
    MemoryMap() = default;
#endif

#if TEST
    u8 load8(u16 address) { return memory[address]; }
    u16 load16(u16 address) { return *reinterpret_cast<const u16*>(memory + address); }
    void store8(u16 address, u8 data) { memory[address] = data; }
    void store16(u16 address, u16 data) { *reinterpret_cast<u16*>(memory + address) = data; }
#else
    u8 load8(u16 address);
    u16 load16(u16 address);
    void store8(u16 address, u8 data);
    void store16(u16 address, u16 data);
#endif
};
