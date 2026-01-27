/*
** EPITECH PROJECT, 2025
** rogem
** File description:
** SerialInterface
*/

#ifndef SERIALINTERFACE_HPP_
#define SERIALINTERFACE_HPP_

#include "PsxDevice.hpp"

enum class SIORegister: uint32_t {
    TX_DATA = 0x0,
    STAT    = 0x4,
    MODE    = 0x8,
    CTRL    = 0xA,
    BAUD    = 0xE
};

/**
 * @brief Serial Interface (SIO) - Handles controller and memory card communication
 *
 * The PS1 Serial Interface provides communication with:
 * - Controllers (port 1 and 2)
 * - Memory cards (port 1 and 2)
 *
 * Register map (0x1F801040-0x1F80104F):
 * 0x1F801040: TX_DATA (R/W) - Transmit/Receive data
 * 0x1F801044: STAT (R) - Status register
 * 0x1F801048: MODE (R/W) - Mode register
 * 0x1F80104A: CTRL (R/W) - Control register
 * 0x1F80104E: BAUD (R/W) - Baud rate register
 */
class SerialInterface : public PsxDevice
{
    public:
        SerialInterface(Bus *bus);
        ~SerialInterface();

        void write8(uint8_t value, uint32_t address) override;
        void write16(uint16_t value, uint32_t address) override;
        void write32(uint32_t value, uint32_t address) override;

        uint8_t read8(uint32_t address) override;
        uint16_t read16(uint32_t address) override;
        uint32_t read32(uint32_t address) override;
};

#endif /* !SERIALINTERFACE_HPP_ */
