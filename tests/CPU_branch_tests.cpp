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

TEST(CpuTest, BNE_BranchWhenNotEqual) {
    auto bios = BIOS();
    auto bus = Bus(&bios, nullptr);
    CPU cpu(&bus);
    Instruction i;

    cpu.pc = RESET_VECTOR;
    cpu.gpr[1] = 0x1234;
    cpu.gpr[2] = 0x5678;

    i.i.rs = 1;
    i.i.rt = 2;
    i.i.immediate = 0x0004;

    cpu.branchOnNotEqual(i);
    EXPECT_EQ(cpu.m_branchSlotAddr, RESET_VECTOR + 4 + ((int16_t)i.i.immediate << 2));
}

TEST(CpuTest, BNE_NoBranchWhenEqual) {
    auto bios = BIOS();
    auto bus = Bus(&bios, nullptr);
    CPU cpu(&bus);
    Instruction i;

    cpu.pc = RESET_VECTOR;
    cpu.gpr[3] = 0xDEAD;
    cpu.gpr[4] = 0xDEAD;

    i.i.rs = 3;
    i.i.rt = 4;
    i.i.immediate = 0x0004;

    cpu.branchOnNotEqual(i);
    EXPECT_EQ(cpu.pc + 4, RESET_VECTOR + 4);  // No branch
}

TEST(CpuTest, BNE_NoBranch_ZeroRegistersEqual) {
    auto bios = BIOS();
    auto bus = Bus(&bios, nullptr);
    CPU cpu(&bus);
    Instruction i;

    cpu.pc = RESET_VECTOR;
    cpu.gpr[0] = 0;  // $zero
    cpu.gpr[1] = 0;

    i.i.rs = 0;
    i.i.rt = 1;
    i.i.immediate = 2;

    cpu.branchOnNotEqual(i);
    EXPECT_EQ(cpu.pc + 4, RESET_VECTOR + 4);  // No branch
}

TEST(CpuTest, BNE_MaxPositiveOffset) {
    auto bios = BIOS();
    auto bus = Bus(&bios, nullptr);
    CPU cpu(&bus);
    Instruction i;

    cpu.pc = RESET_VECTOR;
    cpu.gpr[2] = 100;
    cpu.gpr[3] = 200;

    i.i.rs = 2;
    i.i.rt = 3;
    i.i.immediate = 0x7FFF;

    cpu.branchOnNotEqual(i);
    EXPECT_EQ(cpu.m_branchSlotAddr, RESET_VECTOR + 4 + (0x7FFF << 2));
}
