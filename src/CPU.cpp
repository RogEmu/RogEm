/*
** EPITECH PROJECT, 2024
** RogEm
** File description:
** CPU
*/

#include "CPU.h"
#include <iostream>

static void printInstruction(Instruction inst)
{
    switch (inst.r.opcode)
    {
    case 0x0:
        printf("op:0x%02X rs:0x%02X rt:0x%02X rd:0x%02X shamt:0x%02X func:0x%02X\n",
        inst.r.opcode, inst.r.rs, inst.r.rt, inst.r.rd, inst.r.shamt, inst.r.funct);
        break;
    case 0x02:
    case 0x03:
        printf("op:0x%02X addr:0x%08X\n", inst.j.opcode, inst.j.address);
    default:
        printf("op:0x%02X rs:0x%02X rt:0x%02X imm:0x%04X\n", inst.i.opcode, inst.i.rs, inst.i.rt, inst.i.immediate);
        break;
    }
}

static bool addOverflow(int32_t a, int32_t b)
{
    return ((uint32_t)(a + b) < a);
}

CPU::CPU(const Bus &bus) :
    m_pc(RESET_VECTOR),
    m_bus(bus)
{
    m_registers[0] = 0;
}

void CPU::step()
{
    Instruction instruction = fetchInstruction();

    executeInstruction(instruction);
    printInstruction(instruction);
}

Instruction CPU::fetchInstruction()
{
    uint32_t instruction = loadWord(m_pc);
    m_pc += 4;
    return (Instruction){.raw=instruction};
}

uint32_t CPU::loadWord(uint32_t addr)
{
    return m_bus.loadWord(addr);
}

void CPU::executeInstruction(const Instruction &instruction)
{
    switch (instruction.r.opcode)
    {
    case 0x0F:
        loadUpperImmediate(instruction);
        break;
    case 0x0D:
        orImmediate(instruction);
        break;
    case 0x2B:
        storeWord(instruction);
        break;
    case 0x09:
        addImmediateUnsigned(instruction);
        break;
    case 0x00:
        switch (instruction.r.funct)
        {
        case 0x00:
            shiftLeftLogical(instruction);
            break;
        default:
            illegalInstruction(instruction);
            break;
        }
        break;
    default:
        illegalInstruction(instruction);
        break;
    }
}

uint32_t CPU::getReg(uint8_t reg) const
{
    return m_registers[reg];
}

void CPU::setReg(uint8_t reg, uint32_t val)
{
    m_registers[reg] = val;
    m_registers[0] = 0;
}

void CPU::loadUpperImmediate(const Instruction &instruction)
{
    uint32_t res = instruction.i.immediate;
    res = res << 16;
    setReg(instruction.i.rt, res);
}

void CPU::orImmediate(const Instruction &instruction)
{
    uint32_t res = getReg(instruction.i.rs) | instruction.i.immediate;

    setReg(instruction.i.rt, res);
}

void CPU::storeWord(const Instruction &instruction)
{
    int32_t imm = (int16_t)instruction.i.immediate;
    uint32_t address = getReg(instruction.i.rs) + imm;
    uint32_t value = getReg(instruction.i.rt);

    m_bus.storeWord(address, value);
}

void CPU::storeHalfWord(const Instruction &instruction)
{
    int32_t imm = (int16_t)instruction.i.immediate;
    uint32_t address = getReg(instruction.i.rs) + imm;
    uint16_t value = getReg(instruction.i.rt);

    m_bus.storeHalfWord(address, value);
}

void CPU::storeByte(const Instruction &instruction)
{
    int32_t imm = (int16_t)instruction.i.immediate;
    uint32_t address = getReg(instruction.i.rs) + imm;
    uint8_t value = getReg(instruction.i.rt);

    m_bus.storeByte(address, value);
}

void CPU::shiftLeftLogical(const Instruction &instruction)
{
    uint32_t res = getReg(instruction.r.rt) << instruction.r.shamt;

    setReg(instruction.r.rd, res);
}

void CPU::addImmediateUnsigned(const Instruction &instruction)
{
    int32_t imm = (int16_t)instruction.i.immediate;
    uint32_t res = getReg(instruction.i.rs) + imm;

    setReg(instruction.i.rt, res);
}

void CPU::substractWordUnsigned(const Instruction &instruction)
{
    uint32_t left = getReg(instruction.r.rs);
    uint32_t right = getReg(instruction.r.rt);
    uint32_t tmp = left - right;

    setReg(instruction.r.rd, tmp);
}

void CPU::addWord(const Instruction &instruction)
{
    uint32_t left = getReg(instruction.r.rt);
    uint32_t right = getReg(instruction.r.rs);
    uint32_t tmp = left - right;

    setReg(instruction.r.rd, tmp);
}

void CPU::addWordUnsigned(const Instruction &instruction)
{
    uint32_t left = getReg(instruction.r.rs);
    uint32_t right = getReg(instruction.r.rt);
    uint32_t tmp = left + right;

    setReg(instruction.r.rd, tmp);
}

void CPU::illegalInstruction(const Instruction &instruction)
{
    fprintf(stderr, "Illegal instruction: ");
    printInstruction(instruction);

    // Temporary exit until exception handling is properly implemented
    exit(1);
}
