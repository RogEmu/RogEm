/*
** EPITECH PROJECT, 2026
** rogem
** File description:
** SIODevice
*/

#ifndef SIODEVICE_HPP_
#define SIODEVICE_HPP_

#include <cstdint>

enum class SIORegister: uint8_t
{
    TX_DATA = 0x0,
    RX_DATA = 0x0,
    STAT    = 0x4,
    MODE    = 0x8,
    CTRL    = 0xA,
    BAUD    = 0xE
};

class SIODevice
{
    public:
        virtual ~SIODevice() = default;

        virtual void write(SIORegister reg, uint16_t value) = 0;
        virtual uint16_t read(SIORegister reg) = 0;
        virtual void update(int cycles) { (void)cycles; };

    protected:
        uint16_t m_stat;
        uint16_t m_mode;
        uint16_t m_ctrl;
        uint16_t m_baud;
        int m_baudTimer;
};

#endif /* !SIODEVICE_HPP_ */
