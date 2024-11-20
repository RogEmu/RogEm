/*
** EPITECH PROJECT, 2024
** RogEm
** File description:
** CPU
*/

#ifndef CPU_H_
#define CPU_H_

#include <cstdint>
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
        void orImmediate(const Instruction &instruction);
        void shiftLeftLogical(const Instruction &instruction);

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

        void illegalInstruction(const Instruction &instruction);

    private:
        // General purpose registers
        uint32_t m_registers[NB_GPR];

        // Reserved registers
        uint32_t m_pc;
        uint32_t m_hi;
        uint32_t m_lo;

        Bus m_bus;
};

#endif /* !CPU_H_ */
