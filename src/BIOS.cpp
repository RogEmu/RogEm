/*
** EPITECH PROJECT, 2024
** RogEm
** File description:
** BIOS
*/

#include "BIOS.h"

#include <fstream>
#include <fmt/format.h>

BIOS::BIOS() :
    Memory(BIOS_SIZE)
{
    setReadOnly(true);
}

BIOS::BIOS(const std::string &path) :
    BIOS()
{
    loadFromFile(path);
}

bool BIOS::loadFromFile(const std::string &path)
{
    std::ifstream file(path, std::ios_base::in | std::ios_base::binary);

    if (!file.is_open())
    {
        fmt::println(stderr, "Cannot open BIOS file \"{}\"", path);
        return false;
    }
    file.read(reinterpret_cast<char*>(m_data.data()), BIOS_SIZE);
    if (file.fail())
    {
        fmt::println(stderr, "Cannot read BIOS file \"{}\"", path);
        return false;
    }
    if (file.gcount() != BIOS_SIZE)
    {
        fmt::println(stderr, "The provided BIOS file is invalid: Expected size: {} but got {}", BIOS_SIZE, file.gcount());
        return false;
    }
    return true;
}
