#include "DMA.hpp"

#include <spdlog/spdlog.h>

#include "MemoryMap.hpp"

DMA::DMA() :
    PsxDevice()
{
    m_memoryRange = MemoryMap::DMA_REGISTERS_RANGE;
}

DMA::~DMA()
{
}

void DMA::write8(uint8_t value, uint32_t address)
{
    spdlog::debug("DMA: Write byte 0x{:02X} to 0x{:08X}", value, address);
}

void DMA::write16(uint16_t value, uint32_t address)
{
    spdlog::debug("DMA: Write halfword 0x{:04X} to 0x{:08X}", value, address);
}

void DMA::write32(uint32_t value, uint32_t address)
{
    spdlog::debug("DMA: Write word 0x{:08X} to 0x{:08X}", value, address);
}

uint8_t DMA::read8(uint32_t address)
{
    spdlog::debug("DMA: Read byte at 0x{:08X}", address);
    return 0;
}

uint16_t DMA::read16(uint32_t address)
{
    spdlog::debug("DMA: Read halfword at 0x{:08X}", address);
    return 0;
}

uint32_t DMA::read32(uint32_t address)
{
    spdlog::debug("DMA: Read word at 0x{:08X}", address);
    return 0;
}
