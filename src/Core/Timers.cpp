#include "Timers.hpp"

#include <spdlog/spdlog.h>

#include "MemoryMap.hpp"
#include "Bus.hpp"

Timers::Timers(Bus *bus) :
    PsxDevice(bus)
{
    m_memoryRange = MemoryMap::TIMERS_RANGE;
}

Timers::~Timers()
{
}

void Timers::write8(uint8_t value, uint32_t address)
{
    spdlog::debug("Timers: Write byte 0x{:02X} to 0x{:08X}", value, address);
}

void Timers::write16(uint16_t value, uint32_t address)
{
    spdlog::debug("Timers: Write halfword 0x{:04X} to 0x{:08X}", value, address);
}

void Timers::write32(uint32_t value, uint32_t address)
{
    spdlog::debug("Timers: Write word 0x{:08X} to 0x{:08X}", value, address);
}

uint8_t Timers::read8(uint32_t address)
{
    spdlog::debug("Timers: Read byte at 0x{:08X}", address);
    return 0;
}

uint16_t Timers::read16(uint32_t address)
{
    spdlog::debug("Timers: Read halfword at 0x{:08X}", address);
    return 0;
}

uint32_t Timers::read32(uint32_t address)
{
    spdlog::debug("Timers: Read word at 0x{:08X}", address);
    return 0;
}
