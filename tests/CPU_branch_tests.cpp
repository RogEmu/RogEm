#include <gtest/gtest.h>
#include "Utils.h"

TEST(CpuTest, BEQ_1)
{
    BIOS bios;
    Bus bus(bios);
    CPU cpu(bus);
    Instruction i;

    uint32_t value = 0xB16B00B5;
    int16_t imm = 0x0004;

    loadImmediate(cpu, 11, value);
    i.i.rs = 11;
    i.i.rt = 11;
    i.i.immediate = imm;
    cpu.branchOnEqual(i);

    EXPECT_EQ(cpu.m_pc, RESET_VECTOR + 8);
}

TEST(CpuTest, BEQ_2)
{
    BIOS bios;
    Bus bus(bios);
    CPU cpu(bus);
    Instruction i;

    uint32_t value = 0xB16B00B5;
    int16_t imm = 0x0004;

    loadImmediate(cpu, 11, value);
    i.i.rs = 11;
    i.i.rt = 12;
    i.i.immediate = imm;
    cpu.branchOnEqual(i);

    EXPECT_EQ(cpu.m_pc, RESET_VECTOR + 4);
}

