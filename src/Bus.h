/*
** EPITECH PROJECT, 2024
** RogEm
** File description:
** Bus
*/

#ifndef BUS_H_
#define BUS_H_

#include "BIOS.h"

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

class Bus
{
    public:
        Bus(const BIOS &bios);
        ~Bus();

        uint32_t loadWord(uint32_t addr) const;
        void storeWord(uint32_t addr, uint32_t value);

    private:
        BIOS m_bios;
};

#endif /* !BUS_H_ */
