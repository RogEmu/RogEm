#include "Expansion2.hpp"

#include <spdlog/spdlog.h>

Expansion2::Expansion2(Bus *bus) :
    PsxDevice(bus)
{
    m_memoryRange = MemoryMap::EXP2_RANGE;
}

Expansion2::~Expansion2()
{
}

void Expansion2::write8(uint8_t value, uint32_t address)
{
    spdlog::warn("Expansion 2: Write byte 0x{:02X} to 0x{:08X} is unhandled", value, address);
}

void Expansion2::write16(uint16_t value, uint32_t address)
{
    spdlog::warn("Expansion 2: Write halfword 0x{:04X} to 0x{:08X} is unhandled", value, address);
}

void Expansion2::write32(uint32_t value, uint32_t address)
{
    spdlog::warn("Expansion 2: Write word 0x{:08X} to 0x{:08X} is unhandled", value, address);
}

uint8_t Expansion2::read8(uint32_t address)
{
    spdlog::warn("Expansion 2: Read byte from 0x{:08X} is unhandled", address);
    return 0;
}

uint16_t Expansion2::read16(uint32_t address)
{
    spdlog::warn("Expansion 2: Read halfword from 0x{:08X} is unhandled", address);
    return 0;
}

uint32_t Expansion2::read32(uint32_t address)
{
    spdlog::warn("Expansion 2: Read word from 0x{:08X} is unhandled", address);
    return 0;
}
