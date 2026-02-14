#include "MemoryControl1.hpp"

#include <spdlog/spdlog.h>

#include "Bus.hpp"

MemoryControl1::MemoryControl1(Bus *bus) :
    PsxDevice(bus)
{
    m_memoryRange = MemoryMap::MEMORY_CONTROL_1_RANGE;
}

MemoryControl1::~MemoryControl1()
{
}

void MemoryControl1::write8(uint8_t /* value */, uint32_t /* address */)
{
    spdlog::error("Memory Control 1: Unhandled write byte");
}

void MemoryControl1::write16(uint16_t /* value */, uint32_t /* address */)
{
    spdlog::error("Memory Control 1: Unhandled write halfword");
}

void MemoryControl1::write32(uint32_t value, uint32_t address)
{
    spdlog::trace("Memory Control 1: Write word 0x{:08X} at 0x{:08X}", value, address);
    uint32_t offset = mapAddress(address) / 4;

    if (offset < m_memoryRange.length / 4) {
        m_registers[offset] = value;
    }
}

uint8_t MemoryControl1::read8(uint32_t /* address */)
{
    spdlog::error("Memory Control 1: Unhandled read byte");
    return 0;
}

uint16_t MemoryControl1::read16(uint32_t /* address */)
{
    spdlog::error("Memory Control 1: Unhandled read halfword");
    return 0;
}

uint32_t MemoryControl1::read32(uint32_t address)
{
    spdlog::trace("Memory Control 1: Read word at 0x{:08X}", address);
    uint32_t offset = mapAddress(address) / 4;

    if (offset < m_memoryRange.length / 4) {
        return m_registers[offset];
    }
    return 0;
}
