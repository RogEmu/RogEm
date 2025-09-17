#include <gtest/gtest.h>
#include "BIOS.hpp"
#include "Bus.hpp"
#include "CPU.hpp"
#include "RAM.hpp"

class CpuJumpTest : public testing::Test
{
    protected:
        Bus bus;
        CPU cpu;

        CpuJumpTest() :
            cpu(&bus)
        {
            cpu.setReg(CpuReg::PC, 0x10000);
        }
};

TEST_F(CpuJumpTest, JUMP)
{
    auto pc = cpu.getReg(CpuReg::PC);
    bus.storeWord(pc, 0x08002000); // J 0x2000
    bus.storeWord(pc + 4, 0x25090010); // ADDIU $t1, $t2, 0x10

    cpu.setReg(CpuReg::T0, 0x5);

    cpu.step(); // Executes delay slot
    EXPECT_EQ(cpu.getReg(CpuReg::PC), pc + 4);

    cpu.step(); // Executes jump
    EXPECT_EQ(cpu.getReg(CpuReg::PC), 0x8000);
    EXPECT_EQ(cpu.getReg(CpuReg::T1), 21); // T1 = T0 + 0x10
}

TEST_F(CpuJumpTest, JUMP_AND_LINK)
{
    auto pc = cpu.getReg(CpuReg::PC);
    bus.storeWord(pc, 0x0C002000); // JAL 0x2000
    bus.storeWord(pc + 4, 0x25090010); // ADDIU $t1, $t2, 0x10

    cpu.setReg(CpuReg::T0, 5);

    cpu.step(); // Executes delay slot
    EXPECT_EQ(cpu.getReg(CpuReg::PC), pc + 4);
    EXPECT_EQ(cpu.getReg(CpuReg::RA), pc + 8);

    cpu.step(); // Executes jump
    EXPECT_EQ(cpu.getReg(CpuReg::T1), 21); // T1 = T0 + 10
    EXPECT_EQ(cpu.getReg(CpuReg::PC), 0x8000);
}

TEST_F(CpuJumpTest, JUMP_REGISTER)
{
    auto pc = cpu.getReg(CpuReg::PC);
    bus.storeWord(pc, 0x01800008); // JR $t4
    bus.storeWord(pc + 4, 0x25490010); // ADDIU $t1, $t2, 0x10

    cpu.setReg(CpuReg::T2, 0x10);
    cpu.setReg(CpuReg::T4, 0xFABC);

    cpu.step(); // delay slot
    EXPECT_EQ(cpu.getReg(CpuReg::PC), pc + 4);

    cpu.step(); // jump
    EXPECT_EQ(cpu.getReg(CpuReg::PC), 0xFABC);
    EXPECT_EQ(cpu.getReg(CpuReg::T1), 32);
}

TEST_F(CpuJumpTest, JUMP_AND_LINK_REGISTER)
{
    auto pc = cpu.getReg(CpuReg::PC);
    bus.storeWord(pc, 0x01A06009); // JALR $t4, $t5
    bus.storeWord(pc + 4, 0x25490010); // ADDIU $t1, $t2, 0x10

    cpu.setReg(CpuReg::T2, 0x10);
    cpu.setReg(CpuReg::T5, 0xFABC); // Set jump address

    cpu.step(); // delay slot
    EXPECT_EQ(cpu.getReg(CpuReg::PC), pc + 4);
    EXPECT_EQ(cpu.getReg(CpuReg::T4), pc + 8);

    cpu.step(); // jump
    EXPECT_EQ(cpu.getReg(CpuReg::PC), 0xFABC);
    EXPECT_EQ(cpu.getReg(CpuReg::T1), 32);
}

TEST_F(CpuJumpTest, JUMP_AND_LINK_REGISTER_RA)
{
    auto pc = cpu.getReg(CpuReg::PC);
    bus.storeWord(pc, 0x0180F809); // JALR $t4, $ra
    bus.storeWord(pc + 4, 0x25490010); // ADDIU $t1, $t2, 0x10

    cpu.setReg(CpuReg::T2, 0x10);
    cpu.setReg(CpuReg::T4, 0xFABC); // Set jump address

    cpu.step(); // delay slot
    EXPECT_EQ(cpu.getReg(CpuReg::PC), pc + 4);
    EXPECT_EQ(cpu.getReg(CpuReg::RA), pc + 8);

    cpu.step(); // jump
    EXPECT_EQ(cpu.getReg(CpuReg::PC), 0xFABC);
    EXPECT_EQ(cpu.getReg(CpuReg::T1), 32);
}
