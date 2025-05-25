/*
** EPITECH PROJECT, 2025
** rogem
** File description:
** GPU
*/

#ifndef GPU_HPP_
#define GPU_HPP_

#include "PsxDevice.hpp"

class GPU : public PsxDevice
{
    public:
        GPU();
        ~GPU();

        void reset();

        void write8(uint8_t value, uint32_t address) override;
        void write16(uint16_t value, uint32_t address) override;
        void write32(uint32_t value, uint32_t address) override;

        uint8_t read8(uint32_t address) override;
        uint16_t read16(uint32_t address) override;
        uint32_t read32(uint32_t address) override;

    private:
        uint32_t m_statRegister;
        uint32_t m_cmdResponse;
};

#endif /* !GPU_HPP_ */
