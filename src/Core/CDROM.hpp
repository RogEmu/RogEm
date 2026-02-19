/*
** EPITECH PROJECT, 2025
** rogem
** File description:
** CDROM
*/

#ifndef CDROM_HPP_
#define CDROM_HPP_

#include <array>
#include <cstddef>
#include <cstdint>
#include <initializer_list>
#include <memory>

#include "PsxDevice.hpp"
#include "libcuebin/disc.hpp"

template<size_t Capacity>
struct Fifo {
    std::array<uint8_t, Capacity> data{};
    uint16_t readPos = 0, writePos = 0, count = 0;

    void push(uint8_t value) {
        if (count < Capacity) {
            data[writePos] = value;
            writePos = (writePos + 1) % Capacity;
            count++;
        }
    }

    uint8_t pop() {
        if (count == 0)
            return 0;
        uint8_t val = data[readPos];
        readPos = (readPos + 1) % Capacity;
        count--;
        return val;
    }

    void clear() {
        readPos = 0;
        writePos = 0;
        count = 0;
    }

    bool isEmpty() const { return count == 0; }
    bool isFull() const { return count >= Capacity; }
    uint16_t size() const { return count; }
};

enum class CDROMState {
    Idle,
    WaitingFirstResponse,
    WaitingSecondResponse,
    Reading
};

namespace CDROMInterrupt {
    constexpr uint8_t DataReady   = 1;  // INT1
    constexpr uint8_t Complete    = 2;  // INT2
    constexpr uint8_t Acknowledge = 3;  // INT3
    constexpr uint8_t DataEnd     = 4;  // INT4
    constexpr uint8_t Error       = 5;  // INT5
}

struct CDROMLocation {
    uint8_t minute = 0;
    uint8_t second = 0;
    uint8_t frame = 0;
};

struct PendingResponse {
    Fifo<16> data;
    uint8_t interruptType = 0;
};

class CDROM : public PsxDevice
{
    public:
        CDROM(Bus *bus);
        ~CDROM();

        void reset() override;
        void update(int cycles) override;

        void loadDisc(std::unique_ptr<cuebin::Disc> &disc);
        bool hasDisc() const;
        uint32_t readDataWord();

        void write8(uint8_t value, uint32_t address) override;
        void write16(uint16_t value, uint32_t address) override;
        void write32(uint32_t value, uint32_t address) override;

        uint8_t read8(uint32_t address) override;
        uint16_t read16(uint32_t address) override;
        uint32_t read32(uint32_t address) override;

    private:
        uint8_t computeStatusRegister() const;
        void startCommand(uint8_t cmd);
        void executeCommand();
        void deliverSecondResponse();
        void deliverInterrupt(uint8_t type);
        void pushResponse(uint8_t type, std::initializer_list<uint8_t> bytes);
        void setSecondResponse(uint8_t type, std::initializer_list<uint8_t> bytes, int delay);
        static const char* commandName(uint8_t cmd);
        void logCommand(uint8_t cmd);
        static uint8_t toBcd(uint8_t value);
        static uint8_t fromBcd(uint8_t bcd);
        static uint32_t msfToLba(uint8_t minute, uint8_t second, uint8_t frame);
        static void lbaToMsf(uint32_t lba, uint8_t& minute, uint8_t& second, uint8_t& frame);

        // Command handlers
        void cmdGetStat();
        void cmdSetLoc();
        void cmdPlay();
        void cmdReadN();
        void cmdMotorOn();
        void cmdStop();
        void cmdPause();
        void cmdInit();
        void cmdMute();
        void cmdDemute();
        void cmdSetFilter();
        void cmdSetMode();
        void cmdGetLocL();
        void cmdGetLocP();
        void cmdGetTN();
        void cmdGetTD();
        void cmdSeekL();
        void cmdTest();
        void cmdGetID();
        void cmdReadS();
        void cmdReadTOC();

        void readSector();

        // Register state
        uint8_t m_index;
        uint8_t m_interruptFlag;
        uint8_t m_interruptEnable;

        // FIFOs
        Fifo<16> m_parameterFifo;
        Fifo<16> m_responseFifo;
        Fifo<2340> m_dataFifo;

        // State machine
        CDROMState m_state;
        uint8_t m_currentCommand;
        int m_delayCounter;
        PendingResponse m_secondResponse;
        bool m_hasSecondResponse;

        // CDROM internal state
        uint8_t m_stat;         // Response status byte (0x02 = motor on)
        uint8_t m_mode;
        CDROMLocation m_seekTarget;
        CDROMLocation m_readPos;
        bool m_wantData;

        // Volume registers (stubbed)
        uint8_t m_volumeRR;
        uint8_t m_volumeRL;
        uint8_t m_volumeLR;
        uint8_t m_volumeLL;

        // Disc
        std::unique_ptr<cuebin::Disc> m_disc;
        std::array<uint8_t, 8> m_lastSectorHeader{};

        // Reading state
        int m_readDelayCounter;
        std::array<uint8_t, cuebin::RAW_SECTOR_SIZE> m_sectorBuffer;
};

#endif /* !CDROM_HPP_ */
