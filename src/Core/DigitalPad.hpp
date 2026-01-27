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

class DigitalPad
{
    public:
        DigitalPad();
        ~DigitalPad();

        void tx(uint16_t value);
        uint16_t rx();

        void reset();

    private:
        PadSequenceState m_state;
        uint16_t m_buttons;
};

#endif /* !DIGITALPAD_HPP_ */
