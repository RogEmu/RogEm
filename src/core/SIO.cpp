#include "SIO.hpp"
#include "PadDigital.hpp"
#include "PadAnalog.hpp"
#include <cassert>
#include <memory> // for std::dynamic_pointer_cast

SIO::SIO(Bus* bus, InterruptController& irq) : PsxDevice(bus), m_irq(irq) {
    m_memoryRange = MemoryMap::MemRange{0x1F801040u, 0x10u};
    m_STAT = STAT_TX_EMPTY | STAT_TX_RDY;
}

void SIO::attachController(int portIndex, std::shared_ptr<SIODevice> dev) {
    assert(portIndex == 0 || portIndex == 1);
    m_pad[portIndex] = std::move(dev);
}

void SIO::setDSR(bool activeLow) {
    if (activeLow) m_STAT |= STAT_DSR; else m_STAT &= ~STAT_DSR;
}

void SIO::raiseIRQIfEnabled() {
    if (m_CTRL & CTRL_RX_IRQ_EN) {
        m_irq.triggerIRQ(DeviceIRQ::CONTROLLER_MEMCARD);
    };
}

uint8_t SIO::read8(uint32_t address) {
    switch (address & 0xF) {
        case 0x00: {
            uint8_t v = m_RX;
            m_STAT &= ~STAT_RX_RDY;
            return v;
        }
        case 0x04: return uint8_t(m_STAT & 0xFF);
        case 0x05: return uint8_t(m_STAT >> 8);
        case 0x08: return uint8_t(m_MODE & 0xFF);
        case 0x09: return uint8_t(m_MODE >> 8);
        case 0x0A: return uint8_t(m_CTRL & 0xFF);
        case 0x0B: return uint8_t(m_CTRL >> 8);
        case 0x0E: return uint8_t(m_BAUD & 0xFF);
        case 0x0F: return uint8_t(m_BAUD >> 8);
        default:   return 0xFF;
    }
}

uint16_t SIO::read16(uint32_t address) {
    uint16_t lo = read8(address);
    uint16_t hi = read8(address + 1);
    return lo | (hi << 8);
}

uint32_t SIO::read32(uint32_t address) {
    uint32_t b0 = read8(address);
    uint32_t b1 = read8(address + 1);
    uint32_t b2 = read8(address + 2);
    uint32_t b3 = read8(address + 3);
    return b0 | (b1<<8) | (b2<<16) | (b3<<24);
}

void SIO::write8(uint8_t value, uint32_t address) {
    switch (address & 0xF) {
        case 0x00: // TX
            m_TX = value;
            txByte(value);
            break;
        case 0x08: m_MODE = (m_MODE & 0xFF00) | value; break;
        case 0x09: m_MODE = (m_MODE & 0x00FF) | (uint16_t(value) << 8); break;
        case 0x0A: {
            uint16_t prev = m_CTRL;
            m_CTRL = (m_CTRL & 0xFF00) | value;
            bool prevCS = prev & CTRL_DTR;
            bool currCS = m_CTRL & CTRL_DTR;
            if (prevCS && !currCS) {
                // end transaction on currently selected port
                if (auto& dev = m_pad[m_selectedPort]) dev->endTransaction();
                m_csAsserted = false;
                setDSR(false);
            } else if (!prevCS && currCS) {
                m_csAsserted = true;
                setDSR(false);
            }
            break;
        }
        case 0x0B: m_CTRL = (m_CTRL & 0x00FF) | (uint16_t(value) << 8); break;
        case 0x0E: m_BAUD = (m_BAUD & 0xFF00) | value; break;
        case 0x0F: m_BAUD = (m_BAUD & 0x00FF) | (uint16_t(value) << 8); break;
        default: break;
    }
}

void SIO::write16(uint16_t value, uint32_t address) {
    write8(uint8_t(value & 0xFF), address);
    write8(uint8_t(value >> 8), address + 1);
}

void SIO::write32(uint32_t value, uint32_t address) {
    write8(uint8_t(value & 0xFF), address);
    write8(uint8_t((value >> 8) & 0xFF), address + 1);
    write8(uint8_t((value >> 16) & 0xFF), address + 2);
    write8(uint8_t((value >> 24) & 0xFF), address + 3);
}

void SIO::txByte(uint8_t value) {
    // TX becomes not-empty briefly
    m_STAT &= ~STAT_TX_EMPTY;
    m_STAT |=  STAT_TX_RDY;

    // Select port by CTRL bit13
    m_selectedPort = (m_CTRL & CTRL_PORT_SEL) ? 1 : 0;

    // /CS must be asserted (DTR=1)
    if ((m_CTRL & CTRL_DTR) == 0) {
        // /CS not asserted → end/idle
        if (m_csAsserted) {
            if (auto& dev = m_pad[m_selectedPort]) dev->endTransaction();
        }
        m_csAsserted = false;
        setDSR(false);
        m_STAT |= STAT_TX_EMPTY | STAT_TX_RDY;
        return;
    }

    if (!m_csAsserted) {
        m_csAsserted = true;
        setDSR(false);
    }

    uint8_t rx = 0xFF;
    bool ack = false;
    int ackDelay = 0; // MVP: immediate /ACK

    if (auto& dev = m_pad[m_selectedPort]; dev && dev->isPresent()) {
        dev->onByteFromConsole(value, rx, ack, ackDelay);
    } else {
        rx = 0xFF; // floating bus
    }

    // Make RX data visible to the CPU
    m_RX = rx;
    m_STAT |= STAT_RX_RDY;
    m_STAT |= STAT_TX_EMPTY | STAT_TX_RDY;

    // Pulse /ACK and raise IRQ immediately (simple but works)
    if (ack) {
        setDSR(true);
        raiseIRQIfEnabled();
        setDSR(false);
    }
}

void SIO::setControllerButtons(int portIndex, uint16_t activeLowMask) {
    if (portIndex < 0 || portIndex > 1) return;
    if (!m_pad[portIndex]) return;

    // Only digital pad for now; extend later for analog types !!! (NO JOYSTICK) !!!
    if (auto pd = std::dynamic_pointer_cast<PadDigital>(m_pad[portIndex])) {
        pd->setButtons(activeLowMask);
    } else if (auto pa = std::dynamic_pointer_cast<PadAnalog>(m_pad[portIndex])) {
        pa->setButtons(activeLowMask);
    }
}

void SIO::setControllerAnalog(int portIndex, uint16_t activeLowMask,
                              uint8_t lx, uint8_t ly, uint8_t rx, uint8_t ry) {
    ensurePadType(portIndex, SioPadType::Analog);
    if (auto pa = std::dynamic_pointer_cast<PadAnalog>(m_pad[portIndex])) {
        pa->setButtons(activeLowMask);
        pa->setSticks(rx, ry, lx, ly); // Some games require a swap between those 4 variables, to implement later
    }
}

void SIO::ensurePadType(int portIndex, SioPadType type) {
    if (portIndex < 0 || portIndex > 1) return;

    bool needSwap = false;
    if (!m_pad[portIndex]) {
        needSwap = true;
    } else if (type == SioPadType::Analog && !std::dynamic_pointer_cast<PadAnalog>(m_pad[portIndex])) {
        needSwap = true;
    } else if (type == SioPadType::Digital && !std::dynamic_pointer_cast<PadDigital>(m_pad[portIndex])) {
        needSwap = true;
    }

    if (!needSwap) return;

    // End any ongoing transaction cleanly
    if (m_csAsserted && ( (m_CTRL & CTRL_DTR) != 0 ) && m_selectedPort == portIndex) {
        m_pad[portIndex]->endTransaction();
    }

    if (type == SioPadType::Analog) {
        m_pad[portIndex] = std::make_shared<PadAnalog>();
    } else {
        m_pad[portIndex] = std::make_shared<PadDigital>();
    }
}
