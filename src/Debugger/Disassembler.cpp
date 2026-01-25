/*
** EPITECH PROJECT, 2024
** rogem
** File description:
** Disassembler
*/

#include "Disassembler.hpp"

#include <fmt/format.h>
#include <spdlog/spdlog.h>

static const char *cpuGprNames[32] = {
    "zero",
    "at",
    "v0", "v1",
    "a0", "a1", "a2", "a3",
    "t0", "t1", "t2", "t3", "t4", "t5", "t6", "t7",
    "s0", "s1", "s2", "s3", "s4", "s5", "s6", "s7",
    "t8", "t9",
    "k0", "k1",
    "gp",
    "sp",
    "fp",
    "ra"
};

static const char *cop0RegNames[16] = {
    "N/A",
    "N/A",
    "N/A",
    "BPC",
    "N/A",
    "BDA",
    "TAR",
    "DCIC",
    "BadA",
    "BDAM",
    "N/A",
    "BPCM",
    "SR",
    "CAUSE",
    "EPC",
    "PRID"
};

Disassembler::Disassembler()
{
    auto mode = static_cast<cs_mode>(CS_MODE_MIPS32 + CS_MODE_BIG_ENDIAN);
    auto status = cs_open(CS_ARCH_MIPS, mode, &m_csHandle);
    if (status != CS_ERR_OK) {
        spdlog::error("Disassembler: Error initializing capstone engine: {}", cs_strerror(status));
    }
}

Disassembler::~Disassembler()
{
    cs_close(&m_csHandle);
}

static void wordToByteArray(uint32_t word, uint8_t *bytes)
{
    for (int i = 0; i < 4; i++) {
        bytes[i] = (word >> ((3 - i) * 8)) & 0xFF;
    }
}

InstructionData Disassembler::disasm(uint32_t inst, uint32_t addr) const
{
    cs_insn *disInst = nullptr;
    size_t count = 0;
    uint8_t bytes[4] = {0};
    InstructionData data;

    wordToByteArray(inst, bytes);
    count = cs_disasm(m_csHandle, bytes, 4, addr, 1, &disInst);

    data.bytes = fmt::format("{:02x} {:02x} {:02x} {:02x}",
                            bytes[0], bytes[1], bytes[2], bytes[3]);
    if (count > 0) {
        data.mnemonic = disInst->mnemonic;
        data.operands = disInst->op_str;
        cs_free(disInst, count);
    }
    return data;
}

std::string Disassembler::cpuRegName(uint8_t reg) const
{
    if (reg < 32) {
        return cpuGprNames[reg];
    }
    return std::string("invalid reg index");
}

static const char *getCop0RegName(uint8_t reg)
{
    if (reg < 16) {
        return cop0RegNames[reg];
    }
    return "invalid cop0 reg index";
}

std::string Disassembler::copRegName(int cop, uint8_t reg) const
{
    if (cop == 0) {
        return getCop0RegName(reg);
    }
    return std::string("invalid coprocessor index");
}
