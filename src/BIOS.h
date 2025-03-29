/*
** EPITECH PROJECT, 2024
** RogEm
** File description:
** BIOS
*/

#ifndef BIOS_H_
#define BIOS_H_

#include <string>
#include <cstdint>
#include <vector>

#define BIOS_SIZE (512 * 1024)

class BIOS
{
    public:
        BIOS();
        BIOS(const std::string &path);
        ~BIOS();

        uint32_t loadWord(uint32_t off) const;
        uint16_t loadHalfWord(uint32_t off) const;
        uint8_t loadByte(uint32_t off) const;

        bool loadFromFile(const std::string &path);

        std::vector<uint8_t> *data();
        const std::vector<uint8_t> *data() const;

    private:
        std::vector<uint8_t> m_data;
};

#endif /* !BIOS_H_ */
