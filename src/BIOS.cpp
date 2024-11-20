/*
** EPITECH PROJECT, 2024
** RogEm
** File description:
** BIOS
*/

#include "BIOS.h"
#include <fstream>
#include <iostream>
#include <iomanip>

BIOS::BIOS(const std::string &path)
{
    loadFromFile(path);
}

BIOS::~BIOS()
{
}

uint32_t BIOS::loadWord(uint32_t off) const
{
    uint32_t b1 = m_data[off];
    uint32_t b2 = m_data[off + 1] << 8;
    uint32_t b3 = m_data[off + 2] << 16;
    uint32_t b4 = m_data[off + 3] << 24;
    return b4 | b3 | b2 | b1;
}

bool BIOS::loadFromFile(const std::string &path)
{
    std::ifstream file(path, std::ios_base::in | std::ios_base::binary);

    if (!file.is_open())
    {
        std::cerr << "Cannot open \"" << path << "\"" << std::endl;
        return false;
    }
    file.read((char *)m_data.data(), BIOS_SIZE);
    if (file.fail())
    {
        std::cerr << "Cannot read BIOS file \"" << path << std::endl;
        return false;
    }
    if (file.gcount() != BIOS_SIZE)
    {
        std::cerr << "The provided BIOS file is invalid: ";
        std::cerr << "Expected size: " << BIOS_SIZE << " but got : " << file.gcount() << std::endl;
        return false;
    }
    return true;
}

void BIOS::dump(void) const
{
    int count = 0;
    for (const auto& byte : m_data) {
        printf("%02X", byte);
        count++;

        if (count % 16 == 0) {
            printf("\n");
        } else
        {
            printf(" ");
        }
    }
    if (count % 16 != 0) {
        printf("\n");
    }
}
