/*
** EPITECH PROJECT, 2024
** RogEm
** File description:
** CPU
*/

#ifndef CPU_H_
#define CPU_H_

#include <cstdint>
#include <memory>
#include "Instruction.h"
#include "Bus.h"

#define RESET_VECTOR (uint32_t)0xBFC00000
#define NB_GPR 32
#define COP0_NB_REG 16

enum class GprIndex
{
    ZERO = 0,
    AT,
    V0, V1,
    A0, A1, A2, A3,
    T0, T1, T2, T3, T4, T5, T6, T7,
    S0, S1, S2, S3, S4, S5, S6, S7,
    T8, T9,
    K0, K1,
    GP,
    SP,
    FP,
    RA
};

enum class SpecialRegIndex
{
    PC = 0,
    HI,
    LO
};

enum class CP0RegIndex
{
    BPC = 3,
    BDA = 5,
    JUMPDEST,
    DCIC,
    BadVaddr,
    BDAM,
    BPCM = 11,
    SR,
    CAUSE,
    EPC,
    PRID
};

struct CPU
{
    CPU(Bus *bus);

    void step();
    Instruction fetchInstruction();
    void executeInstruction(const Instruction &instruction);
    void reset();

    uint32_t getPC() const;
    void setPC(uint32_t pc);

    uint32_t getReg(uint8_t reg) const;
    void setReg(uint8_t reg, uint32_t val);

    uint32_t getSpecialReg(uint8_t reg) const;
    void setSpecialReg(uint8_t reg, uint32_t val);

    uint32_t getCop0Reg(uint8_t reg) const;
    void setCop0Reg(uint8_t reg, uint32_t val);

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
    void addImmediate(const Instruction &instruction); // Tested
    void addImmediateUnsigned(const Instruction &instruction); // Tested

    void substractWordUnsigned(const Instruction &instruction); // Tested
    void substractWord(const Instruction &instruction); // Tested

    // Comparison Instructions
    void setOnLessThan(const Instruction &instruction);
    void setOnLessThanImmediate(const Instruction &instruction);
    void setOnLessThanUnsigned(const Instruction &instruction);
    void setOnLessThanImmediateUnsigned(const Instruction &instruction);

    // Logic Instructions
    void andWord(const Instruction &instruction); // Tested
    void orWord(const Instruction &instruction); // Tested
    void xorWord(const Instruction &instruction); // Tested
    void norWord(const Instruction &instruction); // Tested
    void andImmediateWord(const Instruction &instruction); // Tested
    void orImmediateWord(const Instruction &instruction); // Tested
    void xorImmediateWord(const Instruction &instruction); // Tested
    void loadUpperImmediate(const Instruction &instruction); // Tested

    // Shift Instructions
    void shiftLeftLogical(const Instruction &instruction); // Tested
    void shiftLeftLogicalVariable(const Instruction &instruction); // Tested
    void shiftRightLogical(const Instruction &instruction); // Tested
    void shiftRightArithmetic(const Instruction &instruction); // Tested
    void shiftRightArithmeticVariable(const Instruction &instruction); // Tested
    void shiftRightLogicalVariable(const Instruction &instruction);

    // Multiplication and division Instructions
    void multiply(const Instruction &instruction); // Tested - needs checking
    void multiplyUnsigned(const Instruction &instruction); // Tested - needs checking
    void divide(const Instruction &instruction); // Tested - needs checking
    void divideUnsigned(const Instruction &instruction); // Tested - needs checking
    void moveFromHi(const Instruction &instruction); // Tested
    void moveFromLo(const Instruction &instruction); // Tested
    void moveToHi(const Instruction &instruction); // Tested
    void moveToLo(const Instruction &instruction); // Tested

    // Jump Instructions
    void jump(const Instruction &instruction); // Tested
    void jumpAndLink(const Instruction &instruction); // Tested
    void jumpRegister(const Instruction &instruction); // Tested
    void jumpAndLinkRegister(const Instruction &instruction); // Tested

    // Branch instructions
    void executeBranch(const Instruction &instruction);
    void branchOnEqual(const Instruction &instruction); // Tested
    void branchOnNotEqual(const Instruction &instruction);
    void branchOnLessThanZero(const Instruction &instruction);
    void branchOnLessThanZeroAndLink(const Instruction &instruction);
    void branchOnGreaterThanOrEqualToZero(const Instruction &instruction);
    void branchOnGreaterThanZero(const Instruction &instruction);
    void branchOnGreaterThanZeroAndLink(const Instruction &instruction);
    void branchOnLessThanOrEqualToZero(const Instruction &instruction);
    void branchOnGreaterThanOrEqualToZeroAndLink(const Instruction &instruction);

    //COP0 Instructions
    void executeCop0(const Instruction &instruction);
    void mtc0(const Instruction &instruction); // Tested
    void mfc0(const Instruction &instruction); // Tested

    void illegalInstruction(const Instruction &instruction);
    void specialInstruction(const Instruction &instruction);
    void branchOnConditionZero(const Instruction &instruction);

    // General purpose registers
    uint32_t gpr[NB_GPR];

    // Reserved registers
    uint32_t pc;
    uint32_t hi;
    uint32_t lo;

    uint32_t m_branchSlotAddr;
    bool m_inBranchDelay;

    // COP0 registers
    uint32_t m_cop0Reg[COP0_NB_REG];

    // Bus connection
    Bus *m_bus;
};

#endif /* !CPU_H_ */
