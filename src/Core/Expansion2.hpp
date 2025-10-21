#ifndef EXPANSION2_HPP_
#define EXPANSION2_HPP_

#include "PsxDevice.hpp"

class Expansion2 : public PsxDevice
{
    public:
        Expansion2(Bus *bus);
        ~Expansion2();

        void write8(uint8_t value, uint32_t address) override;
        void write16(uint16_t value, uint32_t address) override;
        void write32(uint32_t value, uint32_t address) override;

        uint8_t read8(uint32_t address) override;
        uint16_t read16(uint32_t address) override;
        uint32_t read32(uint32_t address) override;
};

#endif /* !EXPANSION2_HPP_ */
