#include "Timers.hpp"

#include <spdlog/spdlog.h>

#include "MemoryMap.hpp"
#include "Bus.hpp"
#include "InterruptController.hpp"

Timers::Timers(Bus *bus) :
    PsxDevice(bus)
{
    m_memoryRange = MemoryMap::TIMERS_RANGE;
    for (auto &timer : m_timers) {
        memset(&timer, 0, sizeof(timer));
    }
}

Timers::~Timers()
{
}

void Timers::update(int cycles)
{
    for (uint8_t i = 0; i < 3; i++) {
        updateTimer(i, cycles);
    }
}

void Timers::updateTimer(uint8_t index, int cycles)
{
    Timer &timer = m_timers[index];

    if (timer.paused) {
        return;
    }

    timer.currentValue += computeTimerCycles(cycles, index);

    if (timer.currentValue >= (timer.targetValue & 0xFFFF)) {
        timer.mode.reachedTarget = true;

        if (timer.mode.irqTarget) {
            triggerIRQ(index);
        }

        if (timer.mode.resetCounter) {
            timer.currentValue = 0;
        }
    }

    if (timer.currentValue >= 0xFFFF) {
        timer.mode.reachedMax = true;

        if (timer.mode.irqMax) {
            triggerIRQ(index);
        }

        if (!timer.mode.resetCounter) {
            timer.currentValue = 0;
        }
    }
}

int Timers::computeTimerCycles(int cycles, uint8_t index)
{
    Timer &timer = m_timers[index];

    switch (index) {
        case 0: return (timer.mode.clockSource & 0b01) ? cycles : cycles;
        case 1: return (timer.mode.clockSource & 0b01) ? 0 : cycles;
        case 2: return (timer.mode.clockSource & 0b10) ? cycles / 8 : cycles;
        default:
            return 0;
    }
}

void Timers::triggerIRQ(uint8_t index)
{
    Timer &timer = m_timers[index];

    // Set IRQ bit
    timer.mode.irq = true;

    // Handle IRQ once mode
    if (timer.mode.irqOnce) {
        // Disable further IRQs after first trigger
        timer.mode.irqTarget = false;
        timer.mode.irqMax = false;
    }
    spdlog::trace("Timers: Timer {} IRQ triggered", index);
    auto irqc = m_bus->getDevice<InterruptController>();
    if (irqc) {
        auto deviceIrq = DeviceIRQ::TIMER0;
        if (index == 1) {
            deviceIrq = DeviceIRQ::TIMER1;
        } else if (index == 2) {
            deviceIrq = DeviceIRQ::TIMER2;
        }
        irqc->triggerIRQ(deviceIrq);
    }
}

void Timers::onHBlank()
{
    if ((m_timers[1].mode.clockSource & 0b01) && !m_timers[1].paused) {
        m_timers[1].currentValue++;
    }
    if (!m_timers[0].mode.syncEnable) {
        return;
    }
    switch (m_timers[0].mode.syncMode) {
        case 0: m_timers[0].paused = true; break;
        case 1: m_timers[0].currentValue = 0; break;
        case 2:
            m_timers[0].currentValue = 0;
            m_timers[0].paused = 0;
            break;
        case 3:
            m_timers[0].paused = false;
            m_timers[0].mode.syncEnable = false;
            break;
        default:
            break;
    }
}

void Timers::onHBlankEnd()
{
    if (!m_timers[0].mode.syncEnable) {
        return;
    }
    switch (m_timers[0].mode.syncMode) {
        case 0: m_timers[0].paused = false; break;
        case 2: m_timers[0].paused = true; break;
        default:
            break;
    }
}

void Timers::onVBlank()
{
    if (!m_timers[1].mode.syncEnable) {
        return;
    }
    switch (m_timers[1].mode.syncMode) {
        case 0: m_timers[1].paused = true; break;
        case 1: m_timers[1].currentValue = 0; break;
        case 2:
            m_timers[1].currentValue = 0;
            m_timers[1].paused = 0;
            break;
        case 3:
            m_timers[1].paused = false;
            m_timers[1].mode.syncEnable = false;
            break;
        default:
            break;
    }
}

void Timers::onVBlankEnd()
{
    if (!m_timers[1].mode.syncEnable) {
        return;
    }
    switch (m_timers[1].mode.syncMode) {
        case 0: m_timers[1].paused = false; break;
        case 2: m_timers[1].paused = true; break;
        default:
            break;
    }
}

void Timers::write8(uint8_t value, uint32_t address)
{
    spdlog::error("Timers: Unhandled write byte 0x{:02x} to 0x{:08X}", value, address);
}

void Timers::write16(uint16_t value, uint32_t address)
{
    spdlog::trace("Timers: Write halfword 0x{:04X} to 0x{:08X}", value, address);
    writeTimer(address, value);
}

void Timers::write32(uint32_t value, uint32_t address)
{
    spdlog::trace("Timers: Write word 0x{:08X} to 0x{:08X}", value, address);
    writeTimer(address, value);
}

uint8_t Timers::read8(uint32_t address)
{
    spdlog::error("Timers: Unhandled read byte at 0x{:08X}", address);
    return 0;
}

uint16_t Timers::read16(uint32_t address)
{
    spdlog::trace("Timers: Read halfword at 0x{:08X}", address);
    return static_cast<uint16_t>(readTimer(address));
}

uint32_t Timers::read32(uint32_t address)
{
    spdlog::trace("Timers: Read word at 0x{:08X}", address);
    return readTimer(address);
}

uint32_t Timers::readTimer(uint32_t address)
{
    uint8_t timer = (address & 0x30) >> 4;
    uint8_t offset = address & 0xF;

    switch (offset)
    {
        case 0: return m_timers[timer].currentValue & 0xFFFF;
        case 4: return m_timers[timer].rawMode;
        case 8: return m_timers[timer].targetValue & 0xFFFF;
        default:
            break;
    }
    return 0;
}

void Timers::writeTimer(uint32_t address, uint32_t value)
{
    uint8_t timer = (address & 0x30) >> 4;
    uint8_t offset = address & 0xF;

    switch (offset)
    {
        case 0: m_timers[timer].currentValue = value & 0xFFFF; break;
        case 4: m_timers[timer].rawMode = value; break;
        case 8: m_timers[timer].targetValue = value & 0xFFFF; break;
        default:
            break;
    }
}
