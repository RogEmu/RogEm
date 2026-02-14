/*
** EPITECH PROJECT, 2025
** rogem
** File description:
** MemoryMap
*/

#include "MemoryMap.hpp"

#include <spdlog/spdlog.h>

uint32_t MemoryMap::mapAddress(uint32_t addr)
{
    auto segment = AddressSegments[static_cast<uint8_t>(addr >> 29)];

    switch (segment)
    {
        case MemorySegments::KUSEG:
            return addr;
        case MemorySegments::KSEG0:
        case MemorySegments::KSEG1:
            return addr & 0x1FFFFFFF;
        case MemorySegments::KSEG2:
            return addr;
    }
    return 0;
}
