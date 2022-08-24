#include <iostream>

#include "memory_map.hpp"
#include "cpu6502.hpp"

int main()
{
    MemoryMap memoryMap{};
    CPU6502 cpu{ memoryMap };

    while (true)
        cpu.clock();

    return 0;
}
