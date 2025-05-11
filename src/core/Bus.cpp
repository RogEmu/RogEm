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
    m_ram(ram),
    m_ioPorts(nullptr),
    m_cacheControl(0)
{
    m_ioPorts = std::make_unique<Memory>(MemoryMap::IO_PORTS_RANGE.length);
    m_scratchpad = std::make_unique<Memory>(MemoryMap::SCRATCHPAD_RANGE.length);
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

    if (addr == 0x1F801814) {
        return 0xFFFFFFFF;
    }

    if (MemoryMap::BIOS_RANGE.contains(pAddress))
    {
        return m_bios->loadWord(MemoryMap::BIOS_RANGE.remap(pAddress));
    }
    if (MemoryMap::RAM_RANGE.contains(pAddress))
    {
        return m_ram->loadWord(MemoryMap::RAM_RANGE.remap(pAddress));
    }
    if (MemoryMap::IO_PORTS_RANGE.contains(pAddress))
    {
        return m_ioPorts->loadWord(MemoryMap::IO_PORTS_RANGE.remap(pAddress));
    }
    if (MemoryMap::CACHE_CONTROL_RANGE.contains(pAddress))
    {
        return m_cacheControl;
    }
    if (MemoryMap::EXP2_RANGE.contains(pAddress))
    {
        fmt::println(stderr, "Read word from Expansion Region 2 (0x{:08x}): Not implemented", addr);
        return 0;
    }
    if (MemoryMap::SCRATCHPAD_RANGE.contains(pAddress))
    {
        return m_scratchpad->loadWord(MemoryMap::SCRATCHPAD_RANGE.remap(pAddress));
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
    else if (MemoryMap::IO_PORTS_RANGE.contains(pAddress))
    {
        m_ioPorts->storeWord(MemoryMap::IO_PORTS_RANGE.remap(pAddress), value);
    }
    else if (MemoryMap::CACHE_CONTROL_RANGE.contains(pAddress))
    {
        m_cacheControl = value;
    }
    else if (MemoryMap::EXP2_RANGE.contains(pAddress))
    {
        fmt::println(stderr, "Write word 0x{:08X} to Expansion Region 2 (0x{:08X}): Not implemented", value, addr);
    }
    else if (MemoryMap::SCRATCHPAD_RANGE.contains(pAddress))
    {
        m_scratchpad->storeWord(MemoryMap::SCRATCHPAD_RANGE.remap(pAddress), value);
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
    if (MemoryMap::RAM_RANGE.contains(pAddress))
    {
        return m_ram->loadHalfWord(MemoryMap::RAM_RANGE.remap(pAddress));
    }
    if (MemoryMap::IO_PORTS_RANGE.contains(pAddress))
    {
        return m_ioPorts->loadHalfWord(MemoryMap::IO_PORTS_RANGE.remap(pAddress));
    }
    if (MemoryMap::CACHE_CONTROL_RANGE.contains(pAddress))
    {
        fmt::println(stderr, "Read halfword from Cache Control Registers (0x{:08x}): Not implemented", addr);
        return 0;
    }
    if (MemoryMap::EXP2_RANGE.contains(pAddress))
    {
        fmt::println(stderr, "Read halfword from Expansion Region 2 (0x{:08x}): Not implemented", addr);
        return 0;
    }
    if (MemoryMap::SCRATCHPAD_RANGE.contains(pAddress))
    {
        return m_scratchpad->loadHalfWord(MemoryMap::SCRATCHPAD_RANGE.remap(pAddress));
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
    else if (MemoryMap::IO_PORTS_RANGE.contains(pAddress))
    {
        m_ioPorts->storeHalfWord(MemoryMap::IO_PORTS_RANGE.remap(pAddress), value);
    }
    else if (MemoryMap::CACHE_CONTROL_RANGE.contains(pAddress))
    {
        fmt::println(stderr, "Write halfword 0x{:04X} to Cache Control Registers (0x{:08X}): Not implemented", value, addr);
    }
    else if (MemoryMap::EXP2_RANGE.contains(pAddress))
    {
        fmt::println(stderr, "Write halfword 0x{:04X} to Expansion Region 2 (0x{:08X}): Not implemented", value, addr);
    }
    else if (MemoryMap::SCRATCHPAD_RANGE.contains(pAddress))
    {
        m_scratchpad->storeHalfWord(MemoryMap::SCRATCHPAD_RANGE.remap(pAddress), value);
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
    if (MemoryMap::IO_PORTS_RANGE.contains(pAddress))
    {
        return m_ioPorts->loadByte(MemoryMap::IO_PORTS_RANGE.remap(pAddress));
    }
    if (MemoryMap::CACHE_CONTROL_RANGE.contains(pAddress))
    {
        fmt::println(stderr, "Read byte from Cache Control Registers (0x{:08x}): Not implemented", addr);
        return 0;
    }
    if (MemoryMap::EXP2_RANGE.contains(pAddress))
    {
        fmt::println(stderr, "Read byte from Expansion Region 2 (0x{:08x}): Not implemented", addr);
        return 0;
    }
    if (MemoryMap::SCRATCHPAD_RANGE.contains(pAddress))
    {
        return m_scratchpad->loadByte(MemoryMap::SCRATCHPAD_RANGE.remap(pAddress));
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
    else if (MemoryMap::IO_PORTS_RANGE.contains(pAddress))
    {
        m_ioPorts->storeByte(MemoryMap::IO_PORTS_RANGE.remap(pAddress), value);
    }
    else if (MemoryMap::CACHE_CONTROL_RANGE.contains(pAddress))
    {
        fmt::println(stderr, "Write byte 0x{:02X} to Cache Control Registers (0x{:08X}): Not implemented", value, addr);
    }
    else if (MemoryMap::EXP2_RANGE.contains(pAddress))
    {
        fmt::println(stderr, "Write byte 0x{:02X} to Expansion Region 2 (0x{:08X}): Not implemented", value, addr);
    }
    else if (MemoryMap::SCRATCHPAD_RANGE.contains(pAddress))
    {
        m_scratchpad->storeByte(MemoryMap::SCRATCHPAD_RANGE.remap(pAddress), value);
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
    if (MemoryMap::IO_PORTS_RANGE.contains(mappedAddress))
        return m_ioPorts->data();
    return nullptr;
}

const std::vector<uint8_t> *Bus::getMemoryRange(uint32_t addr) const
{
    auto mappedAddress = MemoryMap::mapAddress(addr);

    if (MemoryMap::BIOS_RANGE.contains(mappedAddress))
        return m_bios->data();
    if (MemoryMap::RAM_RANGE.contains(mappedAddress))
        return m_ram->data();
    if (MemoryMap::IO_PORTS_RANGE.contains(mappedAddress))
        return m_ioPorts->data();
    return nullptr;
}
