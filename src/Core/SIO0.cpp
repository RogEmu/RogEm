#include "SIO0.hpp"

#include <spdlog/spdlog.h>

static constexpr int IRQ_TIMER = 1000;

SIO0::SIO0()
{
}

SIO0::~SIO0()
{
}

void SIO0::write(SIORegister reg, uint16_t value)
{
    switch (reg) {
        case SIORegister::TX_DATA:
            m_baudTimer = IRQ_TIMER;
            break;
        case SIORegister::MODE:
            m_mode = value;
            break;
        case SIORegister::CTRL:
            m_ctrl = value;
            break;
        case SIORegister::BAUD:
            m_baud = value;
            break;
        default:
            spdlog::error("SIO0: Write halfword 0x{:04X} at offset 0x{:02X}", value, static_cast<uint8_t>(reg));
            break;
    }
}

uint16_t SIO0::read(SIORegister reg)
{
    switch (reg) {
        case SIORegister::RX_DATA:
            return 0;
        case SIORegister::STAT:
            return 0x07;
        case SIORegister::MODE:
            return m_mode;
        case SIORegister::CTRL:
            return m_ctrl;
        case SIORegister::BAUD:
            return m_baud;
        default:
            spdlog::error("SIO0: Read halfword at offset 0x{:02X}", static_cast<uint8_t>(reg));
            return 0;
    }
}

void SIO0::update(int cycles)
{
    if (m_baudTimer > 0) {
        m_baudTimer -= cycles;
        if (m_baudTimer <= 0) {
            spdlog::debug("SIO0: IRQ triggered");
            m_baudTimer = 0;
        }
    }
}

void SIO0::reset()
{
    for (auto &pad : m_pad) {
        pad.reset();
    }
}
