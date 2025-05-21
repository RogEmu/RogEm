/*
** EPITECH PROJECT, 2025
** rogem
** File description:
** Memory
*/

#ifndef MEMORY_HPP_
#define MEMORY_HPP_

#include <vector>

#include "PsxDevice.hpp"

class Memory : public PsxDevice
{
    public:
        Memory(uint32_t size, uint8_t initVal = 0);

        uint32_t read32(uint32_t addr) override;
        uint16_t read16(uint32_t addr) override;
        uint8_t read8(uint32_t addr) override;

        void write32(uint32_t val, uint32_t addr) override;
        void write16(uint16_t val, uint32_t addr) override;
        void write8(uint8_t val, uint32_t addr) override;

        std::vector<uint8_t> *data();
        const std::vector<uint8_t> *data() const;

        void setReadOnly(bool readOnly);
        bool isReadOnly() const;

    protected:
        std::vector<uint8_t> m_data;
        bool m_readOnly;
};

#endif /* !MEMORY_HPP_ */
