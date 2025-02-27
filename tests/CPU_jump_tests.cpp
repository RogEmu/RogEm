#include <gtest/gtest.h>
#include "Utils.h"

TEST(CpuTest, JUMP) {
    BIOS bios;
    Bus bus(bios);
    CPU cpu(bus);
    Instruction i;

    cpu.m_pc = 0x80010000;
    i.j.address = 0x123456;
    cpu.jump(i);
    uint32_t expected_pc = (cpu.m_pc & 0xF0000000) | (i.j.address << 2);

    EXPECT_EQ(cpu.m_pc, expected_pc);
}

TEST(CpuTest, JUMP_AND_LINK)
{
    BIOS bios;
    Bus bus(bios);
    CPU cpu(bus);
    Instruction i;

    cpu.m_pc = 0x80010000;
    i.j.address = 0x123456;
    uint32_t expected_pc = (cpu.m_pc & 0xF0000000) | (i.j.address << 2);
    uint32_t expected_return = cpu.m_pc + 8;
    cpu.jumpAndLink(i);

    EXPECT_EQ(cpu.m_pc, expected_pc);
    EXPECT_EQ(cpu.m_registers[31], expected_return);
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

TEST(CpuTest, JUMP_AND_LINK_REGISTER) {
    BIOS bios;
    Bus bus(bios);
    CPU cpu(bus);
    Instruction i;

    cpu.m_pc = 0x80010000;

    uint32_t jump_target = 0x80020000;
    cpu.m_registers[5] = jump_target;

    i.r.rs = 5;

    uint32_t expected_return = cpu.m_pc + 8; // JALR stores PC + 8 in $ra
    uint32_t expected_pc = jump_target;

    cpu.jumpAndLinkRegister(i);

    EXPECT_EQ(cpu.m_pc, expected_pc);
    EXPECT_EQ(cpu.m_registers[31], expected_return);
}

