#include "SIO0.hpp"
#include "StateBuffer.hpp"

#include <spdlog/spdlog.h>

static constexpr int IRQ_TIMER = 100;

SIO0::SIO0()
{
    reset();
}

SIO0::~SIO0()
{
}

void SIO0::write(SIORegister reg, uint16_t value)
{
    switch (reg) {
        case SIORegister::TX_DATA: {
            auto port = (m_ctrl >> 13) & 1;
            m_pad[port].tx(value);
            m_baudTimer = IRQ_TIMER;
            break;
        }
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
        case SIORegister::RX_DATA: {
            auto port = (m_ctrl >> 13) & 1;
            return m_pad[port].rx();
        }
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
            m_irq = true;
            m_baudTimer = 0;
        }
    }
}

void SIO0::reset()
{
    for (auto &pad : m_pad) {
        pad.reset();
    }
    m_irq = false;
    m_pad[0].connect();
}

void SIO0::serialize(StateBuffer &buf) const
{
    SIODevice::serialize(buf);
    m_pad[0].serialize(buf);
    m_pad[1].serialize(buf);
    buf.write(m_irq);
}

void SIO0::deserialize(StateBuffer &buf)
{
    SIODevice::deserialize(buf);
    m_pad[0].deserialize(buf);
    m_pad[1].deserialize(buf);
    buf.read(m_irq);
}
