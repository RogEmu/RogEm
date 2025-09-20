#include "PadAnalog.hpp"

bool PadAnalog::onByteFromConsole(uint8_t /*tx*/, uint8_t& rx, bool& ackNow, int& ackDelayCycles) {
    ackNow = true; ackDelayCycles = 0; // MVP: instant ACK

    switch (m_phase) {
        case Phase::Address:
            rx = 0xFF;                   // undefined during address byte
            m_phase = Phase::Command;    // expect 0x42
            return true;

        case Phase::Command:
            // We’ll accept anything and just proceed like “read data” (0x42)
            rx = 0xFF;
            m_phase = Phase::ReplyIdLo;
            return true;

        case Phase::ReplyIdLo: 
            rx = 0x73; 
            m_phase = Phase::ReplyIdHi; 
            return true; // 0x5A73

        case Phase::ReplyIdHi: 
            rx = 0x5A; 
            m_phase = Phase::ReplySwLo; 
            return true;

        case Phase::ReplySwLo: 
            rx = uint8_t(m_buttons & 0xFF);
            m_phase = Phase::ReplySwHi;
            return true;

        case Phase::ReplySwHi:
            rx = uint8_t(m_buttons >> 8);
            m_phase = Phase::ReplyRx;
            return true;

        case Phase::ReplyRx:
            rx = m_rx;
            m_phase = Phase::ReplyRy;
            return true;

        case Phase::ReplyRy:
            rx = m_ry;
            m_phase = Phase::ReplyLx;
            return true;

        case Phase::ReplyLx:
            rx = m_lx;
            m_phase = Phase::ReplyLy;
            return true;

        case Phase::ReplyLy:
            rx = m_ly;
            m_phase = Phase::Done;
            return true;

        case Phase::Done: default:
            rx = 0xFF;
            return false;
    }
}
