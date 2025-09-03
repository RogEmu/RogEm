/*
** EPITECH PROJECT, 2024
** RogEm
** File description:
** CPU
*/

#include "CPU.h"
#include <iostream>
#include <cstring>
#include <spdlog/spdlog.h>

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
    m_bus(bus)
{
    reset();
}

void CPU::step()
{
    Instruction instruction = fetchInstruction();
    m_nextPc = m_pc;
    m_inBranchDelay = m_nextIsBranchDelay;
    if (m_nextIsBranchDelay)
    {
        m_nextPc = m_branchSlotAddr;
        m_nextIsBranchDelay = false;
    }
    else
    {
        m_nextPc += 4;
    }
    if (m_jumpToUnaligned) {
        triggerException(ExceptionType::AddressErrorLoad);
        m_jumpToUnaligned = false;
    }
    if (interruptPending() && shouldExecuteInterrupt()) {
        triggerException(ExceptionType::Interrupt);
        return;
    }
    executeInstruction(instruction);
    handleLoadDelay();
    m_pc = m_nextPc;
    m_inBranchDelay = false;
}

void CPU::reset()
{
    m_nextIsBranchDelay = false;
    m_inBranchDelay = false;
    m_jumpToUnaligned = false;
    m_pc = RESET_VECTOR;
    std::memset(m_loadDelaySlots, 0, sizeof(m_loadDelaySlots));
    std::memset(m_gpr, 0, NB_GPR * sizeof(m_gpr[0]));
    m_cop0.reset();
    m_isTtyOutput = false;
}

void CPU::setTtyOutputFlag(bool ttyOutput)
{
    m_isTtyOutput = ttyOutput;
}

bool CPU::getTtyOutputFlag()
{
    return m_isTtyOutput;
}

std::string CPU::getTtyOutput()
{
    std::string output = m_ttyOutput;
    m_ttyOutput.clear();
    m_isTtyOutput = false;
    return output;
}

void CPU::checkTtyOutput()
{
    uint32_t t1 = getReg(CpuReg::T1);
    uint32_t programCounter = getReg(CpuReg::PC);
    if ((programCounter == 0xA0 && t1 == 0x3C) || (programCounter == 0xB0 && t1 == 0x3D))
    {
        char c = (static_cast<char>(getReg(CpuReg::A0) & 0xFF));
        switch (c)
        {
            case '\t':
                m_ttyOutput.append(8 - (m_ttyOutput.size() % 8), ' ');
                break;
            case '\0':
            case '\n':
                m_isTtyOutput = true;
                break;
            case '\b':
                if (!m_ttyOutput.empty())
                    m_ttyOutput.pop_back();
                break;
            case '\a':
                m_ttyOutput += "[BELL]";
                break;
            default:
                m_ttyOutput += static_cast<char>(c);
                break;
        }
    }
}

Instruction CPU::fetchInstruction()
{
    uint32_t instruction = m_bus->loadWord(m_pc);
    checkTtyOutput();
    return Instruction{.raw=instruction};
}

void CPU::executeInstruction(const Instruction &instruction)
{
    if (instruction.r.opcode & 0x10) {
        executeCoprocessor(instruction);
        return;
    }
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
    case PrimaryOpCode::BGTZ:
        branchOnGreaterThanZero(instruction);
        break;
    case PrimaryOpCode::SLTI:
        setOnLessThanImmediate(instruction);
        break;
    case PrimaryOpCode::SLTIU:
        setOnLessThanImmediateUnsigned(instruction);
        break;
    case PrimaryOpCode::SPECIAL:
        specialInstruction(instruction);
        break;
    case PrimaryOpCode::BCONDZ:
        branchOnConditionZero(instruction);
        break;
    case PrimaryOpCode::BNE:
        branchOnNotEqual(instruction);
        break;
    case PrimaryOpCode::BEQ:
        branchOnEqual(instruction);
        break;
    case PrimaryOpCode::BLEZ:
        branchOnLessThanOrEqualToZero(instruction);
        break;
    default:
        illegalInstruction(instruction);
        break;
    }
}

void CPU::handleLoadDelay()
{
    if (m_loadDelaySlots[0].pending) {
        setReg(m_loadDelaySlots[0].reg, m_loadDelaySlots[0].value);
        m_loadDelaySlots[0].pending = false;
    }
    if (m_loadDelaySlots[1].pending) {
        m_loadDelaySlots[0] = m_loadDelaySlots[1];
        m_loadDelaySlots[1].pending = false;
    }
}

bool CPU::shouldExecuteInterrupt()
{
    return (m_cop0.mfc(12) & 0x401);
}

void CPU::branchOnConditionZero(const Instruction &instruction)
{
    if ((instruction.i.rt & 0x1E) == 0x10) {
        if (instruction.i.rt & 1) {
            branchOnGreaterThanOrEqualToZeroAndLink(instruction);
        } else {
            branchOnLessThanZeroAndLink(instruction);
        }
    } else {
        if (instruction.i.rt & 1) {
            branchOnGreaterThanOrEqualToZero(instruction);
        } else {
            branchOnLessThanZero(instruction);
        }
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
    case SecondaryOpCode::DIV:
        divide(instruction);
        break;
    case SecondaryOpCode::DIVU:
        divideUnsigned(instruction);
        break;
    case SecondaryOpCode::MFHI:
        moveFromHi(instruction);
        break;
    case SecondaryOpCode::MFLO:
        moveFromLo(instruction);
        break;
    case SecondaryOpCode::MTHI:
        moveToHi(instruction);
        break;
    case SecondaryOpCode::MTLO:
        moveToLo(instruction);
        break;
    case SecondaryOpCode::JR:
        jumpRegister(instruction);
        break;
    case SecondaryOpCode::JALR:
        jumpAndLinkRegister(instruction);
        break;
    case SecondaryOpCode::SYSCALL:
        executeSyscall(instruction);
        break;
    case SecondaryOpCode::BREAK:
        executeBreak(instruction);
        break;
    default:
        illegalInstruction(instruction);
        break;
    }
}

uint32_t CPU::getReg(CpuReg reg) const
{
    switch (reg)
    {
    case CpuReg::PC:
        return m_pc;
    case CpuReg::HI:
        return m_hi;
    case CpuReg::LO:
        return m_lo;
    default:
        return m_gpr[static_cast<uint8_t>(reg)];
    }
}

void CPU::setReg(CpuReg reg, uint32_t val)
{
    switch (reg)
    {
    case CpuReg::PC:
        m_pc = val;
        break;
    case CpuReg::HI:
        m_hi = val;
        break;
    case CpuReg::LO:
        m_lo = val;
        break;
    default:
        m_gpr[static_cast<uint8_t>(reg)] = val;
        m_gpr[0] = 0;
    }
}

uint32_t CPU::getCop0Reg(uint8_t reg)
{
    return m_cop0.mfc(reg);
}

void CPU::setCop0Reg(uint8_t reg, uint32_t val)
{
    m_cop0.mtc(reg, val);
}

void CPU::setInterruptPending(bool pending)
{
    uint32_t cause = m_cop0.mfc(13);
    if (pending) {
        cause |= 0x400;
    } else {
        cause &= ~0x400;
    }
    m_cop0.mtc(13, cause);
}

bool CPU::interruptPending()
{
    bool pending = m_cop0.mfc(13) & 0x400;
    return pending;
}

void CPU::loadUpperImmediate(const Instruction &instruction)
{
    uint32_t res = instruction.i.immediate;
    res = res << 16;
    setReg(static_cast<CpuReg>(instruction.i.rt), res);
}

void CPU::storeWord(const Instruction &instruction)
{
    int32_t imm = (int16_t)instruction.i.immediate;
    uint32_t address = getReg(static_cast<CpuReg>(instruction.i.rs)) + imm;

    if (address % 4 != 0) {
        triggerException(ExceptionType::AddressErrorStore);
        return;
    }

    auto sr = getCop0Reg(static_cast<uint8_t>(CP0Reg::SR));
    // Check if cache is isolated
    // TODO: Implement the REAL cache
    if (sr & 0x00010000)
    {
        return;
    }
    uint32_t value = getReg(static_cast<CpuReg>(instruction.i.rt));

    m_bus->storeWord(address, value);
}

void CPU::storeHalfWord(const Instruction &instruction)
{
    int32_t imm = (int16_t)instruction.i.immediate;
    uint32_t address = getReg(static_cast<CpuReg>(instruction.i.rs)) + imm;

    if (address % 2 != 0) {
        triggerException(ExceptionType::AddressErrorStore);
        return;
    }

    auto sr = getCop0Reg(static_cast<uint8_t>(CP0Reg::SR));
    // Check if cache is isolated
    // TODO: Implement the REAL cache
    if (sr & 0x00010000)
    {
        return;
    }

    uint16_t value = static_cast<uint16_t>(getReg(static_cast<CpuReg>(instruction.i.rt)));

    m_bus->storeHalfWord(address, value);
}

void CPU::storeByte(const Instruction &instruction)
{
    auto sr = getCop0Reg(static_cast<uint8_t>(CP0Reg::SR));

    // Check if cache is isolated
    // TODO: Implement the REAL cache
    if (sr & 0x00010000)
    {
        return;
    }

    int32_t imm = (int16_t)instruction.i.immediate;
    uint32_t address = getReg(static_cast<CpuReg>(instruction.i.rs)) + imm;
    uint8_t value = static_cast<uint8_t>(getReg(static_cast<CpuReg>(instruction.i.rt)));

    m_bus->storeByte(address, value);
}

void CPU::shiftLeftLogical(const Instruction &instruction)
{
    uint32_t res = getReg(static_cast<CpuReg>(instruction.r.rt)) << instruction.r.shamt;

    setReg(static_cast<CpuReg>(instruction.r.rd), res);
}

void CPU::addImmediateUnsigned(const Instruction &instruction)
{
    uint32_t imm = static_cast<int16_t>(instruction.i.immediate);
    uint32_t val = getReg(static_cast<CpuReg>(instruction.i.rs));
    uint32_t res = val + imm;

    setReg(static_cast<CpuReg>(instruction.i.rt), res);
}

void CPU::substractWordUnsigned(const Instruction &instruction)
{
    uint32_t left = getReg(static_cast<CpuReg>(instruction.r.rs));
    uint32_t right = getReg(static_cast<CpuReg>(instruction.r.rt));
    uint32_t tmp = left - right;

    setReg(static_cast<CpuReg>(instruction.r.rd), tmp);
}

void CPU::substractWord(const Instruction &instruction)
{
    uint32_t left = getReg(static_cast<CpuReg>(instruction.r.rs));
    uint32_t right = getReg(static_cast<CpuReg>(instruction.r.rt));
    uint32_t tmp = left - right;

    if (subOverflow(left, right))
    {
        triggerException(ExceptionType::Overflow);
        return;
    }
    setReg(static_cast<CpuReg>(instruction.r.rd), tmp);
}

void CPU::addWord(const Instruction &instruction)
{
    uint32_t left = getReg(static_cast<CpuReg>(instruction.r.rs));
    uint32_t right = getReg(static_cast<CpuReg>(instruction.r.rt));
    uint32_t tmp = left + right;

    if (addOverflow(left, right))
    {
        triggerException(ExceptionType::Overflow);
        return;
    }
    setReg(static_cast<CpuReg>(instruction.r.rd), tmp);
}

void CPU::addWordUnsigned(const Instruction &instruction)
{
    uint32_t left = getReg(static_cast<CpuReg>(instruction.r.rs));
    uint32_t right = getReg(static_cast<CpuReg>(instruction.r.rt));
    uint32_t tmp = left + right;

    setReg(static_cast<CpuReg>(instruction.r.rd), tmp);
}

void CPU::addImmediate(const Instruction &instruction)
{
    uint32_t left = getReg(static_cast<CpuReg>(instruction.i.rs));
    uint32_t imm = static_cast<int16_t>(instruction.i.immediate);
    uint32_t tmp = left + imm;

    if (addOverflow(left, imm))
    {
        triggerException(ExceptionType::Overflow);
        return;
    }
    setReg(static_cast<CpuReg>(instruction.i.rt), tmp);
}

void CPU::andWord(const Instruction &instruction)
{
    uint32_t left = getReg(static_cast<CpuReg>(instruction.r.rs));
    uint32_t right = getReg(static_cast<CpuReg>(instruction.r.rt));
    uint32_t res = left & right;

    setReg(static_cast<CpuReg>(instruction.r.rd), res);
}

void CPU::orWord(const Instruction &instruction)
{
    uint32_t left = getReg(static_cast<CpuReg>(instruction.r.rs));
    uint32_t right = getReg(static_cast<CpuReg>(instruction.r.rt));
    uint32_t res = left | right;

    setReg(static_cast<CpuReg>(instruction.r.rd), res);
}

void CPU::xorWord(const Instruction &instruction)
{
    uint32_t left = getReg(static_cast<CpuReg>(instruction.r.rs));
    uint32_t right = getReg(static_cast<CpuReg>(instruction.r.rt));
    uint32_t res = left ^ right;

    setReg(static_cast<CpuReg>(instruction.r.rd), res);
}

void CPU::norWord(const Instruction &instruction)
{
    uint32_t left = getReg(static_cast<CpuReg>(instruction.r.rs));
    uint32_t right = getReg(static_cast<CpuReg>(instruction.r.rt));
    uint32_t res = ~(left | right);

    setReg(static_cast<CpuReg>(instruction.r.rd), res);
}

void CPU::andImmediateWord(const Instruction &instruction)
{
    uint32_t res = getReg(static_cast<CpuReg>(instruction.i.rs)) & (int32_t)instruction.i.immediate;

    setReg(static_cast<CpuReg>(instruction.i.rt), res);
}

void CPU::orImmediateWord(const Instruction &instruction)
{
    uint32_t res = getReg(static_cast<CpuReg>(instruction.i.rs)) | (int32_t)instruction.i.immediate;

    setReg(static_cast<CpuReg>(instruction.i.rt), res);
}

void CPU::xorImmediateWord(const Instruction &instruction)
{
    uint32_t res = getReg(static_cast<CpuReg>(instruction.i.rs)) ^ (int32_t)instruction.i.immediate;

    setReg(static_cast<CpuReg>(instruction.i.rt), res);
}

void CPU::loadWithDelay(CpuReg reg, uint32_t value)
{
    if (m_loadDelaySlots[0].reg == reg) {
        m_loadDelaySlots[0].pending = false;
    }
    m_loadDelaySlots[1].value = value;
    m_loadDelaySlots[1].reg = reg;
    m_loadDelaySlots[1].pending = true;
}

void CPU::loadWord(const Instruction &instruction)
{
    int32_t imm = static_cast<int16_t>(instruction.i.immediate);
    uint32_t address = getReg(static_cast<CpuReg>(instruction.i.rs)) + imm;

    if (address % 4 != 0) {
        triggerException(ExceptionType::AddressErrorLoad);
        return;
    }

    uint32_t value = m_bus->loadWord(address);
    loadWithDelay(static_cast<CpuReg>(instruction.i.rt), value);
}

void CPU::loadHalfWord(const Instruction &instruction)
{
    int32_t imm = static_cast<int16_t>(instruction.i.immediate);
    uint32_t address = getReg(static_cast<CpuReg>(instruction.i.rs)) + imm;

    if (address % 2 != 0) {
        triggerException(ExceptionType::AddressErrorLoad);
        return;
    }

    int16_t value = static_cast<int16_t>(m_bus->loadHalfWord(address));
    loadWithDelay(static_cast<CpuReg>(instruction.i.rt), value);
}

void CPU::loadHalfWordUnsigned(const Instruction &instruction)
{
    int32_t imm = static_cast<int16_t>(instruction.i.immediate);
    uint32_t address = getReg(static_cast<CpuReg>(instruction.i.rs)) + imm;

    if (address % 2 != 0) {
        triggerException(ExceptionType::AddressErrorLoad);
        return;
    }

    uint16_t value = m_bus->loadHalfWord(address);
    loadWithDelay(static_cast<CpuReg>(instruction.i.rt), value);
}

void CPU::loadByte(const Instruction &instruction)
{
    int32_t imm = static_cast<int16_t>(instruction.i.immediate);
    uint32_t address = getReg(static_cast<CpuReg>(instruction.i.rs)) + imm;

    int8_t value = static_cast<int8_t>(m_bus->loadByte(address));
    loadWithDelay(static_cast<CpuReg>(instruction.i.rt), value);
}

void CPU::loadByteUnsigned(const Instruction &instruction)
{
    int32_t imm = static_cast<int16_t>(instruction.i.immediate);
    uint32_t address = getReg(static_cast<CpuReg>(instruction.i.rs)) + imm;

    uint8_t value = m_bus->loadByte(address);
    loadWithDelay(static_cast<CpuReg>(instruction.i.rt), value);
}

void CPU::loadWordRight(const Instruction &instruction)
{
    int32_t offset = static_cast<int16_t>(instruction.i.immediate);
    uint32_t address = getReg(static_cast<CpuReg>(instruction.i.rs)) + offset;

    uint32_t loadedWord = m_bus->loadWord(address & ~3);
    uint32_t shift = (address & 3) * 8;
    uint32_t mask = 0xFFFFFFFF << shift;

    uint32_t currentRegValue = 0;
    CpuReg targetReg = static_cast<CpuReg>(instruction.i.rt);

    if (m_loadDelaySlots[0].pending && m_loadDelaySlots[0].reg == targetReg) {
        currentRegValue = m_loadDelaySlots[0].value;
    } else {
        currentRegValue = getReg(targetReg);
    }

    uint32_t loadedSection = (loadedWord & mask) >> shift;
    uint32_t result = (currentRegValue & ~(mask >> shift)) | loadedSection;
    loadWithDelay(targetReg, result);
}

void CPU::loadWordLeft(const Instruction &instruction)
{
    int32_t offset = static_cast<int16_t>(instruction.i.immediate);
    uint32_t address = getReg(static_cast<CpuReg>(instruction.i.rs)) + offset;

    uint32_t loadedWord = m_bus->loadWord(address & ~3);
    uint32_t shift = (3 - (address & 3)) * 8;
    uint32_t mask = 0xFFFFFFFF >> shift;

    uint32_t currentRegValue = 0;
    CpuReg targetReg = static_cast<CpuReg>(instruction.i.rt);

    if (m_loadDelaySlots[0].pending && m_loadDelaySlots[0].reg == targetReg) {
        currentRegValue = m_loadDelaySlots[0].value;
    } else {
        currentRegValue = getReg(targetReg);
    }

    uint32_t loadedSection = (loadedWord & mask) << shift;
    uint32_t result = loadedSection | (currentRegValue & ~(mask << shift));
    loadWithDelay(targetReg, result);
}


void CPU::storeWordRight(const Instruction &instruction)
{
    int32_t offset = static_cast<int16_t>(instruction.i.immediate);
    uint32_t address = getReg(static_cast<CpuReg>(instruction.i.rs)) + offset;
    uint32_t storedWord = getReg(static_cast<CpuReg>(instruction.i.rt));

    uint32_t currentWord = m_bus->loadWord(address & ~3);
    uint32_t shift = (address & 3) * 8;
    uint32_t mask = 0xFFFFFFFF >> shift;

    uint32_t result = (currentWord & ~(mask << shift)) | ((storedWord & mask) << shift);
    m_bus->storeWord(address & ~3, result);
}

void CPU::storeWordLeft(const Instruction &instruction)
{
    int32_t offset = static_cast<int16_t>(instruction.i.immediate);
    uint32_t address = getReg(static_cast<CpuReg>(instruction.i.rs)) + offset;
    uint32_t storedWord = getReg(static_cast<CpuReg>(instruction.i.rt));

    uint32_t shift = (3 - (address & 3)) * 8;
    uint32_t mask = 0xFFFFFFFF << shift;
    uint32_t currentWord = m_bus->loadWord(address & ~3);

    uint32_t result = (currentWord & ~(mask >> shift)) | ((storedWord & mask) >> shift);
    m_bus->storeWord(address & ~3, result);
}

void CPU::setOnLessThan(const Instruction &instruction)
{
    int32_t left = getReg(static_cast<CpuReg>(instruction.r.rs));
    int32_t right = getReg(static_cast<CpuReg>(instruction.r.rt));
    int32_t res = left < right ? 1 : 0;

    setReg(static_cast<CpuReg>(instruction.r.rd), res);
}

void CPU::setOnLessThanUnsigned(const Instruction &instruction)
{
    uint32_t left = getReg(static_cast<CpuReg>(instruction.r.rs));
    uint32_t right = getReg(static_cast<CpuReg>(instruction.r.rt));
    uint32_t res = left < right ? 1 : 0;

    setReg(static_cast<CpuReg>(instruction.r.rd), res);
}

void CPU::setOnLessThanImmediate(const Instruction &instruction)
{
    int32_t left = getReg(static_cast<CpuReg>(instruction.i.rs));
    int32_t right = static_cast<int16_t>(instruction.i.immediate);
    int32_t res = left < right ? 1 : 0;

    setReg(static_cast<CpuReg>(instruction.i.rt), res);
}

void CPU::setOnLessThanImmediateUnsigned(const Instruction &instruction)
{
    uint32_t left = getReg(static_cast<CpuReg>(instruction.i.rs));
    uint32_t right = static_cast<int16_t>(instruction.i.immediate);
    uint32_t res = left < right ? 1 : 0;

    setReg(static_cast<CpuReg>(instruction.i.rt), res);
}

void CPU::shiftLeftLogicalVariable(const Instruction &instruction)
{
    uint8_t shiftAmount = getReg(static_cast<CpuReg>(instruction.r.rs)) & 0x1F;
    uint32_t value = getReg(static_cast<CpuReg>(instruction.r.rt));
    uint32_t res = value << shiftAmount;

    setReg(static_cast<CpuReg>(instruction.r.rd), res);
}

void CPU::shiftRightLogical(const Instruction &instruction)
{
    uint32_t value = getReg(static_cast<CpuReg>(instruction.r.rt));
    uint32_t res = value >> instruction.r.shamt;

    setReg(static_cast<CpuReg>(instruction.r.rd), res);
}

void CPU::shiftRightLogicalVariable(const Instruction &instruction)
{
    uint32_t value = getReg(static_cast<CpuReg>(instruction.r.rt));
    uint8_t shiftAmount = getReg(static_cast<CpuReg>(instruction.r.rs)) & 0x1F;
    uint32_t res =  value >> shiftAmount;

    setReg(static_cast<CpuReg>(instruction.r.rd), res);
}

void CPU::shiftRightArithmetic(const Instruction &instruction)
{
    int32_t value = getReg(static_cast<CpuReg>(instruction.r.rt));
    int32_t res = value >> instruction.r.shamt;

    setReg(static_cast<CpuReg>(instruction.r.rd), res);
}

void CPU::shiftRightArithmeticVariable(const Instruction &instruction)
{
    int32_t value = getReg(static_cast<CpuReg>(instruction.r.rt));
    uint32_t shiftAmount = getReg(static_cast<CpuReg>(instruction.r.rs)) & 0x1F;
    uint32_t res =  value >> shiftAmount;

    setReg(static_cast<CpuReg>(instruction.r.rd), res);
}

void CPU::multiply(const Instruction &instruction)
{
    int32_t left = static_cast<int32_t>(getReg(static_cast<CpuReg>(instruction.r.rs)));
    int32_t right = static_cast<int32_t>(getReg(static_cast<CpuReg>(instruction.r.rt)));

    int64_t res = static_cast<int64_t>(left) * static_cast<int64_t>(right);

    m_lo = static_cast<int32_t>(res & 0xFFFFFFFF);
    m_hi = static_cast<int32_t>(res >> 32);
}

void CPU::multiplyUnsigned(const Instruction &instruction)
{
    uint32_t left = getReg(static_cast<CpuReg>(instruction.r.rs));
    uint32_t right = getReg(static_cast<CpuReg>(instruction.r.rt));

    uint64_t res = static_cast<uint64_t>(left) * static_cast<uint64_t>(right);

    m_lo = static_cast<uint32_t>(res & 0xFFFFFFFF);
    m_hi = static_cast<uint32_t>(res >> 32);
}

void CPU::divide(const Instruction &instruction)
{
    int32_t left = static_cast<int32_t>(getReg(static_cast<CpuReg>(instruction.r.rs)));
    int32_t right = static_cast<int32_t>(getReg(static_cast<CpuReg>(instruction.r.rt)));

    if (right == 0)
    {
        m_lo = left >= 0 ? -1 : 1;
        m_hi = left;
    }
    else if (left == INT32_MIN && right == -1)
    {
        m_lo = static_cast<uint32_t>(INT32_MIN);
        m_hi = 0;
    }
    else
    {
        m_lo = left / right;
        m_hi = left % right;
    }
}

void CPU::divideUnsigned(const Instruction &instruction)
{
    uint32_t left = getReg(static_cast<CpuReg>(instruction.r.rs));
    uint32_t right = getReg(static_cast<CpuReg>(instruction.r.rt));

    if (right == 0)
    {
        m_lo = UINT32_MAX;
        m_hi = left;
    }
    else
    {
        m_lo = left / right;
        m_hi = left % right;
    }
}

void CPU::moveFromHi(const Instruction &instruction)
{
    setReg(static_cast<CpuReg>(instruction.r.rd), m_hi);
}

void CPU::moveFromLo(const Instruction &instruction)
{
    setReg(static_cast<CpuReg>(instruction.r.rd), m_lo);
}

void CPU::moveToHi(const Instruction &instruction)
{
    m_hi = getReg(static_cast<CpuReg>(instruction.r.rs));
}

void CPU::moveToLo(const Instruction &instruction)
{
    m_lo = getReg(static_cast<CpuReg>(instruction.r.rs));
}

void CPU::jump(const Instruction &instruction)
{
    m_branchSlotAddr = (m_pc & 0xF0000000) | (((uint32_t)instruction.j.address) << 2);
    m_nextIsBranchDelay = true;
}

void CPU::jumpAndLink(const Instruction &instruction)
{
    setReg(CpuReg::RA, m_pc + 8);
    jump(instruction);
}

void CPU::jumpRegister(const Instruction &instruction)
{
    uint32_t targetAddress = getReg(static_cast<CpuReg>(instruction.r.rs));
    if (targetAddress % 4 != 0) {
        m_jumpToUnaligned = true;
        m_badVarAddr = m_pc;
    }
    m_branchSlotAddr = targetAddress;
    m_nextIsBranchDelay = true;
}

void CPU::jumpAndLinkRegister(const Instruction &instruction)
{
    jumpRegister(instruction);
    setReg(static_cast<CpuReg>(instruction.r.rd), m_pc + 8);
}

void CPU::executeBranch(const Instruction &instruction)
{
    m_branchSlotAddr = m_pc + 4 + ((int16_t)instruction.i.immediate << 2);
    m_nextIsBranchDelay = true;
}

void CPU::branchOnEqual(const Instruction &instruction)
{
    if (getReg(static_cast<CpuReg>(instruction.i.rs)) == getReg(static_cast<CpuReg>(instruction.i.rt)))
        executeBranch(instruction);
}

void CPU::branchOnNotEqual(const Instruction &instruction)
{
    if (getReg(static_cast<CpuReg>(instruction.i.rs)) != getReg(static_cast<CpuReg>(instruction.i.rt)))
        executeBranch(instruction);
}

void CPU::branchOnLessThanZero(const Instruction &instruction)
{
    if (static_cast<int32_t>(getReg(static_cast<CpuReg>(instruction.i.rs))) < 0)
        executeBranch(instruction);
}

void CPU::branchOnGreaterThanOrEqualToZero(const Instruction &instruction)
{
    if (static_cast<int32_t>(getReg(static_cast<CpuReg>(instruction.i.rs))) >= 0)
        executeBranch(instruction);
}

void CPU::branchOnGreaterThanZero(const Instruction &instruction)
{
    if (static_cast<int32_t>(getReg(static_cast<CpuReg>(instruction.i.rs))) > 0)
        executeBranch(instruction);
}

void CPU::branchOnLessThanOrEqualToZero(const Instruction &instruction)
{
    if (static_cast<int32_t>(getReg(static_cast<CpuReg>(instruction.i.rs))) <= 0)
        executeBranch(instruction);
}

void CPU::branchOnLessThanZeroAndLink(const Instruction &instruction)
{
    if (static_cast<int32_t>(getReg(static_cast<CpuReg>(instruction.i.rs))) < 0)
    {
        executeBranch(instruction);
    }
    setReg(CpuReg::RA, m_pc + 8);
}

void CPU::branchOnGreaterThanOrEqualToZeroAndLink(const Instruction &instruction)
{
    if (static_cast<int32_t>(getReg(static_cast<CpuReg>(instruction.i.rs))) >= 0)
    {
        executeBranch(instruction);
    }
    setReg(CpuReg::RA, m_pc + 8);
}

void CPU::executeCoprocessor(const Instruction &instruction)
{
    // Obsolete, need to be redone to match specific processors
    auto code = static_cast<CoprocessorOpcode>(instruction.r.rs);

    switch (code)
    {
    case CoprocessorOpcode::MTC:
        mtc0(instruction);
        break;
    case CoprocessorOpcode::MFC:
        mfc0(instruction);
        break;
    default:
        if (instruction.r.rs == 0x10 && instruction.r.funct == 0x10)
        {
            returnFromException(instruction);
            break;
        }
        illegalInstruction(instruction);
        break;
    }
}

void CPU::mtc0(const Instruction &instruction)
{
    uint8_t reg = instruction.r.rd;
    uint32_t data = getReg(static_cast<CpuReg>(instruction.r.rt));

    setCop0Reg(reg, data);
}

void CPU::mfc0(const Instruction &instruction)
{
    CpuReg reg = static_cast<CpuReg>(instruction.r.rt);
    uint32_t data = getCop0Reg(instruction.r.rd);

    setReg(reg, data);
}

void CPU::executeSyscall(const Instruction &instruction)
{
    (void)instruction;
    triggerException(ExceptionType::Syscall);
}

void CPU::executeBreak(const Instruction &instruction)
{
    (void)instruction;
    triggerException(ExceptionType::Breakpoint);
}

void CPU::returnFromException(const Instruction &instruction)
{
    (void)instruction;
    uint32_t status = getCop0Reg(static_cast<uint8_t>(CP0Reg::SR));
    uint8_t stack = (status >> 2) & 0b1111;
    status &= ~0b1111;
    status |= stack;
    setCop0Reg(static_cast<uint8_t>(CP0Reg::SR), status);
}

void CPU::triggerException(ExceptionType exception)
{
    // Set cause register with exception type and branch delay
    uint32_t cause = 0;

    cause |= ((uint32_t)m_inBranchDelay) << 31;
    cause |= static_cast<uint32_t>(exception) << 2;
    setCop0Reg(static_cast<uint8_t>(CP0Reg::CAUSE), cause);

    uint32_t returnAddr = m_pc - (4 * (uint32_t)m_inBranchDelay);
    setCop0Reg(static_cast<uint8_t>(CP0Reg::EPC), returnAddr);

    uint32_t status = getCop0Reg(static_cast<uint8_t>(CP0Reg::SR));
    uint8_t stack = (status << 2) & 0b111111;
    status &= ~0b111111;
    status |= stack;
    setCop0Reg(static_cast<uint8_t>(CP0Reg::SR), status);

    uint32_t handlerAddr = 0;

    switch (exception)
    {
    case ExceptionType::Breakpoint:
        handlerAddr = static_cast<uint32_t>(ExceptionVector::COP0_BRK);
        break;
    case ExceptionType::AddressErrorLoad:
    case ExceptionType::AddressErrorStore:
        setCop0Reg(static_cast<uint8_t>(CP0Reg::BadVaddr), m_badVarAddr);
        handlerAddr = static_cast<uint32_t>(ExceptionVector::GENERAL);
        break;
    default:
        handlerAddr = static_cast<uint32_t>(ExceptionVector::GENERAL);
        break;
    }
    m_nextPc = handlerAddr;
}

void CPU::illegalInstruction(const Instruction &instruction)
{
    spdlog::error("Illegal instruction: 0x{:08X}", instruction.raw);

    triggerException(ExceptionType::RI);
}
