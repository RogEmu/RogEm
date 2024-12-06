#include "RAM.h"

RAM::RAM()
{
}

uint32_t RAM::loadWord(uint32_t addr) const
{
    uint32_t b1 = m_data[addr];
    uint32_t b2 = m_data[addr + 1] << 8;
    uint32_t b3 = m_data[addr + 2] << 16;
    uint32_t b4 = m_data[addr + 3] << 24;
    return b4 | b3 | b2 | b1;
}

uint16_t RAM::loadHalfWord(uint32_t addr) const
{
    uint16_t b1 = m_data[addr];
    uint16_t b2 = m_data[addr + 1] << 8;
    return b1 | b2;
}

uint8_t RAM::loadByte(uint32_t addr) const
{
    return m_data[addr];
}

void RAM::storeWord(uint32_t addr, uint32_t val)
{
    m_data[addr] = val & 0xFF;
    m_data[addr + 1] = val >> 8 & 0xFF;
    m_data[addr + 2] = val >> 16 & 0xFF;
    m_data[addr + 3] = val >> 24 & 0xFF;
}

void RAM::storeHalfWord(uint32_t addr, uint16_t val)
{
    m_data[addr] = val & 0xFF;
    m_data[addr + 1] = val >> 8 & 0xFF;
}

void RAM::storeByte(uint32_t addr, uint8_t val)
{
    m_data[addr] = val;
}
