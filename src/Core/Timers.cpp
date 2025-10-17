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

void Timers::update(int cycles)
{
    for (auto &timer : m_timers) {
        timer.currentValue += cycles;
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
    spdlog::debug("Timers: Write halfword 0x{:04X} to 0x{:08X}", value, address);
    writeTimer(address, value);
}

void Timers::write32(uint32_t value, uint32_t address)
{
    spdlog::debug("Timers: Write word 0x{:08X} to 0x{:08X}", value, address);
    writeTimer(address, value);
}

uint8_t Timers::read8(uint32_t address)
{
    spdlog::error("Timers: Unhandled read byte at 0x{:08X}", address);
    return 0;
}

uint16_t Timers::read16(uint32_t address)
{
    spdlog::debug("Timers: Read halfword at 0x{:08X}", address);
    return readTimer(address);
}

uint32_t Timers::read32(uint32_t address)
{
    spdlog::debug("Timers: Read word at 0x{:08X}", address);
    return readTimer(address);
}

uint32_t Timers::readTimer(uint32_t address)
{
    uint8_t timer = (address & 0x30) >> 4;
    uint8_t offset = address & 0xF;

    switch (offset)
    {
        case 0: return m_timers[timer].currentValue;
        case 4: return m_timers[timer].rawMode;
        case 8: return m_timers[timer].targetValue;
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
        case 0: m_timers[timer].currentValue = value; break;
        case 4: m_timers[timer].rawMode = value; break;
        case 8: m_timers[timer].targetValue = value; break;
        default:
            break;
    }
}
