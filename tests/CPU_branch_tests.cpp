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

    EXPECT_EQ(cpu.pc, RESET_VECTOR + 4);  // ✅ Correct expected behavior
}
