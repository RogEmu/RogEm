/*
** EPITECH PROJECT, 2025
** rogem
** File description:
** Disc
*/

#include "Disc.hpp"

#include <spdlog/spdlog.h>

Disc::Disc()
{
}

Disc::~Disc()
{
    close();
}

bool Disc::open(const std::string& path)
{
    m_file.open(path, std::ios::binary | std::ios::ate);
    if (!m_file.is_open()) {
        spdlog::error("Disc: Failed to open file: {}", path);
        return false;
    }
    auto fileSize = m_file.tellg();
    m_file.seekg(0, std::ios::beg);
    m_totalSectors = static_cast<uint32_t>(fileSize) / RAW_SECTOR_SIZE;
    spdlog::info("Disc: Opened '{}' ({} sectors)", path, m_totalSectors);
    return true;
}

void Disc::close()
{
    if (m_file.is_open())
        m_file.close();
    m_totalSectors = 0;
}

bool Disc::isOpen() const
{
    return m_file.is_open();
}

bool Disc::readSector(uint32_t lba, uint8_t* buffer)
{
    if (lba >= m_totalSectors) {
        spdlog::warn("Disc: LBA {} out of range (total {})", lba, m_totalSectors);
        return false;
    }
    m_file.seekg(static_cast<std::streamoff>(lba) * RAW_SECTOR_SIZE);
    m_file.read(reinterpret_cast<char*>(buffer), RAW_SECTOR_SIZE);
    return m_file.good();
}

uint32_t Disc::totalSectors() const
{
    return m_totalSectors;
}

uint32_t Disc::msfToLba(uint8_t minute, uint8_t second, uint8_t frame)
{
    return (minute * 60 + second) * 75 + frame - 150;
}

void Disc::lbaToMsf(uint32_t lba, uint8_t& minute, uint8_t& second, uint8_t& frame)
{
    uint32_t absolute = lba + 150;
    minute = static_cast<uint8_t>(absolute / (60 * 75));
    absolute %= (60 * 75);
    second = static_cast<uint8_t>(absolute / 75);
    frame = static_cast<uint8_t>(absolute % 75);
}

uint8_t Disc::toBcd(uint8_t value)
{
    return static_cast<uint8_t>((value / 10) << 4 | (value % 10));
}

uint8_t Disc::fromBcd(uint8_t bcd)
{
    return static_cast<uint8_t>((bcd >> 4) * 10 + (bcd & 0x0F));
}
