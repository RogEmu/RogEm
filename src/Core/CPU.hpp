/*
** EPITECH PROJECT, 2024
** RogEm
** File description:
** CPU
*/

#ifndef CPU_HPP_
#define CPU_HPP_

#include <cstdint>
#include <memory>
#include <string>
#include "Instruction.h"
#include "Bus.hpp"
#include "SystemControlCop.hpp"
#include "GTE.hpp"

#define RESET_VECTOR (uint32_t)0xBFC00000
#define NB_GPR 32
#define COP0_NB_REG 16

enum class CpuReg
{
    // General purpose registers
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
    RA,

    // Special registers
    PC,
    HI,
    LO
};

enum class CP0Reg
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

enum class ExceptionVector : uint32_t
{
    // BEV = 0
    RESET = 0xBFC00000, // Reset
    UTLB_MISS = 0x80000000, // UTLB Miss, not used
    COP0_BRK = 0x80000040, // COP0 Break
    GENERAL = 0x80000080, // General
    // BEV = 1 Vectors are unused ?
};

enum class ExceptionType
{
    Interrupt = 0,
    AddressErrorLoad= 4,
    AddressErrorStore,
    IBusError,
    DBusError,
    Syscall,
    Breakpoint,
    RI,
    COP_Unusable,
    Overflow
};

struct LoadDelaySlot
{
    uint32_t value;
    CpuReg reg;
    bool pending;
};

class CPU {
    public:
        CPU(Bus *bus);

        void step();
        void reset();
        void setTtyOutputFlag(bool ttyOutput);
        bool getTtyOutputFlag();
        std::string getTtyOutput();
        uint32_t getReg(CpuReg reg) const;
        void setReg(CpuReg reg, uint32_t val);

        uint32_t getCop0Reg(uint8_t reg);
        void setCop0Reg(uint8_t reg, uint32_t val);
        void setInterruptPending(bool pending);

    private:
        Instruction fetchInstruction();
        void executeInstruction(const Instruction &instruction);
        void handleLoadDelay();
        bool interruptsEnabled();
        bool interruptPending();

        // Load instructions
        void loadWithDelay(CpuReg reg, uint32_t value);
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

        // Jump Instructions
        void jump(const Instruction &instruction);
        void jumpAndLink(const Instruction &instruction);
        void jumpRegister(const Instruction &instruction);
        void jumpAndLinkRegister(const Instruction &instruction);

        // Branch instructions
        void executeBranch(const Instruction &instruction);
        void branchOnConditionZero(const Instruction &instruction);
        void branchOnEqual(const Instruction &instruction); // BEQ
        void branchOnNotEqual(const Instruction &instruction); // BNZ
        void branchOnLessThanOrEqualToZero(const Instruction &instruction); // BLEZ
        void branchOnGreaterThanZero(const Instruction &instruction); // BGTZ
        void branchOnLessThanZero(const Instruction &instruction); // BLTZ
        void branchOnLessThanZeroAndLink(const Instruction &instruction); //BLTZAL
        void branchOnGreaterThanOrEqualToZero(const Instruction &instruction); //BGEZ
        void branchOnGreaterThanOrEqualToZeroAndLink(const Instruction &instruction); // BGEZAL

        //COP Instructions
        void executeCoprocessor(const Instruction &instruction);
        void executeCOP0(const Instruction &instruction);
        void executeCOP2(const Instruction &instruction);
        void mtc0(const Instruction &instruction);
        void mfc0(const Instruction &instruction);
        void mtc2(const Instruction &instruction);
        void mfc2(const Instruction &instruction);
        void ctc2(const Instruction &instruction);
        void cfc2(const Instruction &instruction);
        void returnFromException(const Instruction &instruction);

        // Coprocessor Load/Store
        void lwc0(const Instruction &instruction);
        void swc0(const Instruction &instruction);
        void lwc2(const Instruction &instruction);
        void swc2(const Instruction &instruction);

        // Exception Instructions
        void triggerException(ExceptionType exception);
        void executeSyscall(const Instruction &instruction);
        void executeBreak(const Instruction &instruction);

        void illegalInstruction(const Instruction &instruction);
        void specialInstruction(const Instruction &instruction);

        void checkTtyOutput();

    private:
        // Main CPU registers
        uint32_t m_gpr[NB_GPR];
        uint32_t m_pc;
        uint32_t m_hi;
        uint32_t m_lo;

        SystemControlCop m_cop0;
        GTE m_gte;

        uint32_t m_nextPc;

        LoadDelaySlot m_loadDelaySlots[2];

        uint32_t m_branchSlotAddr;
        bool m_inBranchDelay;
        bool m_nextIsBranchDelay;

        bool m_isTtyOutput;
        std::string m_ttyOutput;

        bool m_jumpToUnaligned;
        uint32_t m_badVarAddr;

        // Bus connection
        Bus *m_bus;
};

#endif /* !CPU_HPP_ */
