#include "CacheControl.hpp"

#include <spdlog/spdlog.h>

CacheControl::CacheControl(Bus *bus) :
    PsxDevice(bus),
    m_cacheControl(0)
{
    m_memoryRange = MemoryMap::CACHE_CONTROL_RANGE;
}

CacheControl::~CacheControl()
{
}

void CacheControl::write8(uint8_t value, uint32_t address)
{
    (void)value;
    spdlog::warn("Cache Control: write byte to 0x{:08X} is unhandled", address);
}

void CacheControl::write16(uint16_t value, uint32_t address)
{
    spdlog::debug("Cache Control: write halfword 0x{:04X} to 0x{:08X}", value, address);
    if (address == 0xFFFE0130) {
        m_cacheControl = (m_cacheControl & 0xFFFF0000) | value;
    } else {
        spdlog::warn("Cache Control: write halfword to unknown address 0x{:08X}", address);
    }
}

void CacheControl::write32(uint32_t value, uint32_t address)
{
    spdlog::debug("Cache Control: Write word 0x{:08X} to 0x{:08X}", value, address);
    if (address == 0xFFFE0130) {
        m_cacheControl = value;
    } else {
        spdlog::warn("Cache Control: Write word to unknown address 0x{:08X}", address);
    }
}

uint8_t CacheControl::read8(uint32_t address)
{
    spdlog::warn("Cache Control: read byte from 0x{:08X} is unhandled", address);
    return 0;
}

uint16_t CacheControl::read16(uint32_t address)
{
    spdlog::debug("Cache Control: read halfword from 0x{:08X}", address);
    if (address == 0xFFFE0130) {
        return static_cast<uint16_t>(m_cacheControl & 0xFFFF);
    } else {
        spdlog::warn("Cache Control: read halfword from unknown address 0x{:08X}", address);
        return 0;
    }
}

uint32_t CacheControl::read32(uint32_t address)
{
    spdlog::debug("Cache Control: read word from 0x{:08X}", address);
    if (address == 0xFFFE0130) {
        return m_cacheControl;
    } else {
        spdlog::warn("Cache Control: read word from unknown address 0x{:08X}", address);
        return 0;
    }
}
