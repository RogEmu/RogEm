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
#include <unordered_map>

#include "PsxDevice.hpp"

class CPU;

enum class PsxDeviceType {
    RAM,
    BIOS,
    DMA,
    SPU,
    GPU,
    Scratchpad,
    Timers,
    IRQController,
    MemControl1,
    MemControl2
};

class Bus
{
    public:
        Bus();
        ~Bus();

        uint32_t loadWord(uint32_t addr) const;
        void storeWord(uint32_t addr, uint32_t value);
        uint16_t loadHalfWord(uint32_t addr) const;
        void storeHalfWord(uint32_t addr, uint16_t value);
        uint8_t loadByte(uint32_t addr) const;
        void storeByte(uint32_t addr, uint8_t value);

        std::vector<uint8_t> *getMemoryRange(uint32_t addr);
        const std::vector<uint8_t> *getMemoryRange(uint32_t addr) const;

        PsxDevice *getDevice(PsxDeviceType deviceType);

        void connectCpu(CPU *cpu);
        CPU *getCpu();

    private:
        std::unordered_map<PsxDeviceType, std::unique_ptr<PsxDevice>> m_devices;

        uint32_t m_cacheControl;
        CPU *m_cpu;
};

#endif /* !BUS_H_ */
