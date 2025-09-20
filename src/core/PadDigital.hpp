/*
** EPITECH PROJECT, 2025
** RogEm
** File description:
** Digital Pad
*/

#pragma once
#include "SIODevice.hpp"
#include <array>

// Simple digital controller (SCPH-1080). ID = 0x5A41.
// Returns: 0xFF during address, then: 0x41, 0x5A, swLo, swHi and stops.
class PadDigital : public SIODevice {
public:
    // Button bit layout (active LOW). You can adapt to match your host mapping later.
    enum Button : uint16_t {
        Select = 1 << 0,  L3 = 1 << 1, R3 = 1 << 2, Start = 1 << 3,
        Up     = 1 << 4,  Right = 1 << 5, Down = 1 << 6, Left  = 1 << 7,
        L2     = 1 << 8,  R2    = 1 << 9, L1   = 1 << 10, R1   = 1 << 11,
        Triangle=1<<12,   Circle=1<<13, Cross=1<<14,     Square=1<<15
    };

    void setButtons(uint16_t activeLowMask); // feed from host (0 means pressed)

    // SIODevice
    bool onByteFromConsole(uint8_t tx, uint8_t& rx, bool& ackNow, int& ackDelayCycles) override;
    void endTransaction() override;
    bool isPresent() const override { return true; }

private:
    enum class Phase {
        Address, // first byte (0x01) – reply undefined; we return 0xFF
        Command, // expecting 0x42 for "Read Data"
        ReplyIdLo,  // 0x41
        ReplyIdHi,  // 0x5A
        ReplySwLo,
        ReplySwHi,
        Done
    } m_phase = Phase::Address;

    uint16_t m_buttons = 0xFFFF; // all released (active low)
};
