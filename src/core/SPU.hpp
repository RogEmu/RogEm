/*
** EPITECH PROJECT, 2025
** rogem
** File description:
** SPU
*/

#ifndef SPU_HPP_
#define SPU_HPP_

#include "PsxDevice.hpp"

class SPU : public PsxDevice
{
    public:
        SPU(Bus *bus);
        ~SPU();

        void write8(uint8_t value, uint32_t address) override;
        void write16(uint16_t value, uint32_t address) override;
        void write32(uint32_t value, uint32_t address) override;

        uint8_t read8(uint32_t address) override;
        uint16_t read16(uint32_t address) override;
        uint32_t read32(uint32_t address) override;
};

#endif /* !SPU_HPP_ */
