/*
** EPITECH PROJECT, 2024
** RogEm
** File description:
** Bus
*/

#ifndef BUS_HPP_
#define BUS_HPP_

#include <cstdint>
#include <vector>
#include <memory>
#include <unordered_map>
#include <typeindex>

#include "PsxDevice.hpp"

class CPU;
class StateBuffer;

class Bus
{
    public:
        Bus();
        ~Bus();

        void reset();

        void serialize(StateBuffer &buf) const;
        void deserialize(StateBuffer &buf);

        uint32_t loadWord(uint32_t addr) const;
        void storeWord(uint32_t addr, uint32_t value);
        uint16_t loadHalfWord(uint32_t addr) const;
        void storeHalfWord(uint32_t addr, uint16_t value);
        uint8_t loadByte(uint32_t addr) const;
        void storeByte(uint32_t addr, uint8_t value);

        std::vector<uint8_t> *getMemoryRange(uint32_t addr);
        const std::vector<uint8_t> *getMemoryRange(uint32_t addr) const;

        template<typename T>
        void addDevice(std::unique_ptr<T> device) {
            m_devices[std::type_index(typeid(T))] = std::move(device);
        }

        template<typename T>
        T *getDevice() {
            auto it = m_devices.find(std::type_index(typeid(T)));
            if (it != m_devices.end()) {
                return dynamic_cast<T*>(it->second.get());
            }
            return nullptr;
        }

        void updateDevices(int cycles);

        void connectCpu(CPU *cpu);
        CPU *getCpu();

    private:
        std::unordered_map<std::type_index, std::unique_ptr<PsxDevice>> m_devices;

        uint32_t m_cacheControl;
        CPU *m_cpu;
};

#endif /* !BUS_HPP_ */
