/*
** EPITECH PROJECT, 2026
** rogem
** File description:
** DigitalPad
*/

#include "DigitalPad.hpp"
#include "StateBuffer.hpp"

#include <spdlog/spdlog.h>

DigitalPad::DigitalPad()
{
    reset();
}

DigitalPad::~DigitalPad()
{
}

void DigitalPad::tx(uint16_t value)
{
    m_tx = static_cast<uint8_t>(value);
}

uint16_t DigitalPad::rx()
{
    uint16_t rxVal = 0xFFFF;

    if (!m_connected) {
        return rxVal;
    }

    switch (m_state) {
        case PadSequenceState::HighZ:
            if (m_tx == 0x01) {
                m_state = PadSequenceState::IDLO;
                rxVal = 0xFF;
            }
            break;
        case PadSequenceState::IDLO:
            if (m_tx == 0x42) {
                m_state = PadSequenceState::IDHI;
                rxVal = 0x41;
            }
            break;
        case PadSequenceState::IDHI:
            m_state = PadSequenceState::MOT0;
            rxVal = 0x5A;
            break;
        case PadSequenceState::MOT0:
            m_state = PadSequenceState::MOT1;
            rxVal = m_buttons & 0xFF;
            break;
        case PadSequenceState::MOT1:
            m_state = PadSequenceState::HighZ;
            rxVal = (m_buttons >> 8) & 0xFF;
            break;
        default:
            break;
    }
    return rxVal;
}

void DigitalPad::reset()
{
    m_buttons = 0xFFFF;
    m_state = PadSequenceState::HighZ;
    m_tx = 0;
    m_connected = false;
}

void DigitalPad::serialize(StateBuffer &buf) const
{
    buf.write(m_state);
    buf.write(m_buttons);
    buf.write(m_tx);
    buf.write(m_connected);
}

void DigitalPad::deserialize(StateBuffer &buf)
{
    buf.read(m_state);
    buf.read(m_buttons);
    buf.read(m_tx);
    buf.read(m_connected);
}
