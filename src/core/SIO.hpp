/*
** EPITECH PROJECT, 2025
** RogEm
** File description:
** SIO header
*/

#pragma once
#include <memory>
#include "PsxDevice.hpp"
#include "InterruptController.hpp"
#include "SIODevice.hpp"
#include "PadDigital.hpp"

// SIO0 (controllers/memcards), maps at 0x1F801040..0x1F80104F

class PadDigital;
class PadAnalog;

enum class SioPadType { 
    Digital, 
    Analog 
};

class SIO : public PsxDevice {

public:
    SIO(Bus* bus, InterruptController& irq);

    // PsxDevice MMIO:
    void write8(uint8_t value, uint32_t address) override;
    void write16(uint16_t value, uint32_t address) override;
    void write32(uint32_t value, uint32_t address) override;
    uint8_t read8(uint32_t address) override;
    uint16_t read16(uint32_t address) override;
    uint32_t read32(uint32_t address) override;

    // Route pads:
    void attachController(int portIndex, std::shared_ptr<SIODevice> dev);

    void setControllerButtons(int portIndex, uint16_t activeLowMask);
    void setControllerAnalog(int portIndex, uint16_t activeLowMask,
                             uint8_t lx, uint8_t ly, uint8_t rx, uint8_t ry);
    void ensurePadType(int portIndex, SioPadType type);

private:
    // Registers (subset)
    uint8_t  m_TX = 0;
    uint8_t  m_RX = 0;
    uint16_t m_STAT = 0;
    uint16_t m_MODE = 0;
    uint16_t m_CTRL = 0;
    uint16_t m_BAUD = 0;

    // STAT/CTRL bits we actually use
    static constexpr uint16_t STAT_RX_RDY   = 1 << 0;
    static constexpr uint16_t STAT_TX_RDY   = 1 << 1;
    static constexpr uint16_t STAT_TX_EMPTY = 1 << 2;
    static constexpr uint16_t STAT_DSR      = 1 << 7;  // device /ACK shown here

    static constexpr uint16_t CTRL_TX_EN    = 1 << 0;
    static constexpr uint16_t CTRL_DTR      = 1 << 1;  // /CS
    static constexpr uint16_t CTRL_RX_IRQ_EN= 1 << 10; // enable IRQ on /ACK
    static constexpr uint16_t CTRL_PORT_SEL = 1 << 13; // 0=port1, 1=port2
    static constexpr int IRQ_LINE = 7;

    InterruptController& m_irq;
    std::shared_ptr<SIODevice> m_pad[2];

    bool    m_csAsserted = false;
    int     m_selectedPort = 0;

    // helpers
    void setDSR(bool activeLow);
    void raiseIRQIfEnabled();

    // unified byte TX path
    void txByte(uint8_t value);
};
