#include "InterruptController.hpp"

#include <spdlog/spdlog.h>

#include "MemoryMap.hpp"
#include "Bus.hpp"
#include "CPU.hpp"

InterruptController::InterruptController(Bus *bus) :
    PsxDevice(bus)
{
    m_memoryRange = MemoryMap::INTERRUPT_CONTROL_RANGE;
    m_istat = 0;
    m_imask = 0;
}

InterruptController::~InterruptController()
{
}

void InterruptController::write8(uint8_t value, uint32_t address)
{
    spdlog::debug("IRQ Controller: Write byte 0x{:02X} to 0x{:08X}", value, address);
    uint8_t offset = address & 0x7;

    switch (offset)
    {
    case 0: m_istat &= static_cast<uint32_t>(value); break;
    case 1: m_istat &= static_cast<uint32_t>(value) << 8; break;
    case 2: m_istat &= static_cast<uint32_t>(value) << 16; break;
    case 3: m_istat &= static_cast<uint32_t>(value) << 24; break;
    case 4: m_imask = static_cast<uint32_t>(value); break;
    case 5: m_imask = static_cast<uint32_t>(value) << 8; break;
    case 6: m_imask = static_cast<uint32_t>(value) << 16; break;
    case 7: m_imask = static_cast<uint32_t>(value) << 24; break;
    default:
        break;
    }

    setCpuIrqPending(irqPending());
}

void InterruptController::write16(uint16_t value, uint32_t address)
{
    spdlog::debug("IRQ Controller: Write halfword 0x{:04X} to 0x{:08X}", value, address);
    uint8_t offset = address & 0x7;

    switch (offset)
    {
    case 0: m_istat &= static_cast<uint32_t>(value); break;
    case 2: m_istat &= static_cast<uint32_t>(value) << 16; break;
    case 4: m_imask = static_cast<uint32_t>(value); break;
    case 6: m_imask = static_cast<uint32_t>(value) << 16; break;
    default:
        break;
    }

    setCpuIrqPending(irqPending());
}

void InterruptController::write32(uint32_t value, uint32_t address)
{
    spdlog::debug("IRQ Controller: Write word 0x{:08X} to 0x{:08X}", value, address);
    uint8_t offset = address & 0x7;

    switch (offset)
    {
    case 0: m_istat &= value; break;
    case 4: m_imask = value; break;
    default:
        break;
    }

    setCpuIrqPending(irqPending());
}

uint8_t InterruptController::read8(uint32_t address)
{
    spdlog::debug("IRQ Controller: Read byte at 0x{:08X}", address);
    uint8_t offset = address & 0x7;
    switch (offset)
    {
    case 0: return m_istat & 0xFF;
    case 1: return (m_istat >> 8) & 0xFF;
    case 2: return (m_istat >> 16) & 0xFF;
    case 3: return (m_istat >> 24) & 0xFF;
    case 4: return m_imask & 0xFF;
    case 5: return (m_imask >> 8) & 0xFF;
    case 6: return (m_imask >> 16) & 0xFF;
    case 7: return (m_imask >> 24) & 0xFF;
    default:
        break;
    }
    return 0;
}

uint16_t InterruptController::read16(uint32_t address)
{
    spdlog::debug("IRQ Controller: Read halfword at 0x{:08X}", address);
    uint8_t offset = address & 0x7;
    switch (offset)
    {
    case 0: return m_istat & 0xFFFF;
    case 2: return (m_istat >> 16) & 0xFFFF;
    case 4: return m_imask & 0xFFFF;
    case 6: return (m_imask >> 16) & 0xFFFF;
    default:
        break;
    }
    return 0;
}

uint32_t InterruptController::read32(uint32_t address)
{
    spdlog::debug("IRQ Controller: Read word at 0x{:08X}", address);
    uint8_t offset = address & 0x7;

    switch (offset)
    {
    case 0: return m_istat;
    case 4: return m_imask;
    default:
        return 0;
    }
}

void InterruptController::triggerIRQ(DeviceIRQ device)
{
    m_istat |= static_cast<uint32_t>(device);
    setCpuIrqPending(true);
}

bool InterruptController::irqPending()
{
    return (m_istat & m_imask);
}

void InterruptController::setCpuIrqPending(bool pending)
{
    CPU *cpu = m_bus->getCpu();
    if (!cpu) {
        spdlog::error("IRQ Controller: CPU is not connected to the bus!");
        return;
    }
    cpu->setInterruptPending(pending);
}
