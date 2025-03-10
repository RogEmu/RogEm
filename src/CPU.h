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

struct CPU
{
    CPU(const Bus &bus);

    void step();
    Instruction fetchInstruction();
    void executeInstruction(const Instruction &instruction);

    uint32_t getReg(uint8_t reg) const;
    void setReg(uint8_t reg, uint32_t val);

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
    void loadUpperImmediate(const Instruction &instruction);

    // Shift Instructions
    void shiftLeftLogical(const Instruction &instruction);
    void shiftLeftLogicalVariable(const Instruction &instruction);
    void shiftRightLogical(const Instruction &instruction);
    void shiftRightArithmetic(const Instruction &instruction);
    void shiftRightArithmeticVariable(const Instruction &instruction);
    void shiftRightLogicalVariable(const Instruction &instruction);

    // Multiplication and division Instructions
    void multiply(const Instruction &instruction);
    void multiplyUnsigned(const Instruction &instruction);
    void divide(const Instruction &instruction);
    void divideUnsigned(const Instruction &instruction);
    void moveFromHi(const Instruction &instruction);
    void moveFromLo(const Instruction &instruction);
    void moveToHi(const Instruction &instruction);
    void moveToLo(const Instruction &instruction);

    // Jump and branche Instructions
    void jump(const Instruction &instruction);
    void jumpAndLink(const Instruction &instruction);
    void jumpRegister(const Instruction &instruction);
    void jumpAndLinkRegister(const Instruction &instruction);
    void branchOnEqual(const Instruction &instruction);
    void branchOnNotEqual(const Instruction &instruction);
    void branchOnLessThanZero(const Instruction &instruction);
    void branchOnGreaterThanOrEqualToZero(const Instruction &instruction);
    void branchOnGreaterThanZero(const Instruction &instruction);
    void branchOnLessThanOrEqualToZero(const Instruction &instruction);

    void illegalInstruction(const Instruction &instruction);
    void specialInstruction(const Instruction &instruction);
    void branchOnConditionZero(const Instruction &instruction);

    void debugState() const;

    // General purpose registers
    uint32_t m_registers[NB_GPR];

    // Reserved registers
    uint32_t m_pc;
    uint32_t m_hi;
    uint32_t m_lo;

    Bus m_bus;
};

#endif /* !CPU_H_ */
