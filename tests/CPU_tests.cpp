#include <gtest/gtest.h>
#include "Utils.h"

TEST(CpuTest, Constructor)
{
    BIOS bios;
    Bus bus(bios);
    CPU cpu(bus);

    EXPECT_EQ(cpu.m_registers[0], 0);
}

TEST(CpuTest, LUI_1)
{
    BIOS bios;
    Bus bus(bios);
    CPU cpu(bus);

    // Load upper immediate value 0xAAAA into $8
    Instruction i;

    i.i.rt = 0x08;
    i.i.immediate = 0xAAAA;
    cpu.loadUpperImmediate(i);

    EXPECT_EQ(cpu.m_registers[i.i.rt], i.i.immediate << 16);
}

TEST(CpuTest, LUI_2)
{
    BIOS bios;
    Bus bus(bios);
    CPU cpu(bus);

    // Load upper immediate value 0xFF40 into $8
    Instruction i;
    i.i.rt = 0x08;
    i.i.immediate = 0xFF40;

    cpu.loadUpperImmediate(i);

    EXPECT_EQ(cpu.m_registers[i.i.rt], i.i.immediate << 16);
}

TEST(CpuTest, LUI_3)
{
    BIOS bios;
    Bus bus(bios);
    CPU cpu(bus);

    // Load upper immediate value 0xDEAD into $1
    Instruction i;
    i.i.rt = 0x08;
    i.i.immediate = 0xDEAD;

    cpu.loadUpperImmediate(i);

    EXPECT_EQ(cpu.m_registers[i.i.rt], i.i.immediate << 16);
}

TEST(CpuTest, Load_Immediate)
{
    BIOS bios;
    Bus bus(bios);
    CPU cpu(bus);

    uint32_t value = 0x12345678;
    uint8_t reg = 5;

    // Load immediate value 0x12345678 into $5
    loadImmediate(cpu, 5, value);

    EXPECT_EQ(cpu.m_registers[reg], value);
}
