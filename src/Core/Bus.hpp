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
#include <spdlog/spdlog.h>

#include "PsxDevice.hpp"

class CPU;

class Bus
{
    public:
        Bus();
        ~Bus();

        template<typename T>
        inline T load(uint32_t addr) const {
            uint32_t pAddress = MemoryMap::mapAddress(addr);

            for (auto &[_, device] : m_devices) {
                if (device->isAddressed(pAddress)) {
                    if (std::is_same<T, uint32_t>()) {
                        return device->read32(pAddress);
                    } else if (std::is_same<T, uint16_t>()) {
                        return device->read16(pAddress);
                    } else if (std::is_same<T, uint8_t>()) {
                        return device->read8(pAddress);
                    } else {
                        static_assert("Unsupported type for Bus::load");
                    }
                }
            }
            spdlog::error("Bus: Read at address 0x{:08X} is not supported", addr);
            return T{};
        }

        template<typename T>
        inline void store(uint32_t addr, T value) {
            uint32_t pAddress = MemoryMap::mapAddress(addr);

            for (auto &[_, device] : m_devices) {
                if (device->isAddressed(pAddress)) {
                    if (std::is_same<T, uint32_t>()) {
                        device->write32(value, pAddress);
                        return;
                    } else if (std::is_same<T, uint16_t>()) {
                        device->write16(value, pAddress);
                        return;
                    } else if (std::is_same<T, uint8_t>()) {
                        device->write8(value, pAddress);
                        return;
                    } else {
                        static_assert("Unsupported type for Bus::store");
                    }
                }
            }
            spdlog::error("Bus: Write at address 0x{:08X} is not supported", addr);
        }

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
