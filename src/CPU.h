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
#include "Bus.h"

#define RESET_VECTOR 0xBFC00000
#define NB_GPR 32

union Instruction {
    uint32_t raw;  // The raw 32-bit instruction

    // R-type format: opcode | rs | rt | rd | shamt | funct
    struct {
        uint32_t funct : 6;
        uint32_t shamt : 5;
        uint32_t rd    : 5;
        uint32_t rt    : 5;
        uint32_t rs    : 5;
        uint32_t opcode: 6;
    } r;

    // I-type format: opcode | rs | rt | immediate
    struct {
        uint32_t immediate : 16;
        uint32_t rt        : 5;
        uint32_t rs        : 5;
        uint32_t opcode    : 6;
    } i;

    // J-type format: opcode | address
    struct {
        uint32_t address : 26;
        uint32_t opcode  : 6;
    } j;
};

enum class PrimaryOpCode
{
    ILLEGAL = -1,
    SPECIAL = 0x00,
    J = 0x02,
    ADDIU = 0x09,
    ANDI = 0x0C,
    ORI = 0x0D,
    XORI = 0x0E,
    LUI = 0x0F,
    SW = 0x2B,
    SH = 0x29,
    SB = 0x28,
    LW = 0x23,
    LH = 0x21,
    LHU = 0x25,
    LB = 0x20,
    LBU = 0x24
};

enum class SecondaryOpCode
{
    ILLEGAL = -1,
    SLL = 0x00,
    ADD = 0x20,
    ADDU = 0x21,
    SUB = 0x22,
    SUBU = 0x23,
    AND = 0x24,
    OR = 0x25,
    XOR = 0x26,
    NOR = 0x27
};

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
        void shiftLeftLogical(const Instruction &instruction);

        // Load instructions
        void loadWord(const Instruction &instruction);
        void loadHalfWord(const Instruction &instruction);
        void loadByte(const Instruction &instruction);
        void loadByteUnsigned(const Instruction &instruction);
        void loadHalfWordUnsigned(const Instruction &instruction);

        // Store instructions
        void storeWord(const Instruction &instruction);
        void storeHalfWord(const Instruction &instruction);
        void storeByte(const Instruction &instruction);

        // Arithmetic Instructions
        void addWord(const Instruction &instruction);
        void addWordUnsigned(const Instruction &instruction);
        void addImmediateUnsigned(const Instruction &instruction);

        void substractWordUnsigned(const Instruction &instruction);
        void substractWord(const Instruction &instruction);

        // Logic Instructions
        void andWord(const Instruction &instruction);
        void orWord(const Instruction &instruction);
        void xorWord(const Instruction &instruction);
        void norWord(const Instruction &instruction);
        void andImmediateWord(const Instruction &instruction);
        void orImmediateWord(const Instruction &instruction);
        void xorImmediateWord(const Instruction &instruction);

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
