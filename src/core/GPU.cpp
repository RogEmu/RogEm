#include "GPU.hpp"

#include <spdlog/spdlog.h>

GPU::GPU() :
    PsxDevice()
{
    m_memoryRange = MemoryMap::GPU_REGISTERS_RANGE;
    reset();
}

GPU::~GPU()
{
}

void GPU::reset()
{
    m_statRegister = 0x10000000;
    m_cmdResponse = 0;
}


void GPU::write8(uint8_t /* value */, uint32_t /* address */)
{
    spdlog::warn("GPU: write byte is unhandled");
}

void GPU::write16(uint16_t /* value */, uint32_t /* address */)
{
    spdlog::warn("GPU: write halfword is unhandled");
}

void GPU::write32(uint32_t value, uint32_t address)
{
    spdlog::debug("GPU: Write 0x{:08X} to 0x{:08X}", value, address);
}


uint8_t GPU::read8(uint32_t /* address */)
{
    spdlog::warn("GPU: read byte is unhandled");
    return 0;
}

uint16_t GPU::read16(uint32_t /* address */)
{
    spdlog::warn("GPU: read halfword is unhandled");
    return 0;
}

uint32_t GPU::read32(uint32_t address)
{
    spdlog::debug("GPU: read from 0x{:08X}", address);
    if (address == 0x1F801814) {
        return m_statRegister;
    } else if (address == 0x1F801810) {
        return m_cmdResponse;
    }
    return 0;
}
