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

TEST(CpuTest, JUMP_AND_LINK)
{
    BIOS bios;
    Bus bus(bios);
    CPU cpu(bus);
    Instruction i;

    uint32_t address = 0x12345678;
    i.j.address = address;
    cpu.jumpAndLink(i);

    EXPECT_EQ(cpu.m_pc, address);
    EXPECT_EQ(cpu.m_registers[31], 0x0);
}

TEST(CpuTest, JUMP_REGISTER)
{
    BIOS bios;
    Bus bus(bios);
    CPU cpu(bus);
    Instruction i;

    uint32_t address = 0x12345678;
    loadImmediate(cpu, 8, address);
    i.r.rs = 8;
    cpu.jumpRegister(i);

    EXPECT_EQ(cpu.m_pc, address);
}