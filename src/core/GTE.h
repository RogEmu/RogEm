/*
** EPITECH PROJECT, 2024
** RogEm
** File description:
** GTE
*/

#ifndef GTE_H_
#define GTE_H_

#include <cstdint>
#include <array>
#include "Instruction.h"
#include "Coprocessor.h"

class GTE : public Coprocessor {
    public:
        GTE();

        void reset();

        void execute(uint32_t opcode) override;
        void mtc(uint8_t reg, uint32_t value) override;
        void ctc(uint8_t reg, uint32_t value) override;
        uint32_t mfc(uint8_t reg) override;
        uint32_t cfc(uint8_t reg) override;

    private:
            // Coprocessor 2 Control Registers (Cop2C)
        std::array<int32_t, 32> m_ctrlReg{};

        // Coprocessor 2 Data Registers (Cop2D)
        std::array<int32_t, 32> m_dataReg{};

        void decodeAndExecute(uint32_t opcode);

        void executeRTPS();
        void executeRTPT();

        // Internal helper functions
        int16_t extractSigned16(uint32_t value, bool upper);
        int32_t clampMAC(int64_t value, int limitHigh, int limitLow, uint32_t flagBit);

        // Flag management
        static constexpr int32_t IR_LIMIT_LOW = -0x8000;
        static constexpr int32_t IR_LIMIT_HIGH = 0x7FFF;
        void updateFlags(int32_t value, int limitHigh, int limitLow, uint32_t flagBit);
};

#endif /* !GTE_H_ */
