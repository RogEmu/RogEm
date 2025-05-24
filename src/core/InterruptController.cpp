#include "InterruptController.hpp"

#include <spdlog/spdlog.h>

#include "MemoryMap.hpp"

InterruptController::InterruptController()
{
    m_memoryRange = MemoryMap::INTERRUPT_CONTROL_RANGE;
}

InterruptController::~InterruptController()
{
}

void InterruptController::write8(uint8_t value, uint32_t address)
{
    spdlog::debug("IRQ Controller: Write byte 0x{:02X} to 0x{:08X}", value, address);
}

void InterruptController::write16(uint16_t value, uint32_t address)
{
    spdlog::debug("IRQ Controller: Write halfword 0x{:04X} to 0x{:08X}", value, address);
}

void InterruptController::write32(uint32_t value, uint32_t address)
{
    spdlog::debug("IRQ Controller: Write word 0x{:08X} to 0x{:08X}", value, address);
}

uint8_t InterruptController::read8(uint32_t address)
{
    spdlog::debug("IRQ Controller: Read byte at 0x{:08X}", address);
    return 0;
}

uint16_t InterruptController::read16(uint32_t address)
{
    spdlog::debug("IRQ Controller: Read halfword at 0x{:08X}", address);
    return 0;
}

uint32_t InterruptController::read32(uint32_t address)
{
    spdlog::debug("IRQ Controller: Read word at 0x{:08X}", address);
    return 0;
}
