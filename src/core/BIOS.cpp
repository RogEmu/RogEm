/*
** EPITECH PROJECT, 2024
** RogEm
** File description:
** BIOS
*/

#include "BIOS.h"

#include <fstream>
#include <spdlog/spdlog.h>

#include "MemoryMap.hpp"

BIOS::BIOS(Bus *bus) :
    Memory(bus, MemoryMap::BIOS_RANGE.length)
{
    setReadOnly(true);
    m_memoryRange.start = MemoryMap::BIOS_RANGE.start;
    m_memoryRange.length = MemoryMap::BIOS_RANGE.length;
}

BIOS::BIOS(Bus* bus, const std::string &path) :
    BIOS(bus)
{
    loadFromFile(path);
}

bool BIOS::loadFromFile(const std::string &path)
{
    std::ifstream file(path, std::ios_base::in | std::ios_base::binary);

    if (!file.is_open())
    {
        spdlog::error("Cannot open BIOS file \"{}\"", path);
        return false;
    }
    file.read(reinterpret_cast<char*>(m_data.data()), MemoryMap::BIOS_RANGE.length);
    if (file.fail())
    {
        spdlog::error("Cannot read BIOS file \"{}\"", path);
        return false;
    }
    if (file.gcount() != MemoryMap::BIOS_RANGE.length)
    {
        spdlog::error("The provided BIOS file is invalid: Expected size: {} but got {}", MemoryMap::BIOS_RANGE.length, file.gcount());
        return false;
    }
    return true;
}
