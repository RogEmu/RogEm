/*
** EPITECH PROJECT, 2024
** RogEm
** File description:
** CPU
*/

#include "CPU.h"
#include <iostream>
#include <cstring>
#include <fmt/format.h>

#include "Disassembler.h"

static bool addOverflow(int32_t a, int32_t b)
{
    if (a > 0 && b > 0 && a > INT32_MAX - b)
        return true;
    if (a < 0 && b < 0 && a < INT32_MIN - b)
        return true;
    return false;
}

static bool subOverflow(int32_t a, int32_t b)
{
    if (b < 0 && a > INT32_MAX + b)
        return true;
    if (b > 0 && a < INT32_MIN + b)
        return true;
    return false;
}

CPU::CPU(Bus *bus) :
    pc(RESET_VECTOR),
    m_bus(bus)
{
    gpr[0] = 0;
    m_inBranchDelay = false;
}

void CPU::step()
{
    Instruction instruction = fetchInstruction();
    // std::cout << Disassembler::disassemble(pc, instruction) << std::endl;
    auto nextPc = pc;
    if (m_inBranchDelay)
    {
        nextPc = m_branchSlotAddr;
        m_inBranchDelay = false;
    }
    else
    {
        nextPc += 4;
    }
    executeInstruction(instruction);
    // Disassembler::debugState(pc, gpr);
    pc = nextPc;
}

void CPU::reset()
{
    m_inBranchDelay = false;
    m_pc = RESET_VECTOR;
    std::memset(m_registers, 0, NB_GPR * sizeof(m_registers[0]));
    std::memset(m_cop0Reg, 0, COP0_NB_REG * sizeof(m_cop0Reg[0]));
}

Instruction CPU::fetchInstruction()
{
    uint32_t instruction = m_bus->loadWord(pc);
    return Instruction{.raw=instruction};
}

void CPU::executeInstruction(const Instruction &instruction)
{
    switch (static_cast<PrimaryOpCode>(instruction.r.opcode))
    {
    case PrimaryOpCode::LUI:
        loadUpperImmediate(instruction);
        break;
    case PrimaryOpCode::ORI:
        orImmediateWord(instruction);
        break;
    case PrimaryOpCode::SW:
        storeWord(instruction);
        break;
    case PrimaryOpCode::ADDIU:
        addImmediateUnsigned(instruction);
        break;
    case PrimaryOpCode::ADDI:
        addImmediate(instruction);
        break;
    case PrimaryOpCode::J:
        jump(instruction);
        break;
    case PrimaryOpCode::JAL:
        jumpAndLink(instruction);
        break;
    case PrimaryOpCode::LW:
        loadWord(instruction);
        break;
    case PrimaryOpCode::SH:
        storeHalfWord(instruction);
        break;
    case PrimaryOpCode::LB:
        loadByte(instruction);
        break;
    case PrimaryOpCode::LBU:
        loadByteUnsigned(instruction);
        break;
    case PrimaryOpCode::LHU:
        loadHalfWordUnsigned(instruction);
        break;
    case PrimaryOpCode::XORI:
        xorImmediateWord(instruction);
        break;
    case PrimaryOpCode::ANDI:
        andImmediateWord(instruction);
        break;
    case PrimaryOpCode::SB:
        storeByte(instruction);
        break;
    case PrimaryOpCode::LH:
        loadHalfWord(instruction);
        break;
    case PrimaryOpCode::SWR:
        storeWordRight(instruction);
        break;
    case PrimaryOpCode::SWL:
        storeWordLeft(instruction);
        break;
    case PrimaryOpCode::LWR:
        loadWordRight(instruction);
        break;
    case PrimaryOpCode::LWL:
        loadWordLeft(instruction);
        break;
    // case PrimaryOpCode::SLTI:
    //     setOnLessThanImmediate(instruction);
    //     break;
    // case PrimaryOpCode::SLTIU:
    //     setOnLessThanImmediateUnsigned(instruction);
    //     break;
    case PrimaryOpCode::SPECIAL:
        specialInstruction(instruction);
        break;
    case PrimaryOpCode::BCONDZ:
        branchOnConditionZero(instruction);
        break;
    case PrimaryOpCode::COP0:
        executeCop0(instruction);
        break;
    case PrimaryOpCode::BNE:
        branchOnNotEqual(instruction);
        break;
    case PrimaryOpCode::BEQ:
        branchOnEqual(instruction);
        break;
    default:
        illegalInstruction(instruction);
        break;
    }
}

void CPU::branchOnConditionZero(const Instruction &Instruction)
{
    switch (static_cast<BranchOnConditionZero>(Instruction.i.rt))
    {
    case BranchOnConditionZero::BLTZ:
        branchOnLessThanZero(Instruction);
        break;
    case BranchOnConditionZero::BGEZ:
        branchOnGreaterThanOrEqualToZero(Instruction);
        break;
    // case BranchOnConditionZero::BLTZAL:
    //     branchOnLessThanZeroAndLink(Instruction);
    //     break;
    // case BranchOnConditionZero::BGEZAL:
    //     branchOnGreaterOrEqualZeroAndLink(Instruction);
    //     break;
    default:
        illegalInstruction(Instruction);
        break;
    }
}

void CPU::specialInstruction(const Instruction &instruction)
{
    switch (static_cast<SecondaryOpCode>(instruction.r.funct))
    {
    case SecondaryOpCode::SLL:
        shiftLeftLogical(instruction);
        break;
    case SecondaryOpCode::OR:
        orWord(instruction);
        break;
    case SecondaryOpCode::XOR:
        xorWord(instruction);
        break;
    case SecondaryOpCode::NOR:
        norWord(instruction);
        break;
    case SecondaryOpCode::ADDU:
        addWordUnsigned(instruction);
        break;
    case SecondaryOpCode::SUBU:
        substractWordUnsigned(instruction);
        break;
    case SecondaryOpCode::AND:
        andWord(instruction);
        break;
    case SecondaryOpCode::SUB:
        substractWord(instruction);
        break;
    case SecondaryOpCode::ADD:
        addWord(instruction);
        break;
    case SecondaryOpCode::SLT:
        setOnLessThan(instruction);
        break;
    case SecondaryOpCode::SLTU:
        setOnLessThanUnsigned(instruction);
        break;
    case SecondaryOpCode::SLLV:
        shiftLeftLogicalVariable(instruction);
        break;
    case SecondaryOpCode::SRL:
        shiftRightLogical(instruction);
        break;
    case SecondaryOpCode::SRLV:
        shiftRightLogicalVariable(instruction);
        break;
    case SecondaryOpCode::SRA:
        shiftRightArithmetic(instruction);
        break;
    case SecondaryOpCode::SRAV:
        shiftRightArithmeticVariable(instruction);
        break;
    case SecondaryOpCode::MULT:
        multiply(instruction);
        break;
    case SecondaryOpCode::MULTU:
        multiplyUnsigned(instruction);
        break;
    // case SecondaryOpCode::DIV:
    //     divide(instruction);
    //     break;
    // case SecondaryOpCode::DIVU:
    //     divideUnsigned(instruction);
    //     break;
    // case SecondaryOpCode::MFHI:
    //     moveFromHi(instruction);
    //     break;
    // case SecondaryOpCode::MFLO:
    //     moveFromLo(instruction);
    //     break;
    // case SecondaryOpCode::MTHI:
    //     moveToHi(instruction);
    //     break;
    // case SecondaryOpCode::MTLO:
    //     moveToLo(instruction);
    //     break;
    case SecondaryOpCode::JR:
        jumpRegister(instruction);
        break;
    case SecondaryOpCode::JALR:
        jumpAndLinkRegister(instruction);
        break;
    default:
        illegalInstruction(instruction);
        break;
    }
}

uint32_t CPU::getReg(uint8_t reg) const
{
    return gpr[reg];
}

void CPU::setReg(uint8_t reg, uint32_t val)
{
    gpr[reg] = val;
    gpr[0] = 0;
}

void CPU::loadUpperImmediate(const Instruction &instruction)
{
    uint32_t res = instruction.i.immediate;
    res = res << 16;
    setReg(instruction.i.rt, res);
}

void CPU::storeWord(const Instruction &instruction)
{
    int32_t imm = (int16_t)instruction.i.immediate;
    uint32_t address = getReg(instruction.i.rs) + imm;
    uint32_t value = getReg(instruction.i.rt);

    m_bus->storeWord(address, value);
}

void CPU::storeHalfWord(const Instruction &instruction)
{
    int32_t imm = (int16_t)instruction.i.immediate;
    uint32_t address = getReg(instruction.i.rs) + imm;
    uint16_t value = static_cast<uint16_t>(getReg(instruction.i.rt));

    m_bus->storeHalfWord(address, value);
}

void CPU::storeByte(const Instruction &instruction)
{
    int32_t imm = (int16_t)instruction.i.immediate;
    uint32_t address = getReg(instruction.i.rs) + imm;
    uint8_t value = static_cast<uint8_t>(getReg(instruction.i.rt));

    m_bus->storeByte(address, value);
}

void CPU::shiftLeftLogical(const Instruction &instruction)
{
    uint32_t res = getReg(instruction.r.rt) << instruction.r.shamt;

    setReg(instruction.r.rd, res);
}

void CPU::addImmediateUnsigned(const Instruction &instruction)
{
    uint32_t imm = static_cast<int16_t>(instruction.i.immediate);
    uint32_t val = getReg(instruction.i.rs);
    uint32_t res = val + imm;

    setReg(instruction.i.rt, res);
}

void CPU::substractWordUnsigned(const Instruction &instruction)
{
    uint32_t left = getReg(instruction.r.rs);
    uint32_t right = getReg(instruction.r.rt);
    uint32_t tmp = left - right;

    setReg(instruction.r.rd, tmp);
}

void CPU::substractWord(const Instruction &instruction)
{
    uint32_t left = getReg(instruction.r.rs);
    uint32_t right = getReg(instruction.r.rt);
    uint32_t tmp = left - right;

    if (subOverflow(left, right))
    {
        // Overflow: need to raise exception on the system
        std::cerr << "Substraction overflow! : TODO Raise Exception" << std::endl;
        return;
    }
    setReg(instruction.r.rd, tmp);
}

void CPU::addWord(const Instruction &instruction)
{
    uint32_t left = getReg(instruction.r.rs);
    uint32_t right = getReg(instruction.r.rt);
    uint32_t tmp = left + right;

    if (addOverflow(left, right))
    {
        // Overflow: need to raise exception on the system
        std::cerr << "Addition overflow! : TODO Raise Exception : TODO Raise Exception" << std::endl;
        return;
    }
    setReg(instruction.r.rd, tmp);
}

void CPU::addWordUnsigned(const Instruction &instruction)
{
    uint32_t left = getReg(instruction.r.rs);
    uint32_t right = getReg(instruction.r.rt);
    uint32_t tmp = left + right;

    setReg(instruction.r.rd, tmp);
}

void CPU::addImmediate(const Instruction &instruction)
{
    uint32_t left = getReg(instruction.i.rs);
    uint32_t imm = static_cast<int16_t>(instruction.i.immediate);
    uint32_t tmp = left + imm;

    if (addOverflow(left, imm))
    {
        // Overflow: need to raise exception on the system
        // Do not modify register on overflow
        std::cerr << "Addition overflow! : TODO Raise Exception" << std::endl;
        return;
    }
    setReg(instruction.i.rt, tmp);
}

void CPU::andWord(const Instruction &instruction)
{
    uint32_t left = getReg(instruction.r.rs);
    uint32_t right = getReg(instruction.r.rt);
    uint32_t res = left & right;

    setReg(instruction.r.rd, res);
}

void CPU::orWord(const Instruction &instruction)
{
    uint32_t left = getReg(instruction.r.rs);
    uint32_t right = getReg(instruction.r.rt);
    uint32_t res = left | right;

    setReg(instruction.r.rd, res);
}

void CPU::xorWord(const Instruction &instruction)
{
    uint32_t left = getReg(instruction.r.rs);
    uint32_t right = getReg(instruction.r.rt);
    uint32_t res = left ^ right;

    setReg(instruction.r.rd, res);
}

void CPU::norWord(const Instruction &instruction)
{
    uint32_t left = getReg(instruction.r.rs);
    uint32_t right = getReg(instruction.r.rt);
    uint32_t res = ~(left | right);

    setReg(instruction.r.rd, res);
}

void CPU::andImmediateWord(const Instruction &instruction)
{
    uint32_t res = getReg(instruction.i.rs) & (int32_t)instruction.i.immediate;

    setReg(instruction.i.rt, res);
}

void CPU::orImmediateWord(const Instruction &instruction)
{
    uint32_t res = getReg(instruction.i.rs) | (int32_t)instruction.i.immediate;

    setReg(instruction.i.rt, res);
}

void CPU::xorImmediateWord(const Instruction &instruction)
{
    uint32_t res = getReg(instruction.i.rs) ^ (int32_t)instruction.i.immediate;

    setReg(instruction.i.rt, res);
}

void CPU::loadWord(const Instruction &instruction)
{
    int32_t imm = static_cast<int16_t>(instruction.i.immediate);
    uint32_t address = getReg(instruction.i.rs) + imm;

    if (address % 4 != 0) {
        // Misaligned: need to raise exception on the system
        std::cerr << "Address is misaligned,!" << std::endl;
        return;
    }

    uint32_t value = m_bus->loadWord(address);
    setReg(instruction.i.rt, value);  // No sign extension needed for 32-bit word load
}

void CPU::loadHalfWord(const Instruction &instruction)
{
    int32_t imm = static_cast<int16_t>(instruction.i.immediate);
    uint32_t address = getReg(instruction.i.rs) + imm;

    if (address % 2 != 0) {
        // Misaligned: need to raise exception on the system
        std::cerr << "Address is misaligned,!" << std::endl;
        return;
    }

    int16_t value = static_cast<int16_t>(m_bus->loadHalfWord(address));  // Sign-extend
    setReg(instruction.i.rt, static_cast<int32_t>(value));
}

void CPU::loadHalfWordUnsigned(const Instruction &instruction)
{
    int32_t imm = static_cast<int16_t>(instruction.i.immediate);
    uint32_t address = getReg(instruction.i.rs) + imm;

    if (address % 2 != 0) {
        // Misaligned: need to raise exception on the system
        std::cerr << "Address is misaligned,!" << std::endl;
        return;
    }

    uint16_t value = m_bus->loadHalfWord(address);
    setReg(instruction.i.rt, static_cast<uint32_t>(value));
}

void CPU::loadByte(const Instruction &instruction)
{
    int32_t imm = static_cast<int16_t>(instruction.i.immediate);
    uint32_t address = getReg(instruction.i.rs) + imm;

    int8_t value = static_cast<int8_t>(m_bus->loadByte(address));
    setReg(instruction.i.rt, static_cast<int32_t>(value));
}

void CPU::loadByteUnsigned(const Instruction &instruction)
{
    int32_t imm = static_cast<int16_t>(instruction.i.immediate);
    uint32_t address = getReg(instruction.i.rs) + imm;

    uint8_t value = m_bus->loadByte(address);
    setReg(instruction.i.rt, static_cast<uint32_t>(value));
}

void CPU::loadWordRight(const Instruction &instruction)
{
    int32_t imm = static_cast<int16_t>(instruction.i.immediate);
    uint32_t address = getReg(instruction.i.rs) + imm;
    uint32_t value = m_bus->loadWord(address & ~3);

    uint32_t shift = (address & 3) * 8;
    uint32_t mask = 0xFFFFFFFF >> shift;
    uint32_t right = (value << shift) & mask;

    uint32_t res = (getReg(instruction.i.rt) & ~mask) | right;
    setReg(instruction.i.rt, res);
}

void CPU::loadWordLeft(const Instruction &instruction)
{
    int32_t imm = static_cast<int16_t>(instruction.i.immediate);
    uint32_t address = getReg(instruction.i.rs) + imm;
    uint32_t value = m_bus->loadWord(address & ~3);

    uint32_t shift = (3 - (address & 3)) * 8;
    uint32_t mask = 0xFFFFFFFF << shift;
    uint32_t left = (value >> shift) & mask;

    uint32_t res = (getReg(instruction.i.rt) & ~mask) | left;
    setReg(instruction.i.rt, res);
}

void CPU::storeWordRight(const Instruction &instruction)
{
    int32_t imm = static_cast<int16_t>(instruction.i.immediate);
    uint32_t address = getReg(instruction.i.rs) + imm;
    uint32_t value = getReg(instruction.i.rt);

    uint32_t shift = (address & 3) * 8;
    uint32_t mask = 0xFFFFFFFF >> shift;
    uint32_t oldValue = m_bus->loadWord(address & ~3);

    uint32_t res = (oldValue & ~mask) | ((value << shift) & mask);
    m_bus->storeWord(address & ~3, res);
}

void CPU::storeWordLeft(const Instruction &instruction)
{
    int32_t imm = static_cast<int16_t>(instruction.i.immediate);
    uint32_t address = getReg(instruction.i.rs) + imm;
    uint32_t value = getReg(instruction.i.rt);

    uint32_t shift = (3 - (address & 3)) * 8;
    uint32_t mask = 0xFFFFFFFF << shift;
    uint32_t oldValue = m_bus->loadWord(address & ~3);

    uint32_t res = (oldValue & ~mask) | ((value >> shift) & mask);
    m_bus->storeWord(address & ~3, res);
}

void CPU::setOnLessThan(const Instruction &instruction)
{
    int32_t left = getReg(instruction.r.rs);
    int32_t right = getReg(instruction.r.rt);
    int32_t res = left < right ? 1 : 0;

    setReg(instruction.r.rd, res);
}

void CPU::setOnLessThanUnsigned(const Instruction &instruction)
{
    uint32_t left = getReg(instruction.r.rs);
    uint32_t right = getReg(instruction.r.rt);
    int32_t res = left < right ? 1 : 0;

    setReg(instruction.r.rd, res);
}

void CPU::setOnLessThanImmediate(const Instruction &instruction)
{
    int32_t left = getReg(instruction.i.rs);
    int32_t right = static_cast<int16_t>(instruction.i.immediate);
    int32_t res = left < right ? 1 : 0;

    setReg(instruction.i.rt, res);
}

void CPU::setOnLessThanImmediateUnsigned(const Instruction &instruction)
{
    uint32_t left = getReg(instruction.i.rs);
    uint32_t right = static_cast<uint32_t>(static_cast<int16_t>(instruction.i.immediate));
    int32_t res = left < right ? 1 : 0;

    setReg(instruction.i.rt, res);
}

void CPU::shiftLeftLogicalVariable(const Instruction &instruction)
{
    uint32_t res = getReg(instruction.r.rt) << (getReg(instruction.r.rs) & 0x1F);

    setReg(instruction.r.rd, res);
}

void CPU::shiftRightLogical(const Instruction &instruction)
{
    uint32_t res = getReg(instruction.r.rt) >> instruction.r.shamt;

    setReg(instruction.r.rd, res);
}

void CPU::shiftRightLogicalVariable(const Instruction &instruction)
{
    uint32_t res = getReg(instruction.r.rt) >> (getReg(instruction.r.rs) & 0x1F);

    setReg(instruction.r.rd, res);
}

void CPU::shiftRightArithmetic(const Instruction &instruction)
{
    int32_t value = getReg(instruction.r.rt);
    int32_t res = value >> instruction.r.shamt;

    setReg(instruction.r.rd, res);
}

void CPU::shiftRightArithmeticVariable(const Instruction &instruction)
{
    int32_t value = getReg(instruction.r.rt);
    uint32_t shiftAmount = getReg(instruction.r.rs) & 0x1F;
    uint32_t res =  value >> shiftAmount;

    setReg(instruction.r.rd, res);
}

void CPU::multiply(const Instruction &instruction)
{
    int32_t left = static_cast<int32_t>(getReg(instruction.r.rs));
    int32_t right = static_cast<int32_t>(getReg(instruction.r.rt));

    int64_t res = static_cast<int64_t>(left) * static_cast<int64_t>(right);

    lo = static_cast<int32_t>(res & 0xFFFFFFFF);
    hi = static_cast<int32_t>(res >> 32);
}

void CPU::multiplyUnsigned(const Instruction &instruction)
{
    uint32_t left = static_cast<int32_t>(getReg(instruction.r.rs));
    uint32_t right = static_cast<int32_t>(getReg(instruction.r.rt));

    uint64_t res = static_cast<uint64_t>(left) * static_cast<uint64_t>(right);

    lo = static_cast<uint32_t>(res & 0xFFFFFFFF);
    hi = static_cast<uint32_t>(res >> 32);
}

void CPU::divide(const Instruction &instruction)
{
    int32_t left = static_cast<int32_t>(getReg(instruction.r.rs));
    int32_t right = static_cast<int32_t>(getReg(instruction.r.rt));

    if (right == 0)
    {
        lo = 0;
        hi = left;
    }
    else
    {
        lo = left / right;
        hi = left % right;
    }
}

void CPU::divideUnsigned(const Instruction &instruction)
{
    uint32_t left = getReg(instruction.r.rs);
    uint32_t right = getReg(instruction.r.rt);

    if (right == 0)
    {
        lo = 0;
        hi = left;
    }
    else
    {
        lo = left / right;
        hi = left % right;
    }
}

void CPU::moveFromHi(const Instruction &instruction)
{
    setReg(instruction.r.rd, hi);
}

void CPU::moveFromLo(const Instruction &instruction)
{
    setReg(instruction.r.rd, lo);
}

void CPU::moveToHi(const Instruction &instruction)
{
    hi = getReg(instruction.r.rs);
}

void CPU::moveToLo(const Instruction &instruction)
{
    lo = getReg(instruction.r.rs);
}

void CPU::jump(const Instruction &instruction)
{
    m_branchSlotAddr = (pc & 0xF0000000) | (((uint32_t)instruction.j.address) << 2);
    m_inBranchDelay = true;
}

void CPU::jumpAndLink(const Instruction &instruction)
{
    setReg(31, pc + 8);
    m_branchSlotAddr = (pc & 0xF0000000) | (((uint32_t)instruction.j.address) << 2);
    m_inBranchDelay = true;
}

void CPU::jumpRegister(const Instruction &instruction)
{
    m_branchSlotAddr = getReg(instruction.r.rs);
    m_inBranchDelay = true;
}

void CPU::jumpAndLinkRegister(const Instruction &instruction)
{
    setReg(instruction.r.rd, pc + 8);
    m_branchSlotAddr = getReg(instruction.r.rs);
    m_inBranchDelay = true;
}

void CPU::executeBranch(const Instruction &instruction)
{
    m_branchSlotAddr = pc + 4 + ((int16_t)instruction.i.immediate << 2);
    m_inBranchDelay = true;
}

void CPU::branchOnEqual(const Instruction &instruction)
{
    if (getReg(instruction.i.rs) == getReg(instruction.i.rt))
        pc += 4 + ((int16_t)instruction.i.immediate << 2);
    else
        pc += 4;
}

void CPU::branchOnNotEqual(const Instruction &instruction)
{
    if (getReg(instruction.i.rs) != getReg(instruction.i.rt))
        executeBranch(instruction);
}

void CPU::branchOnLessThanZero(const Instruction &instruction)
{
    if (static_cast<int32_t>(getReg(instruction.i.rs)) < 0)
        executeBranch(instruction);
}

void CPU::branchOnGreaterThanOrEqualToZero(const Instruction &instruction)
{
    if (static_cast<int32_t>(getReg(instruction.i.rs)) >= 0)
        executeBranch(instruction);
}

void CPU::branchOnGreaterThanZero(const Instruction &instruction)
{
    if (static_cast<int32_t>(getReg(instruction.i.rs)) > 0)
        executeBranch(instruction);
}

void CPU::branchOnGreaterThanZeroAndLink(const Instruction &instruction)
{
    if (static_cast<int32_t>(getReg(instruction.i.rs)) > 0)
    {
        setReg(31, pc + 8);
        executeBranch(instruction);
    }
}

void CPU::branchOnLessThanOrEqualToZero(const Instruction &instruction)
{
    if (static_cast<int32_t>(getReg(instruction.i.rs)) <= 0)
        executeBranch(instruction);
}

void CPU::branchOnLessThanZeroAndLink(const Instruction &instruction)
{
    if (static_cast<int32_t>(getReg(instruction.i.rs)) < 0)
    {
        setReg(31, pc + 8);
        pc += 4 + ((int16_t)instruction.i.immediate << 2);
    }
    else
        pc += 4;
}

void CPU::branchOnGreaterThanOrEqualToZeroAndLink(const Instruction &instruction)
{
    if (static_cast<int32_t>(getReg(instruction.i.rs)) >= 0)
    {
        setReg(31, pc + 8);
        pc += 4 + ((int16_t)instruction.i.immediate << 2);
    }
    else
        pc += 4;
}

void CPU::executeCop0(const Instruction &instruction)
{
    auto code = static_cast<CoprocessorOpcodes>(instruction.r.rs);

    switch (code)
    {
    case CoprocessorOpcodes::MTC:
        mtc0(instruction);
        break;
    case CoprocessorOpcodes::MFC:
        mfc0(instruction);
        break;
    default:
        illegalInstruction(instruction);
        break;
    }
}

void CPU::mtc0(const Instruction &instruction)
{
    // mtc# rt,rd       ;cop#datRd = rt ;data regs
    uint8_t regn = instruction.r.rd;
    uint32_t data = gpr[instruction.r.rt];

    m_cop0Reg[regn] = data;
}

void CPU::mfc0(const Instruction &instruction)
{
    uint8_t regn = instruction.r.rd;
    uint32_t data = m_cop0Reg[instruction.r.rt];

    m_registers[regn] = data;
}

void CPU::illegalInstruction(const Instruction &instruction)
{
    fprintf(stderr, "Illegal instruction: ");
    std::cerr << Disassembler::disassemble(pc, instruction) << std::endl;

    // Temporary exit until exception handling is properly implemented
    exit(1);
}
