/*
** EPITECH PROJECT, 2025
** rogem
** File description:
** TIMERS
*/

#ifndef TIMERS_HPP_
#define TIMERS_HPP_

#include "PsxDevice.hpp"

struct TimerMode
{
    uint32_t syncEnable : 1;
    uint32_t syncMode : 2;
    uint32_t resetCounter : 1;
    uint32_t irqTarget : 1;
    uint32_t irqMax : 1;
    uint32_t irqOnce : 1;
    uint32_t irqPulse : 1;
    uint32_t clockSource : 2;
    uint32_t irq : 1;
    uint32_t reachedTarget : 1;
    uint32_t reachedMax : 1;
};

struct Timer
{
    uint32_t currentValue;
    uint32_t targetValue;
    union {
        TimerMode mode;
        uint32_t rawMode;
    };
};

class Timers : public PsxDevice
{
    public:
        Timers(Bus *bus);
        ~Timers();

        void update(int cycles) override;

        void write8(uint8_t value, uint32_t address) override;
        void write16(uint16_t value, uint32_t address) override;
        void write32(uint32_t value, uint32_t address) override;

        uint8_t read8(uint32_t address) override;
        uint16_t read16(uint32_t address) override;
        uint32_t read32(uint32_t address) override;

    private:
        uint32_t readTimer(uint32_t address);
        void writeTimer(uint32_t address, uint32_t value);

    private:
        Timer m_timers[3];
};

#endif /* !TIMERS_HPP_ */
