#include <gtest/gtest.h>
#include "Utils.h"
#include "BIOS.h"
#include "Bus.h"
#include "CPU.h"

TEST(CpuTest, JUMP) {
    auto bios = std::make_shared<BIOS>();
    auto bus = std::make_shared<Bus>(bios, nullptr);
    CPU cpu(bus);
    Instruction i;

    cpu.m_pc = 0x80010000;
    i.j.address = 0x123456;
    cpu.jump(i);
    uint32_t expected_pc = (cpu.m_pc & 0xF0000000) | (i.j.address << 2);

    EXPECT_EQ(cpu.m_branchSlotAddr, expected_pc);
}

TEST(CpuTest, JUMP_AND_LINK)
{
    auto bios = std::make_shared<BIOS>();
    auto bus = std::make_shared<Bus>(bios, nullptr);
    CPU cpu(bus);
    Instruction i;

    cpu.m_pc = 0x80010000;
    i.j.address = 0x123456;
    uint32_t expected_pc = (cpu.m_pc & 0xF0000000) | (i.j.address << 2);
    uint32_t expected_return = cpu.m_pc + 8;
    cpu.jumpAndLink(i);

    EXPECT_EQ(cpu.m_branchSlotAddr, expected_pc);
    EXPECT_EQ(cpu.m_registers[31], expected_return);
}

TEST(CpuTest, JUMP_REGISTER)
{
    auto bios = std::make_shared<BIOS>();
    auto bus = std::make_shared<Bus>(bios, nullptr);
    CPU cpu(bus);
    Instruction i;

    uint32_t address = 0x12345678;
    loadImmediate(cpu, 8, address);
    i.r.rs = 8;
    cpu.jumpRegister(i);

    EXPECT_EQ(cpu.m_branchSlotAddr, address);
}

TEST(CpuTest, JUMP_AND_LINK_REGISTER)
{
    auto bios = std::make_shared<BIOS>();
    auto bus = std::make_shared<Bus>(bios, nullptr);
    CPU cpu(bus);
    Instruction i;

    cpu.m_pc = 0x80010000;

    uint32_t jump_target = 0x80020000;
    cpu.setReg(5, jump_target);

    i.r.rs = 5;
    i.r.rd = 12;

    uint32_t expected_return = cpu.m_pc + 8; // JALR stores PC + 8 in $ra
    uint32_t expected_pc = jump_target;

    cpu.jumpAndLinkRegister(i);

    EXPECT_EQ(cpu.m_registers[12], expected_return);
    EXPECT_EQ(cpu.m_branchSlotAddr, expected_pc);
}

TEST(CpuTest, JUMP_AND_LINK_REGISTER_2)
{
    auto bios = std::make_shared<BIOS>();
    auto bus = std::make_shared<Bus>(bios, nullptr);
    CPU cpu(bus);
    Instruction i;

    cpu.m_pc = 0x80010000;

    uint32_t jump_target = 0x80020000;
    cpu.setReg(5, jump_target);

    i.r.rs = 5;
    i.r.rd = 24;

    uint32_t expected_return = cpu.m_pc + 8; // JALR stores PC + 8 in $ra
    uint32_t expected_pc = jump_target;

    cpu.jumpAndLinkRegister(i);

    EXPECT_EQ(cpu.m_registers[24], expected_return);
    EXPECT_EQ(cpu.m_branchSlotAddr, expected_pc);
}

TEST(CpuTest, JUMP_AND_LINK_REGISTER_3)
{
    auto bios = std::make_shared<BIOS>();
    auto bus = std::make_shared<Bus>(bios, nullptr);
    CPU cpu(bus);
    Instruction i;

    cpu.m_pc = 0x80010000;

    uint32_t jump_target = 0x80020000;
    cpu.setReg(5, jump_target);

    i.r.rs = 5;
    i.r.rd = 31;

    uint32_t expected_return = cpu.m_pc + 8; // JALR stores PC + 8 in $ra
    uint32_t expected_pc = jump_target;

    cpu.jumpAndLinkRegister(i);

    EXPECT_EQ(cpu.m_registers[31], expected_return);
    EXPECT_EQ(cpu.m_branchSlotAddr, expected_pc);
}
