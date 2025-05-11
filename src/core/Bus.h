/*
** EPITECH PROJECT, 2024
** RogEm
** File description:
** Bus
*/

#ifndef BUS_H_
#define BUS_H_

#include <cstdint>
#include <vector>
#include <memory>

class BIOS;
class RAM;
class Memory;

class Bus
{
    public:
        Bus(BIOS *bios, RAM *ram);
        ~Bus();

        uint32_t loadWord(uint32_t addr) const;
        void storeWord(uint32_t addr, uint32_t value);
        uint16_t loadHalfWord(uint32_t addr) const;
        void storeHalfWord(uint32_t addr, uint16_t value);
        uint8_t loadByte(uint32_t addr) const;
        void storeByte(uint32_t addr, uint8_t value);

        std::vector<uint8_t> *getMemoryRange(uint32_t addr);
        const std::vector<uint8_t> *getMemoryRange(uint32_t addr) const;

    private:
        BIOS *m_bios;
        RAM *m_ram;
        std::unique_ptr<Memory> m_ioPorts;
        std::unique_ptr<Memory> m_scratchpad;

        uint32_t m_cacheControl;
};

#endif /* !BUS_H_ */
