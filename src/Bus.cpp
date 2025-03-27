/*
** EPITECH PROJECT, 2024
** RogEm
** File description:
** Bus
*/

#include "Bus.h"

#include <fmt/format.h>

#include "BIOS.h"
#include "RAM.h"

// When adding address spaces, use physical addresses
const MemRange BIOS_RANGE = {0x1FC00000, 512 * 1024};
const MemRange RAM_RANGE = {0x0, 2 * 1024 * 1024};
const MemRange MEMORY_CONTROL_1_RANGE = {0x1F801000, 36};
const MemRange MEMORY_CONTROL_2_RANGE = {0x1F801060, 4};
const MemRange CACHE_CONTROL_RANGE = {0xFFFE0000, 512};
const MemRange SPU_CONTROL_REGS_RANGE = {0x1F801D80, 64};
const MemRange EXP_REG_2_RANGE = {0x1F802000, 1024 * 8};

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

Bus::Bus(BIOS* bios, RAM *ram) :
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
        fmt::println(stderr, "Unaligned LW instruction\n");
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
    if (CACHE_CONTROL_RANGE.contains(pAddress))
    {
        fmt::println(stderr, "Read word from Cache Control Registers (0x{:08x}): Not implemented", addr);
        return 0;
    }
    if (SPU_CONTROL_REGS_RANGE.contains(pAddress))
    {
        fmt::println(stderr, "Read word from SPU Control Registers (0x{:08x}): Not implemented", addr);
        return 0;
    }
    if (EXP_REG_2_RANGE.contains(pAddress))
    {
        fmt::println(stderr, "Read word from Expansion Region 2 (0x{:08x}): Not implemented", addr);
        return 0;
    }
    return 0;
}

void Bus::storeWord(uint32_t addr, uint32_t value)
{
    uint32_t pAddress = mapAddress(addr);

    if (addr % 4 != 0)
    {
        fmt::println(stderr, "Unaligned SW instruction");
        return;
    }

    if (RAM_RANGE.contains(pAddress))
    {
        m_ram->storeWord(pAddress, value);
    }
    else if (MEMORY_CONTROL_1_RANGE.contains(pAddress))
    {
        fmt::println(stderr, "Write word 0x{:08X} to Memory Control 1 (0x{:08X}): Not supported", value, addr);
    }
    else if (MEMORY_CONTROL_2_RANGE.contains(pAddress))
    {
        fmt::println(stderr, "Write word 0x{:08X} to Memory Control 2 (0x{:08X}): Not supported", value, addr);
    }
    else if (CACHE_CONTROL_RANGE.contains(pAddress))
    {
        fmt::println(stderr, "Write word 0x{:08X} to Cache Control Registers (0x{:08X}): Not implemented", value, addr);
    }
    else if (SPU_CONTROL_REGS_RANGE.contains(pAddress))
    {
        fmt::println(stderr, "Write word 0x{:08X} to SPU Control Registers (0x{:08X}): Not implemented", value, addr);
    }
    else if (EXP_REG_2_RANGE.contains(pAddress))
    {
        fmt::println(stderr, "Write word 0x{:08X} to Expansion Region 2 (0x{:08X}): Not implemented", value, addr);
    }
    else
    {
        fmt::println(stderr, "Address 0x{:08X} is not supported", addr);
    }
}

uint16_t Bus::loadHalfWord(uint32_t addr) const
{
    uint32_t pAddress = mapAddress(addr);

    if (addr % 2 != 0)
    {
        fmt::println(stderr, "Unaligned LH instruction");
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
    if (CACHE_CONTROL_RANGE.contains(pAddress))
    {
        fmt::println(stderr, "Read halfword from Cache Control Registers (0x{:08x}): Not implemented", addr);
        return 0;
    }
    if (SPU_CONTROL_REGS_RANGE.contains(pAddress))
    {
        fmt::println(stderr, "Read halfword from SPU Control Registers (0x{:08x}): Not implemented", addr);
        return 0;
    }
    if (EXP_REG_2_RANGE.contains(pAddress))
    {
        fmt::println(stderr, "Read halfword from Expansion Region 2 (0x{:08x}): Not implemented", addr);
        return 0;
    }
    return 0;
}

void Bus::storeHalfWord(uint32_t addr, uint16_t value)
{
    uint32_t pAddress = mapAddress(addr);

    if (addr % 2 != 0)
    {
        fmt::println(stderr, "Unaligned SH instruction");
        return;
    }
    if (RAM_RANGE.contains(pAddress))
    {
        m_ram->storeHalfWord(pAddress, value);
    }
    else if (MEMORY_CONTROL_1_RANGE.contains(pAddress))
    {
        fmt::println(stderr, "Write halfword 0x{:04X} to Memory Control 1 (0x{:08X}): Not supported", value, addr);
    }
    else if (MEMORY_CONTROL_2_RANGE.contains(pAddress))
    {
        fmt::println(stderr, "Write halfword 0x{:04X} to Memory Control 2 (0x{:08X}): Not supported", value, addr);
    }
    else if (CACHE_CONTROL_RANGE.contains(pAddress))
    {
        fmt::println(stderr, "Write halfword 0x{:04X} to Cache Control Registers (0x{:08X}): Not implemented", value, addr);
    }
    else if (SPU_CONTROL_REGS_RANGE.contains(pAddress))
    {
        fmt::println(stderr, "Write halfword 0x{:04X} to SPU Control Registers (0x{:08X}): Not implemented", value, addr);
    }
    else if (EXP_REG_2_RANGE.contains(pAddress))
    {
        fmt::println(stderr, "Write halfword 0x{:04X} to Expansion Region 2 (0x{:08X}): Not implemented", value, addr);
    }
    else
    {
        fmt::println(stderr, "Address 0x{:08X} is not supported", addr);
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
    if (CACHE_CONTROL_RANGE.contains(pAddress))
    {
        fmt::println(stderr, "Read byte from Cache Control Registers (0x{:08x}): Not implemented", addr);
        return 0;
    }
    if (SPU_CONTROL_REGS_RANGE.contains(pAddress))
    {
        fmt::println(stderr, "Read byte from SPU Control Registers (0x{:08x}): Not implemented", addr);
        return 0;
    }
    if (EXP_REG_2_RANGE.contains(pAddress))
    {
        fmt::println(stderr, "Read byte from Expansion Region 2 (0x{:08x}): Not implemented", addr);
        return 0;
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
    else if (MEMORY_CONTROL_1_RANGE.contains(pAddress))
    {
        fmt::println(stderr, "Write byte 0x{:02x} to Memory Control 1 (0x{:08X}): Not supported", value, addr);
    }
    else if (MEMORY_CONTROL_2_RANGE.contains(pAddress))
    {
        fmt::println(stderr, "Write byte 0x{:02x} to Memory Control 2 (0x{:08X}): Not supported", value, addr);
    }
    else if (CACHE_CONTROL_RANGE.contains(pAddress))
    {
        fmt::println(stderr, "Write byte 0x{:02X} to Cache Control Registers (0x{:08X}): Not implemented", value, addr);
    }
    else if (SPU_CONTROL_REGS_RANGE.contains(pAddress))
    {
        fmt::println(stderr, "Write byte 0x{:02X} to SPU Control Registers (0x{:08X}): Not implemented", value, addr);
    }
    else if (EXP_REG_2_RANGE.contains(pAddress))
    {
        fmt::println(stderr, "Write byte 0x{:02X} to Expansion Region 2 (0x{:08X}): Not implemented", value, addr);
    }
    else
    {
        fmt::println(stderr, "Address 0x{:08X} is not supported", addr);
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
            fmt::println(stderr, "Unsupported address space: {:08X}", addr);
            break;
    }
    return 0;
}
