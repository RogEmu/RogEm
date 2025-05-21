/*
** EPITECH PROJECT, 2025
** rogem
** File description:
** DMA
*/

#ifndef DMA_HPP_
#define DMA_HPP_

#include "PsxDevice.hpp"

class DMA : public PsxDevice
{
    public:
        DMA();
        ~DMA();

        void write8(uint8_t value, uint32_t address) override;
        void write16(uint16_t value, uint32_t address) override;
        void write32(uint32_t value, uint32_t address) override;

        uint8_t read8(uint32_t address) override;
        uint16_t read16(uint32_t address) override;
        uint32_t read32(uint32_t address) override;
};

#endif /* !DMA_HPP_ */
