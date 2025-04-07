#include <gtest/gtest.h>
#include "Utils.h"
#include "BIOS.h"
#include "Bus.h"
#include "CPU.h"

TEST(CpuTest, BEQ_1)
{
    auto bios = BIOS();
    auto bus = Bus(&bios, nullptr);
    CPU cpu(&bus);
    Instruction i;

    uint32_t value = 0xB16B00B5;
    int16_t imm = 0x0004;

    loadImmediate(cpu, 11, value);
    i.i.rs = 11;
    i.i.rt = 11;
    i.i.immediate = imm;
    cpu.branchOnEqual(i);

    EXPECT_EQ(cpu.pc, RESET_VECTOR + 4 + (imm << 2));
}

TEST(CpuTest, BEQ_2)
{
    auto bios = BIOS();
    auto bus = Bus(&bios, nullptr);
    CPU cpu(&bus);
    Instruction i;

    uint32_t value = 0xB16B00B5;
    int16_t imm = 0x0004;

    cpu.pc = RESET_VECTOR;  // Set initial PC

    loadImmediate(cpu, 11, value);
    cpu.gpr[12] = 0;  // Ensure $12 is zero

    i.i.rs = 11;
    i.i.rt = 12;
    i.i.immediate = imm;
    cpu.branchOnEqual(i);

    EXPECT_EQ(cpu.pc, RESET_VECTOR + 4);
}

TEST(CpuTest, BNE_BranchWhenNotEqual)
{
    auto bios = BIOS();
    auto bus = Bus(&bios, nullptr);
    CPU cpu(&bus);
    Instruction i;

    uint8_t srcReg = static_cast<uint8_t>(GprIndex::T0);
    uint8_t targetReg = static_cast<uint8_t>(GprIndex::T1);

    cpu.setReg(srcReg, 11);
    cpu.setReg(targetReg, 12);

    i.i.rs = srcReg;
    i.i.rt = targetReg;
    i.i.immediate = 0x0004;
    cpu.m_branchSlotAddr = 0;

    cpu.branchOnNotEqual(i);
    EXPECT_EQ(cpu.m_branchSlotAddr, RESET_VECTOR + 4 + ((int16_t)i.i.immediate << 2));
    EXPECT_EQ(cpu.m_inBranchDelay, true);  // Branch taken
}

TEST(CpuTest, BNE_NoBranchWhenEqual)
{
    auto bios = BIOS();
    auto bus = Bus(&bios, nullptr);
    CPU cpu(&bus);
    Instruction i;

    uint8_t srcReg = static_cast<uint8_t>(GprIndex::T0);
    uint8_t targetReg = static_cast<uint8_t>(GprIndex::T1);

    cpu.setReg(srcReg, 11);
    cpu.setReg(targetReg, 11);  // Same value

    i.i.rs = srcReg;
    i.i.rt = targetReg;
    i.i.immediate = 0x0004;
    cpu.m_branchSlotAddr = 0;

    cpu.branchOnNotEqual(i);
    EXPECT_EQ(cpu.m_branchSlotAddr, 0);  // No branch
    EXPECT_EQ(cpu.m_inBranchDelay, false);  // No branch taken
}

TEST(CpuTest, BNE_NoBranch_ZeroRegistersEqual)
{
    auto bios = BIOS();
    auto bus = Bus(&bios, nullptr);
    CPU cpu(&bus);
    Instruction i;

    uint8_t srcReg = static_cast<uint8_t>(GprIndex::T0);
    uint8_t targetReg = static_cast<uint8_t>(GprIndex::T1);

    cpu.setReg(srcReg, 0);
    cpu.setReg(targetReg, 0);  // Same value

    i.i.rs = srcReg;
    i.i.rt = targetReg;
    i.i.immediate = 0x0004;
    cpu.m_branchSlotAddr = 0;

    cpu.branchOnNotEqual(i);
    EXPECT_EQ(cpu.m_branchSlotAddr, 0);  // No branch
    EXPECT_EQ(cpu.m_inBranchDelay, false);  // No branch taken
}

TEST(CpuTest, BNE_MaxPositiveOffset) {
    auto bios = BIOS();
    auto bus = Bus(&bios, nullptr);
    CPU cpu(&bus);
    Instruction i;

    uint8_t srcReg = static_cast<uint8_t>(GprIndex::T0);
    uint8_t targetReg = static_cast<uint8_t>(GprIndex::T1);

    cpu.setReg(srcReg, 11);
    cpu.setReg(targetReg, 12);

    i.i.rs = srcReg;
    i.i.rt = targetReg;
    i.i.immediate = 0x7FFF;  // Maximum positive offset
    cpu.m_branchSlotAddr = 0;

    cpu.branchOnNotEqual(i);
    EXPECT_EQ(cpu.m_branchSlotAddr, RESET_VECTOR + 4 + ((int16_t)i.i.immediate << 2));
    EXPECT_EQ(cpu.m_inBranchDelay, true);  // Branch taken
}
