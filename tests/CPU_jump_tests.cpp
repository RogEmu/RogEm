#include <gtest/gtest.h>
#include "Utils.h"

TEST(CpuTest, JUMP)
{
    BIOS bios;
    Bus bus(bios);
    CPU cpu(bus);
    Instruction i;

    uint32_t address = 0x12345678;
    i.j.address = address;
    cpu.jump(i);

    EXPECT_EQ(cpu.m_pc, address);
}
