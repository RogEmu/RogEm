#include "MemoryControl2.hpp"
#include "StateBuffer.hpp"

#include <spdlog/spdlog.h>

MemoryControl2::MemoryControl2(Bus *bus) :
    PsxDevice(bus),
    m_memControl2(0)
{
    m_memoryRange = MemoryMap::MEMORY_CONTROL_2_RANGE;
}

MemoryControl2::~MemoryControl2()
{
}

void MemoryControl2::serialize(StateBuffer &buf) const
{
    buf.write(m_memControl2);
}

void MemoryControl2::deserialize(StateBuffer &buf)
{
    buf.read(m_memControl2);
}

void MemoryControl2::write8(uint8_t value, uint32_t address)
{
    (void)value;
    spdlog::warn("Memory Control 2: Write byte to 0x{:08X} is unhandled", address);
}

void MemoryControl2::write16(uint16_t value, uint32_t address)
{
    spdlog::trace("Memory Control 2: Write halfword 0x{:04X} to 0x{:08X}", value, address);
    m_memControl2 = (m_memControl2 & 0xFFFF0000) | value;
}

void MemoryControl2::write32(uint32_t value, uint32_t address)
{
    spdlog::trace("Memory Control 2: Write word 0x{:08X} to 0x{:08X}", value, address);
    m_memControl2 = value;
}

uint8_t MemoryControl2::read8(uint32_t address)
{
    spdlog::warn("Memory Control 2: Read byte from 0x{:08X} is unhandled", address);
    return 0;
}

uint16_t MemoryControl2::read16(uint32_t address)
{
    spdlog::warn("Memory Control 2: Read halfword from 0x{:08X} -> 0x{:04X}", address, m_memControl2 & 0xFFFF);
    return static_cast<uint16_t>(m_memControl2 & 0xFFFF);
}

uint32_t MemoryControl2::read32(uint32_t address)
{
    spdlog::warn("Memory Control 2: Read word from 0x{:08X} -> 0x{:08X}", address, m_memControl2);
    return m_memControl2;
}
