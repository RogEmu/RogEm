/*
** EPITECH PROJECT, 2025
** RogEm
** File description:
** Analog Pad
*/

#pragma once
#include "SIODevice.hpp"
#include <cstdint>

class PadAnalog : public SIODevice {
public:
    // Buttons are still active-LOW (0 = pressed)
    void setButtons(uint16_t activeLowMask) { m_buttons = activeLowMask; }

    // Sticks are 0..255, center ≈ 128. Order: Rx, Ry, Lx, Ly (common layout)
    void setSticks(uint8_t rx, uint8_t ry, uint8_t lx, uint8_t ly) {
        m_rx = rx; m_ry = ry; m_lx = lx; m_ly = ly;
    }

    bool onByteFromConsole(uint8_t tx, uint8_t& rx, bool& ackNow, int& ackDelayCycles) override;
    void endTransaction() override { m_phase = Phase::Address; }

private:
    enum class Phase { 
        Address, 
        Command, 
        ReplyIdLo, 
        ReplyIdHi, 
        ReplySwLo, 
        ReplySwHi,
        ReplyRx, 
        ReplyRy, 
        ReplyLx, 
        ReplyLy, 
        Done 
    } m_phase = Phase::Address;

    uint16_t m_buttons = 0xFFFF;
    uint8_t  m_rx = 0x80, m_ry = 0x80, m_lx = 0x80, m_ly = 0x80;
};
