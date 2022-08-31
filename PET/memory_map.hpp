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

    void init(CPU6502& cpu, UpdateScreenFunc func) { m_io.init(cpu, func, vram); }

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

    void clock();

    IO& getIO() { return m_io; }
private:
#if TEST
    u8 memory[0x10000];
#endif
    u8 ram[0x2000]{};
    u8 vram[0x400]{};
    IO m_io;

#if defined(SERIES1)
#include "roms/basic1_C000.inl"
#include "roms/editor1_E000.inl"
#include "roms/kernal1_F000.inl"
#endif
#if defined(SERIES2)
#include "roms/basic2_C000.inl"
#include "roms/editor2_E000.inl"
#include "roms/kernal2_F000.inl"
#endif
#if defined(SERIES4)
#include "roms/basic4_B000.inl"
#include "roms/editor4_E000.inl"
#include "roms/kernal4_F000.inl"
#endif
};
