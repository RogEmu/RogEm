/*
** EPITECH PROJECT, 2024
** RogEm
** File description:
** CPU
*/

#ifndef CPU_H_
#define CPU_H_

#include <cstdint>
#include <iomanip>
#include "Instruction.h"
#include "Bus.h"

#define RESET_VECTOR 0xBFC00000
#define NB_GPR 32

class CPU
{
    public:
        CPU(const Bus &bus);

        void step();
        Instruction fetchInstruction();
        uint32_t loadWord(uint32_t addr);
        void executeInstruction(const Instruction &instruction);

        uint32_t getReg(uint8_t reg) const;
        void setReg(uint8_t reg, uint32_t val);

        // Instructions
        void loadUpperImmediate(const Instruction &instruction);

        // Load instructions
        void loadWord(const Instruction &instruction);
        void loadByte(const Instruction &instruction);
        void loadByteUnsigned(const Instruction &instruction);
        void loadHalfWord(const Instruction &instruction);
        void loadHalfWordUnsigned(const Instruction &instruction);

        // Unaligned Load Instructions
        void loadWordRight(const Instruction &instruction);
        void loadWordLeft(const Instruction &instruction);

        // Unaligned Store Instructions
        void storeWordRight(const Instruction &instruction);
        void storeWordLeft(const Instruction &instruction);

        // Store instructions
        void storeWord(const Instruction &instruction);
        void storeHalfWord(const Instruction &instruction);
        void storeByte(const Instruction &instruction);

        // Arithmetic Instructions
        void addWord(const Instruction &instruction);
        void addWordUnsigned(const Instruction &instruction);
        void addImmediate(const Instruction &instruction);
        void addImmediateUnsigned(const Instruction &instruction);

        void substractWordUnsigned(const Instruction &instruction);
        void substractWord(const Instruction &instruction);

        // Comparison Instructions
        void setOnLessThan(const Instruction &instruction);
        void setOnLessThanImmediate(const Instruction &instruction);
        void setOnLessThanUnsigned(const Instruction &instruction);
        void setOnLessThanImmediateUnsigned(const Instruction &instruction);

        // Logic Instructions
        void andWord(const Instruction &instruction);
        void orWord(const Instruction &instruction);
        void xorWord(const Instruction &instruction);
        void norWord(const Instruction &instruction);
        void andImmediateWord(const Instruction &instruction);
        void orImmediateWord(const Instruction &instruction);
        void xorImmediateWord(const Instruction &instruction);

        // Shift Instructions
        void shiftLeftLogical(const Instruction &instruction);
        void shiftLeftLogicalVariable(const Instruction &instruction);
        void shiftRightLogical(const Instruction &instruction);
        void shiftRightArithmetic(const Instruction &instruction);
        void shiftRightArithmeticVariable(const Instruction &instruction);
        void shiftRightLogicalVariable(const Instruction &instruction);

        // Jump Instructions
        void jump(const Instruction &instruction);

        void illegalInstruction(const Instruction &instruction);
        void specialInstruction(const Instruction &instruction);

    private:
        void debugState() const;

    private:
        // General purpose registers
        uint32_t m_registers[NB_GPR];

        // Reserved registers
        uint32_t m_pc;
        uint32_t m_hi;
        uint32_t m_lo;

        Bus m_bus;

        const std::string m_registerNames[32] = {
            "zero", "at", "v0", "v1", "a0", "a1", "a2", "a3",
            "t0", "t1", "t2", "t3", "t4", "t5", "t6", "t7",
            "s0", "s1", "s2", "s3", "s4", "s5", "s6", "s7",
            "t8", "t9", "k0", "k1", "gp", "sp", "fp", "ra"
        };
};

#endif /* !CPU_H_ */
