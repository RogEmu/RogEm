/*
** EPITECH PROJECT, 2025
** rogem
** File description:
** MemoryControl1
*/

#ifndef MEMORYCONTROL1_HPP_
#define MEMORYCONTROL1_HPP_

#include "PsxDevice.hpp"

class MemoryControl1 : public PsxDevice
{
    public:
        MemoryControl1(Bus *bus);
        ~MemoryControl1();

        void write8(uint8_t value, uint32_t address) override;
        void write16(uint16_t value, uint32_t address) override;
        void write32(uint32_t value, uint32_t address) override;

        uint8_t read8(uint32_t address) override;
        uint16_t read16(uint32_t address) override;
        uint32_t read32(uint32_t address) override;

    private:
        uint32_t m_registers[9];
};

#endif /* !MEMORYCONTROL1_HPP_ */
