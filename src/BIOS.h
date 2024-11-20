/*
** EPITECH PROJECT, 2024
** RogEm
** File description:
** BIOS
*/

#ifndef BIOS_H_
#define BIOS_H_

#include <array>
#include <string>
#include <cstdint>

#define BIOS_SIZE 512 * 1024

class BIOS
{
    public:
        BIOS(const std::string &path);
        ~BIOS();

        uint32_t loadWord(uint32_t off) const;
        bool loadFromFile(const std::string &path);
        void dump(void) const;

    private:
        std::array<uint8_t, BIOS_SIZE> m_data;
};

#endif /* !BIOS_H_ */
