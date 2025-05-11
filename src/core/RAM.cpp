#include "RAM.h"

#include <cstring>

#include "MemoryMap.hpp"

RAM::RAM() :
    Memory(RAM_SIZE)
{
}

void RAM::loadExecutable(uint32_t baseAddr, const std::vector<uint8_t> &code)
{
    uint32_t mappedBase = MemoryMap::mapAddress(baseAddr);
    if (MemoryMap::RAM_RANGE.contains(mappedBase)) {
        uint32_t physicalAddr = MemoryMap::RAM_RANGE.remap(mappedBase);
        std::memcpy(&m_data[physicalAddr], &code[0], code.size());
    }
}
