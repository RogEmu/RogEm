/*
** EPITECH PROJECT, 2024
** RogEm
** File description:
** Bus
*/

#include "Bus.h"

#include "BIOS.h"
#include "RAM.h"

// When adding address spaces, use physical addresses
const MemRange BIOS_RANGE = {0x1FC00000, 512 * 1024};
const MemRange RAM_RANGE = {0x0, 2 * 1024 * 1024};
const MemRange MEMORY_CONTROL_RANGE = {0x1F801000, 36};

const MemorySegments AddressSegments[] = {
    MemorySegments::KUSEG,
    MemorySegments::KUSEG,
    MemorySegments::KUSEG,
    MemorySegments::KUSEG,
    MemorySegments::KSEG0,
    MemorySegments::KSEG1,
    MemorySegments::KSEG2,
    MemorySegments::KSEG2,
};

Bus::Bus(const std::shared_ptr<BIOS> &bios, const std::shared_ptr<RAM> &ram) :
    m_bios(bios),
    m_ram(ram)
{
}

Bus::~Bus()
{
}

uint32_t Bus::loadWord(uint32_t addr) const
{
    uint32_t pAddress = mapAddress(addr);

    if (addr % 4 != 0)
    {
        fprintf(stderr, "Unaligned LW instruction\n");
        return 0;
    }

    if (BIOS_RANGE.contains(pAddress))
    {
        return m_bios->loadWord(BIOS_RANGE.remap(pAddress));
    }
    if (RAM_RANGE.contains(pAddress))
    {
        return m_ram->loadWord(RAM_RANGE.remap(pAddress));
    }
    return 0;
}

void Bus::storeWord(uint32_t addr, uint32_t value)
{
    uint32_t pAddress = mapAddress(addr);

    if (addr % 4 != 0)
    {
        fprintf(stderr, "Unaligned SW instruction\n");
        return;
    }

    if (RAM_RANGE.contains(pAddress))
    {
        m_ram->storeWord(pAddress, value);
    }
    else if (MEMORY_CONTROL_RANGE.contains(pAddress))
    {
        fprintf(stderr, "Trying to write word 0x%08x into Memory Control: Not supported yet\n", value);
    }
    else
    {
        fprintf(stderr, "Address 0x%08X is not supported\n", addr);
    }
}

uint16_t Bus::loadHalfWord(uint32_t addr) const
{
    uint32_t pAddress = mapAddress(addr);

    if (addr % 2 != 0)
    {
        fprintf(stderr, "Unaligned LH instruction\n");
        return 0;
    }

    if (BIOS_RANGE.contains(pAddress))
    {
        return m_bios->loadHalfWord(BIOS_RANGE.remap(pAddress));
    }
    else if (RAM_RANGE.contains(pAddress))
    {
        return m_ram->loadHalfWord(RAM_RANGE.remap(pAddress));
    }
    return 0;
}

void Bus::storeHalfWord(uint32_t addr, uint16_t value)
{
    uint32_t pAddress = mapAddress(addr);

    if (addr % 2 != 0)
    {
        fprintf(stderr, "Unaligned SH instruction\n");
        return;
    }
    if (RAM_RANGE.contains(pAddress))
    {
        m_ram->storeHalfWord(pAddress, value);
    }
    else if (MEMORY_CONTROL_RANGE.contains(pAddress))
    {
        fprintf(stderr, "Trying to write halfword 0x%04x into Memory Control: Not supported yet\n", value);
    }
    else
    {
        fprintf(stderr, "Address 0x%08X is not supported\n", addr);
    }
}

uint8_t Bus::loadByte(uint32_t addr) const
{
    uint32_t pAddress = mapAddress(addr);

    if (BIOS_RANGE.contains(pAddress))
    {
        return m_bios->loadByte(BIOS_RANGE.remap(pAddress));
    }
    if (RAM_RANGE.contains(pAddress))
    {
        return m_ram->loadByte(RAM_RANGE.remap(pAddress));
    }
    return 0;
}

void Bus::storeByte(uint32_t addr, uint8_t value)
{
    uint32_t pAddress = mapAddress(addr);

    if (RAM_RANGE.contains(pAddress))
    {
        m_ram->storeByte(pAddress, value);
    }
    else if (MEMORY_CONTROL_RANGE.contains(pAddress))
    {
        fprintf(stderr, "Trying to write byte 0x%02x into Memory Control: Not supported yet\n", value);
    }
    else
    {
        fprintf(stderr, "Address 0x%08X is not supported\n", addr);
    }
}

uint32_t Bus::mapAddress(uint32_t addr) const
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
        default:
            printf("Unsupported address space: %08x\n", addr);
            break;
    }
    return 0;
}
