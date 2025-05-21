#include "SPU.hpp"

#include <fmt/format.h>

#include "MemoryMap.hpp"

SPU::SPU() :
    PsxDevice()
{
    m_memoryRange = MemoryMap::SPU_CONTROL_REGS_RANGE;
}

SPU::~SPU()
{
}

void SPU::write8(uint8_t value, uint32_t address)
{
    fmt::println("SPU: Write byte 0x{:02X} to 0x{:08X}", value, address);
}

void SPU::write16(uint16_t value, uint32_t address)
{
    fmt::println("SPU: Write halfword 0x{:04X} to 0x{:08X}", value, address);
}

void SPU::write32(uint32_t value, uint32_t address)
{
    fmt::println("SPU: Write word 0x{:08X} to 0x{:08X}", value, address);
}

uint8_t SPU::read8(uint32_t address)
{
    fmt::println("SPU: Read byte at 0x{:08X}", address);
    return 0;
}

uint16_t SPU::read16(uint32_t address)
{
    fmt::println("SPU: Read halfword at 0x{:08X}", address);
    return 0;
}

uint32_t SPU::read32(uint32_t address)
{
    fmt::println("SPU: Read word at 0x{:08X}", address);
    return 0;
}
