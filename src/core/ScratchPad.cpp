#include "ScratchPad.hpp"

#include "MemoryMap.hpp"

ScratchPad::ScratchPad() :
    Memory(MemoryMap::SCRATCHPAD_RANGE.length, 0)
{
    m_memoryRange.start = MemoryMap::SCRATCHPAD_RANGE.start;
    m_memoryRange.length = MemoryMap::SCRATCHPAD_RANGE.length;
}

ScratchPad::~ScratchPad()
{
}
