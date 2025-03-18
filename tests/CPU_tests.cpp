#include <gtest/gtest.h>
#include "Utils.h"
#include "BIOS.h"
#include "Bus.h"
#include "CPU.h"

TEST(CpuTest, Constructor)
{
    auto bios = std::make_shared<BIOS>();
    auto bus = std::make_shared<Bus>(bios, nullptr);
    CPU cpu(bus);

    EXPECT_EQ(cpu.m_registers[0], 0);
}

TEST(CpuTest, LUI_1)
{
    auto bios = std::make_shared<BIOS>();
    auto bus = std::make_shared<Bus>(bios, nullptr);
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
    auto bios = std::make_shared<BIOS>();
    auto bus = std::make_shared<Bus>(bios, nullptr);
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
    auto bios = std::make_shared<BIOS>();
    auto bus = std::make_shared<Bus>(bios, nullptr);
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
    auto bios = std::make_shared<BIOS>();
    auto bus = std::make_shared<Bus>(bios, nullptr);
    CPU cpu(bus);

    uint32_t value = 0x12345678;
    uint8_t reg = 5;

    // Load immediate value 0x12345678 into $5
    loadImmediate(cpu, 5, value);

    EXPECT_EQ(cpu.m_registers[reg], value);
}
