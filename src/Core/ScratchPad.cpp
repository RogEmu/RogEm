#include "ScratchPad.hpp"

#include "MemoryMap.hpp"
#include "Bus.hpp"

ScratchPad::ScratchPad(Bus *bus) :
    Memory(bus, MemoryMap::SCRATCHPAD_RANGE.length, 0)
{
    m_memoryRange.start = MemoryMap::SCRATCHPAD_RANGE.start;
    m_memoryRange.length = MemoryMap::SCRATCHPAD_RANGE.length;
}

ScratchPad::~ScratchPad()
{
}
