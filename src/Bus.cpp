/*
** EPITECH PROJECT, 2024
** RogEm
** File description:
** Bus
*/

#include "Bus.h"

const MemRange BIOS_RANGE = {0xBFC00000, 512 * 1024};
const MemRange RAM_RANGE = {0x0, 2 * 1024 * 1024};

Bus::Bus(const BIOS &bios) :
    m_bios(bios)
{
}

Bus::~Bus()
{
}

uint32_t Bus::loadWord(uint32_t addr) const
{
    if (addr % 4 != 0)
    {
        fprintf(stderr, "Unaligned LW instruction\n");
        return 0;
    }
    if (BIOS_RANGE.contains(addr))
    {
        return m_bios.loadWord(BIOS_RANGE.remap(addr));
    }
    return 0;
}

void Bus::storeWord(uint32_t addr, uint32_t value)
{
    if (addr % 4 != 0)
    {
        fprintf(stderr, "Unaligned SW instruction\n");
        return;
    }
    (void) value;
    fprintf(stderr, "Address: %04X is not supported\n", addr);
}
