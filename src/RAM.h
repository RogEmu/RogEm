/*
** EPITECH PROJECT, 2024
** rogem
** File description:
** RAM
*/

#ifndef RAM_H_
#define RAM_H_

#include <vector>
#include <cstdint>
#include <string>

#define RAM_SIZE (2048 * 1024)

class RAM
{
    public:
        RAM();

        uint32_t loadWord(uint32_t addr) const;
        uint16_t loadHalfWord(uint32_t addr) const;
        uint8_t loadByte(uint32_t addr) const;

        void storeWord(uint32_t addr, uint32_t val);
        void storeHalfWord(uint32_t addr, uint16_t val);
        void storeByte(uint32_t addr, uint8_t val);

        std::vector<uint8_t> *data();
        const std::vector<uint8_t> *data() const;

    private:
        std::vector<uint8_t> m_data;
};

#endif /* !RAM_H_ */
