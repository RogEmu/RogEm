#include "ScratchPad.hpp"
#include "StateBuffer.hpp"

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

void ScratchPad::serialize(StateBuffer &buf) const
{
    buf.writeVec(m_data);
}

void ScratchPad::deserialize(StateBuffer &buf)
{
    buf.readVec(m_data);
}
