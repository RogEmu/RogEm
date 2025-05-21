/*
** EPITECH PROJECT, 2025
** rogem
** File description:
** Memory
*/

#include "Memory.hpp"

Memory::Memory(uint32_t size, uint8_t initVal) :
    PsxDevice()
{
    m_data.resize(size, initVal);
    m_readOnly = false;
}

uint32_t Memory::read32(uint32_t addr)
{
    addr = m_memoryRange.remap(addr);
    uint32_t b1 = m_data[addr];
    uint32_t b2 = m_data[addr + 1] << 8;
    uint32_t b3 = m_data[addr + 2] << 16;
    uint32_t b4 = m_data[addr + 3] << 24;
    return b4 | b3 | b2 | b1;
}

uint16_t Memory::read16(uint32_t addr)
{
    addr = m_memoryRange.remap(addr);
    uint16_t b1 = m_data[addr];
    uint16_t b2 = m_data[addr + 1] << 8;
    return b1 | b2;
}

uint8_t Memory::read8(uint32_t addr)
{
    addr = m_memoryRange.remap(addr);
    return m_data[addr];
}

void Memory::write32(uint32_t val, uint32_t addr)
{
    if (m_readOnly) {
        return;
    }
    addr = m_memoryRange.remap(addr);
    m_data[addr] = val & 0xFF;
    m_data[addr + 1] = val >> 8 & 0xFF;
    m_data[addr + 2] = val >> 16 & 0xFF;
    m_data[addr + 3] = val >> 24 & 0xFF;
}

void Memory::write16(uint16_t val, uint32_t addr)
{
    if (m_readOnly) {
        return;
    }
    addr = m_memoryRange.remap(addr);
    m_data[addr] = val & 0xFF;
    m_data[addr + 1] = val >> 8 & 0xFF;
}

void Memory::write8(uint8_t val, uint32_t addr)
{
    if (m_readOnly) {
        return;
    }
    addr = m_memoryRange.remap(addr);
    m_data[addr] = val;
}

std::vector<uint8_t> *Memory::data()
{
    return &m_data;
}

const std::vector<uint8_t> *Memory::data() const
{
    return &m_data;
}

void Memory::setReadOnly(bool readOnly)
{
    m_readOnly = readOnly;
}

bool Memory::isReadOnly() const
{
    return m_readOnly;
}
