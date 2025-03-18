/*
** EPITECH PROJECT, 2024
** RogEm
** File description:
** Bus
*/

#ifndef BUS_H_
#define BUS_H_

#include <memory>

class BIOS;
struct RAM;

struct MemRange
{
    uint32_t start;
    uint32_t length;

    bool contains(uint32_t addr) const
    {
        return (addr >= start && addr < start + length);
    }

    uint32_t remap(uint32_t addr) const
    {
        return addr - start;
    }
};

enum class MemorySegments
{
    KUSEG,
    KSEG0,
    KSEG1,
    KSEG2
};

class Bus
{
    public:
        Bus(const std::shared_ptr<BIOS> &bios, const std::shared_ptr<RAM> &ram);
        ~Bus();

        uint32_t loadWord(uint32_t addr) const;
        void storeWord(uint32_t addr, uint32_t value);
        uint16_t loadHalfWord(uint32_t addr) const;
        void storeHalfWord(uint32_t addr, uint16_t value);
        uint8_t loadByte(uint32_t addr) const;
        void storeByte(uint32_t addr, uint8_t value);

        uint32_t mapAddress(uint32_t addr) const;

    private:
        std::shared_ptr<BIOS> m_bios;
        std::shared_ptr<RAM> m_ram;
};

#endif /* !BUS_H_ */
