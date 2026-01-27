/*
** EPITECH PROJECT, 2025
** rogem
** File description:
** SerialInterface
*/

#ifndef SERIALINTERFACE_HPP_
#define SERIALINTERFACE_HPP_

#include "PsxDevice.hpp"

#include "SIO0.hpp"

class SerialInterface : public PsxDevice
{
    public:
        SerialInterface(Bus *bus);
        ~SerialInterface();

        void update(int cycles) override;
        void reset() override;
        DigitalPad &getPad(int index) { return m_sio0.getPad(index); }

        void write8(uint8_t value, uint32_t address) override;
        void write16(uint16_t value, uint32_t address) override;
        void write32(uint32_t value, uint32_t address) override;

        uint8_t read8(uint32_t address) override;
        uint16_t read16(uint32_t address) override;
        uint32_t read32(uint32_t address) override;

    private:
        void writeSIO1(uint16_t value, uint32_t address);
        uint16_t readSIO1(uint32_t address);

    private:
        SIO0 m_sio0;
};

#endif /* !SERIALINTERFACE_HPP_ */
