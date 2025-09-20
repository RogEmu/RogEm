#include "PadDigital.hpp"

void PadDigital::setButtons(uint16_t activeLowMask) {
    m_buttons = activeLowMask;
}

bool PadDigital::onByteFromConsole(uint8_t tx, uint8_t& rx, bool& ackNow, int& ackDelayCycles) {
    ackNow = true;          // MVP: /ACK immediately
    ackDelayCycles = 20;    // small safe delay; tune later

    switch (m_phase) {
        case Phase::Address:
            // Console should send 0x01 for controller; we ignore rx here
            rx = 0xFF; // floating/undefined during address
            m_phase = Phase::Command;
            return true;

        case Phase::Command:
            // Expect 0x42 "Read Data"
            if (tx == 0x42) {
                rx = 0xFF; // command echo doesn't matter
                m_phase = Phase::ReplyIdLo;
            } else {
                // Unsupported command; end
                rx = 0xFF;
                m_phase = Phase::Done;
            }
            return true;

        case Phase::ReplyIdLo:
            rx = 0x41;              // ID low
            m_phase = Phase::ReplyIdHi;
            return true;

        case Phase::ReplyIdHi:
            rx = 0x5A;              // ID high
            m_phase = Phase::ReplySwLo;
            return true;

        case Phase::ReplySwLo:
            rx = static_cast<uint8_t>(m_buttons & 0xFF);    // buttons low
            m_phase = Phase::ReplySwHi;
            return true;

        case Phase::ReplySwHi:
            rx = static_cast<uint8_t>((m_buttons >> 8) & 0xFF); // buttons high
            m_phase = Phase::Done;
            return true;

        case Phase::Done:
        default:
            rx = 0xFF;
            return false;
    }
}

void PadDigital::endTransaction() {
    m_phase = Phase::Address;
}
