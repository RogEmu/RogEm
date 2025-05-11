/*
** EPITECH PROJECT, 2025
** rogem
** File description:
** Memory
*/

#ifndef MEMORY_HPP_
#define MEMORY_HPP_

#include <vector>
#include <cstdint>

class Memory
{
    public:
        Memory(uint32_t size, uint8_t initVal = 0);

        uint32_t loadWord(uint32_t addr) const;
        uint16_t loadHalfWord(uint32_t addr) const;
        uint8_t loadByte(uint32_t addr) const;

        void storeWord(uint32_t addr, uint32_t val);
        void storeHalfWord(uint32_t addr, uint16_t val);
        void storeByte(uint32_t addr, uint8_t val);

        std::vector<uint8_t> *data();
        const std::vector<uint8_t> *data() const;

        void setReadOnly(bool readOnly);
        bool isReadOnly() const;

    protected:
        std::vector<uint8_t> m_data;

    private:
        bool m_readOnly;
};

#endif /* !MEMORY_HPP_ */
