/*
** EPITECH PROJECT, 2026
** rogem
** File description:
** DigitalPad
*/

#ifndef DIGITALPAD_HPP_
#define DIGITALPAD_HPP_

#include "SIODevice.hpp"

enum class PadSequenceState {
    HighZ,
    IDLO,
    IDHI,
    MOT0,
    MOT1
};

enum PadButton
{
    PAD_SELECT    = 0x0001,
    PAD_L3        = 0x0002,
    PAD_R3        = 0x0004,
    PAD_START     = 0x0008,
    PAD_JOYUP     = 0x0010,
    PAD_JOYRIGHT  = 0x0020,
    PAD_JOYDOWN   = 0x0040,
    PAD_JOYLEFT   = 0x0080,
    PAD_L2        = 0x0100,
    PAD_R2        = 0x0200,
    PAD_L1        = 0x0400,
    PAD_R1        = 0x0800,
    PAD_TRIANGLE  = 0x1000,
    PAD_CIRCLE    = 0x2000,
    PAD_CROSS     = 0x4000,
    PAD_SQUARE    = 0x8000,
    PAD_UNKOWN    = 0x10000
};

class DigitalPad
{
    public:
        DigitalPad();
        ~DigitalPad();

        void tx(uint16_t value);
        uint16_t rx();

        void reset();
        bool isConnected() const { return m_connected; }
        void connect() { m_connected = true; }
        void disconnect() { m_connected = false; }
        void updateButtons(uint16_t buttons) { m_buttons = buttons; }

    private:
        PadSequenceState m_state;
        uint16_t m_buttons;
        uint8_t m_tx;
        bool m_connected;
};

#endif /* !DIGITALPAD_HPP_ */
