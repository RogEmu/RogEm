/*
** EPITECH PROJECT, 2025
** rogem
** File description:
** Disc
*/

#ifndef DISC_HPP_
#define DISC_HPP_

#include <cstdint>
#include <fstream>
#include <string>

constexpr uint32_t RAW_SECTOR_SIZE = 2352;

class Disc
{
public:
    Disc();
    ~Disc();

    bool open(const std::string& path);
    void close();
    bool isOpen() const;

    bool readSector(uint32_t lba, uint8_t* buffer);
    uint32_t totalSectors() const;

    // MSF <-> LBA (MSF values are decimal, not BCD)
    static uint32_t msfToLba(uint8_t minute, uint8_t second, uint8_t frame);
    static void lbaToMsf(uint32_t lba, uint8_t& minute, uint8_t& second, uint8_t& frame);

    // BCD conversion
    static uint8_t toBcd(uint8_t value);
    static uint8_t fromBcd(uint8_t bcd);

private:
    std::ifstream m_file;
    uint32_t m_totalSectors = 0;
};

#endif /* !DISC_HPP_ */
