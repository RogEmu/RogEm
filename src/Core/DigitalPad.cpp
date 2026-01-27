/*
** EPITECH PROJECT, 2026
** rogem
** File description:
** DigitalPad
*/

#include "DigitalPad.hpp"

#include <spdlog/spdlog.h>

DigitalPad::DigitalPad() :
    m_state(PadSequenceState::HighZ),
    m_buttons(0xFFFF)
{
}

DigitalPad::~DigitalPad()
{
}

void DigitalPad::tx(uint16_t value)
{
    spdlog::debug("Digital Pad: TX value 0x{:04X}", value);
}

uint16_t DigitalPad::rx()
{
    switch (m_state) {
        case PadSequenceState::HighZ:
            m_state = PadSequenceState::IDLO;
            return 0xFF;
        case PadSequenceState::IDLO:
            m_state = PadSequenceState::IDHI;
            return 0x41;
        case PadSequenceState::IDHI:
            m_state = PadSequenceState::MOT0;
            return 0x5A;
        case PadSequenceState::MOT0:
            m_state = PadSequenceState::MOT1;
            return m_buttons & 0xFF;
        case PadSequenceState::MOT1:
            m_state = PadSequenceState::HighZ;
            return (m_buttons >> 8) & 0xFF;
        default:
            break;
    }
    return 0xFFFF;
}

void DigitalPad::reset()
{
    m_buttons = 0xFFFF;
    m_state = PadSequenceState::HighZ;
}
