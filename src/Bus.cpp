/*
** EPITECH PROJECT, 2024
** RogEm
** File description:
** Bus
*/

#include "Bus.h"

#include <fmt/format.h>

#include "MemoryMap.hpp"
#include "BIOS.h"
#include "RAM.h"

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
    uint32_t pAddress = MemoryMap::mapAddress(addr);

    if (addr % 4 != 0)
    {
        fmt::println(stderr, "Unaligned LW instruction\n");
        return 0;
    }

    if (MemoryMap::BIOS_RANGE.contains(pAddress))
    {
        return m_bios->loadWord(MemoryMap::BIOS_RANGE.remap(pAddress));
    }
    if (MemoryMap::RAM_RANGE.contains(pAddress))
    {
        return m_ram->loadWord(MemoryMap::RAM_RANGE.remap(pAddress));
    }
    if (MemoryMap::CACHE_CONTROL_RANGE.contains(pAddress))
    {
        fmt::println(stderr, "Read word from Cache Control Registers (0x{:08x}): Not implemented", addr);
        return 0;
    }
    if (MemoryMap::SPU_CONTROL_REGS_RANGE.contains(pAddress))
    {
        fmt::println(stderr, "Read word from SPU Control Registers (0x{:08x}): Not implemented", addr);
        return 0;
    }
    if (MemoryMap::EXP_REG_2_RANGE.contains(pAddress))
    {
        fmt::println(stderr, "Read word from Expansion Region 2 (0x{:08x}): Not implemented", addr);
        return 0;
    }
    return 0;
}

void Bus::storeWord(uint32_t addr, uint32_t value)
{
    uint32_t pAddress = MemoryMap::mapAddress(addr);

    if (addr % 4 != 0)
    {
        fmt::println(stderr, "Unaligned SW instruction");
        return;
    }

    if (MemoryMap::RAM_RANGE.contains(pAddress))
    {
        m_ram->storeWord(pAddress, value);
    }
    else if (MemoryMap::MEMORY_CONTROL_1_RANGE.contains(pAddress))
    {
        fmt::println(stderr, "Write word 0x{:08X} to Memory Control 1 (0x{:08X}): Not supported", value, addr);
    }
    else if (MemoryMap::MEMORY_CONTROL_2_RANGE.contains(pAddress))
    {
        fmt::println(stderr, "Write word 0x{:08X} to Memory Control 2 (0x{:08X}): Not supported", value, addr);
    }
    else if (MemoryMap::CACHE_CONTROL_RANGE.contains(pAddress))
    {
        fmt::println(stderr, "Write word 0x{:08X} to Cache Control Registers (0x{:08X}): Not implemented", value, addr);
    }
    else if (MemoryMap::SPU_CONTROL_REGS_RANGE.contains(pAddress))
    {
        fmt::println(stderr, "Write word 0x{:08X} to SPU Control Registers (0x{:08X}): Not implemented", value, addr);
    }
    else if (MemoryMap::EXP_REG_2_RANGE.contains(pAddress))
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
    uint32_t pAddress = MemoryMap::mapAddress(addr);

    if (addr % 2 != 0)
    {
        fmt::println(stderr, "Unaligned LH instruction");
        return 0;
    }

    if (MemoryMap::BIOS_RANGE.contains(pAddress))
    {
        return m_bios->loadHalfWord(MemoryMap::BIOS_RANGE.remap(pAddress));
    }
    else if (MemoryMap::RAM_RANGE.contains(pAddress))
    {
        return m_ram->loadHalfWord(MemoryMap::RAM_RANGE.remap(pAddress));
    }
    if (MemoryMap::CACHE_CONTROL_RANGE.contains(pAddress))
    {
        fmt::println(stderr, "Read halfword from Cache Control Registers (0x{:08x}): Not implemented", addr);
        return 0;
    }
    if (MemoryMap::SPU_CONTROL_REGS_RANGE.contains(pAddress))
    {
        fmt::println(stderr, "Read halfword from SPU Control Registers (0x{:08x}): Not implemented", addr);
        return 0;
    }
    if (MemoryMap::EXP_REG_2_RANGE.contains(pAddress))
    {
        fmt::println(stderr, "Read halfword from Expansion Region 2 (0x{:08x}): Not implemented", addr);
        return 0;
    }
    return 0;
}

void Bus::storeHalfWord(uint32_t addr, uint16_t value)
{
    uint32_t pAddress = MemoryMap::mapAddress(addr);

    if (addr % 2 != 0)
    {
        fmt::println(stderr, "Unaligned SH instruction");
        return;
    }
    if (MemoryMap::RAM_RANGE.contains(pAddress))
    {
        m_ram->storeHalfWord(pAddress, value);
    }
    else if (MemoryMap::MEMORY_CONTROL_1_RANGE.contains(pAddress))
    {
        fmt::println(stderr, "Write halfword 0x{:04X} to Memory Control 1 (0x{:08X}): Not supported", value, addr);
    }
    else if (MemoryMap::MEMORY_CONTROL_2_RANGE.contains(pAddress))
    {
        fmt::println(stderr, "Write halfword 0x{:04X} to Memory Control 2 (0x{:08X}): Not supported", value, addr);
    }
    else if (MemoryMap::CACHE_CONTROL_RANGE.contains(pAddress))
    {
        fmt::println(stderr, "Write halfword 0x{:04X} to Cache Control Registers (0x{:08X}): Not implemented", value, addr);
    }
    else if (MemoryMap::SPU_CONTROL_REGS_RANGE.contains(pAddress))
    {
        fmt::println(stderr, "Write halfword 0x{:04X} to SPU Control Registers (0x{:08X}): Not implemented", value, addr);
    }
    else if (MemoryMap::EXP_REG_2_RANGE.contains(pAddress))
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
    uint32_t pAddress = MemoryMap::mapAddress(addr);

    if (MemoryMap::BIOS_RANGE.contains(pAddress))
    {
        return m_bios->loadByte(MemoryMap::BIOS_RANGE.remap(pAddress));
    }
    if (MemoryMap::RAM_RANGE.contains(pAddress))
    {
        return m_ram->loadByte(MemoryMap::RAM_RANGE.remap(pAddress));
    }
    if (MemoryMap::CACHE_CONTROL_RANGE.contains(pAddress))
    {
        fmt::println(stderr, "Read byte from Cache Control Registers (0x{:08x}): Not implemented", addr);
        return 0;
    }
    if (MemoryMap::SPU_CONTROL_REGS_RANGE.contains(pAddress))
    {
        fmt::println(stderr, "Read byte from SPU Control Registers (0x{:08x}): Not implemented", addr);
        return 0;
    }
    if (MemoryMap::EXP_REG_2_RANGE.contains(pAddress))
    {
        fmt::println(stderr, "Read byte from Expansion Region 2 (0x{:08x}): Not implemented", addr);
        return 0;
    }
    return 0;
}

void Bus::storeByte(uint32_t addr, uint8_t value)
{
    uint32_t pAddress = MemoryMap::mapAddress(addr);

    if (MemoryMap::RAM_RANGE.contains(pAddress))
    {
        m_ram->storeByte(pAddress, value);
    }
    else if (MemoryMap::MEMORY_CONTROL_1_RANGE.contains(pAddress))
    {
        fmt::println(stderr, "Write byte 0x{:02x} to Memory Control 1 (0x{:08X}): Not supported", value, addr);
    }
    else if (MemoryMap::MEMORY_CONTROL_2_RANGE.contains(pAddress))
    {
        fmt::println(stderr, "Write byte 0x{:02x} to Memory Control 2 (0x{:08X}): Not supported", value, addr);
    }
    else if (MemoryMap::CACHE_CONTROL_RANGE.contains(pAddress))
    {
        fmt::println(stderr, "Write byte 0x{:02X} to Cache Control Registers (0x{:08X}): Not implemented", value, addr);
    }
    else if (MemoryMap::SPU_CONTROL_REGS_RANGE.contains(pAddress))
    {
        fmt::println(stderr, "Write byte 0x{:02X} to SPU Control Registers (0x{:08X}): Not implemented", value, addr);
    }
    else if (MemoryMap::EXP_REG_2_RANGE.contains(pAddress))
    {
        fmt::println(stderr, "Write byte 0x{:02X} to Expansion Region 2 (0x{:08X}): Not implemented", value, addr);
    }
    else
    {
        fmt::println(stderr, "Address 0x{:08X} is not supported", addr);
    }
}

std::vector<uint8_t> *Bus::getMemoryRange(uint32_t addr)
{
    auto mappedAddress = MemoryMap::mapAddress(addr);

    if (MemoryMap::BIOS_RANGE.contains(mappedAddress))
        return m_bios->data();
    if (MemoryMap::RAM_RANGE.contains(mappedAddress))
        return m_ram->data();
    return nullptr;
}

const std::vector<uint8_t> *Bus::getMemoryRange(uint32_t addr) const
{
    auto mappedAddress = MemoryMap::mapAddress(addr);

    if (MemoryMap::BIOS_RANGE.contains(mappedAddress))
        return m_bios->data();
    if (MemoryMap::RAM_RANGE.contains(mappedAddress))
        return m_ram->data();
    return nullptr;
}
