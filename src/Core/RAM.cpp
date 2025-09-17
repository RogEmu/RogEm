#include "RAM.h"

#include <cstring>

#include "MemoryMap.hpp"
#include "Bus.h"

RAM::RAM(Bus *bus) :
    Memory(bus, MemoryMap::RAM_RANGE.length)
{
    m_memoryRange.start = MemoryMap::RAM_RANGE.start;
    m_memoryRange.length = MemoryMap::RAM_RANGE.length;
}

void RAM::loadExecutable(uint32_t baseAddr, const std::vector<uint8_t> &code)
{
    uint32_t mappedBase = MemoryMap::mapAddress(baseAddr);
    if (m_memoryRange.contains(mappedBase)) {
        uint32_t physicalAddr = m_memoryRange.remap(mappedBase);
        std::memcpy(&m_data[physicalAddr], &code[0], code.size());
    }
}
