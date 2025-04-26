#include <gtest/gtest.h>
#include "BIOS.h"
#include "Bus.h"
#include "CPU.h"
#include "RAM.h"

class CpuLogicalTest : public testing::Test
{
    protected:
        Bus bus;
        BIOS bios;
        RAM ram;
        CPU cpu;

        const uint32_t defaultRegVal = 0xDEADBEEF;

        CpuLogicalTest() :
            bus(&bios, &ram),
            cpu(&bus)
        {
            cpu.reset();
            cpu.setReg(CpuReg::PC, 0x10000);
        }

        void runLogicalInstruction(SecondaryOpCode opcode, CpuReg rd, CpuReg rs, CpuReg rt)
        {
            Instruction i;
            i.r.opcode = static_cast<uint8_t>(PrimaryOpCode::SPECIAL);
            i.r.rd = static_cast<uint8_t>(rd);
            i.r.rs = static_cast<uint8_t>(rs);
            i.r.rt = static_cast<uint8_t>(rt);
            i.r.funct = static_cast<uint8_t>(opcode);

            auto pc = cpu.getReg(CpuReg::PC);
            bus.storeWord(pc, i.raw);
            cpu.setReg(rd, defaultRegVal);
            cpu.step();
        }

        void runLogicalInstructionImmediate(PrimaryOpCode opcode, CpuReg rs, CpuReg rt, uint16_t imm)
        {
            Instruction i;
            i.i.opcode = static_cast<uint8_t>(opcode);
            i.i.rs = static_cast<uint8_t>(rs);
            i.i.rt = static_cast<uint8_t>(rt);
            i.i.immediate = imm;

            auto pc = cpu.getReg(CpuReg::PC);
            bus.storeWord(pc, i.raw);
            cpu.setReg(rt, defaultRegVal);
            cpu.step();
        }
};

TEST_F(CpuLogicalTest, ORI_1)
{
    uint16_t imm = 0x1F1F;
    CpuReg rs = CpuReg::ZERO;
    CpuReg rt = CpuReg::T0;

    runLogicalInstructionImmediate(PrimaryOpCode::ORI, rs, rt, imm);
    EXPECT_EQ(cpu.getReg(rt), imm);
}

TEST_F(CpuLogicalTest, ORI_2)
{
    uint32_t value = 0x89FD3EAB;
    uint16_t imm = 0x1F1F;
    CpuReg rs = CpuReg::T0;
    CpuReg rt = CpuReg::T1;

    cpu.setReg(rs, value);
    runLogicalInstructionImmediate(PrimaryOpCode::ORI, rs, rt, imm);
    EXPECT_EQ(cpu.getReg(rt), value | imm);
}

TEST_F(CpuLogicalTest, ANDI_1)
{
    uint32_t value = 0x42424242;
    uint16_t imm = 0xBEEF;
    CpuReg rs = CpuReg::T0;
    CpuReg rt = CpuReg::T1;

    cpu.setReg(rs, value);
    runLogicalInstructionImmediate(PrimaryOpCode::ANDI, rs, rt, imm);
    EXPECT_EQ(cpu.getReg(rt), value & imm);
}

TEST_F(CpuLogicalTest, ANDI_2)
{
    uint32_t value = 0x89FD3EAB;
    uint16_t imm = 0x9A4F;
    CpuReg rs = CpuReg::T0;
    CpuReg rt = CpuReg::T1;

    cpu.setReg(rs, value);
    runLogicalInstructionImmediate(PrimaryOpCode::ANDI, rs, rt, imm);
    EXPECT_EQ(cpu.getReg(rt), value & imm);
}

TEST_F(CpuLogicalTest, ANDI_3)
{
    uint32_t value = 0x89FD3EAB;
    uint16_t imm = 0x1;
    CpuReg rs = CpuReg::T0;
    CpuReg rt = CpuReg::T1;

    cpu.setReg(rs, value);
    runLogicalInstructionImmediate(PrimaryOpCode::ANDI, rs, rt, imm);
    EXPECT_EQ(cpu.getReg(rt), value & imm);
}

TEST_F(CpuLogicalTest, XORI_1)
{
    uint32_t value = 0x89FD3EAB;
    uint16_t imm = 0x0;
    CpuReg rs = CpuReg::T0;
    CpuReg rt = CpuReg::T1;

    cpu.setReg(rs, value);
    runLogicalInstructionImmediate(PrimaryOpCode::XORI, rs, rt, imm);
    EXPECT_EQ(cpu.getReg(rt), value ^ imm);
}

TEST_F(CpuLogicalTest, XORI_2)
{
    uint32_t value = 0x8FBC78;
    uint16_t imm = 0xAF54;
    CpuReg rs = CpuReg::T0;
    CpuReg rt = CpuReg::T1;

    cpu.setReg(rs, value);
    runLogicalInstructionImmediate(PrimaryOpCode::XORI, rs, rt, imm);
    EXPECT_EQ(cpu.getReg(rt), value ^ imm);
}

TEST_F(CpuLogicalTest, AND_1)
{
    uint32_t rsVal = 0x12345678;
    uint32_t rtVal = 0x89FD3EAB;
    CpuReg rd = CpuReg::T0;
    CpuReg rs = CpuReg::T1;
    CpuReg rt = CpuReg::T2;

    cpu.setReg(rs, rsVal);
    cpu.setReg(rt, rtVal);
    runLogicalInstruction(SecondaryOpCode::AND, rd, rs, rt);
    EXPECT_EQ(cpu.getReg(rd), rsVal & rtVal);
}


TEST_F(CpuLogicalTest, AND_2)
{
    uint32_t rsVal = 0xFFFFFFFF;
    uint32_t rtVal = 0x89FD3EAB;
    CpuReg rd = CpuReg::T0;
    CpuReg rs = CpuReg::T1;
    CpuReg rt = CpuReg::T2;

    cpu.setReg(rs, rsVal);
    cpu.setReg(rt, rtVal);
    runLogicalInstruction(SecondaryOpCode::AND, rd, rs, rt);
    EXPECT_EQ(cpu.getReg(rd), rsVal & rtVal);
}

TEST_F(CpuLogicalTest, AND_3)
{
    uint32_t rsVal = 0x11111111;
    uint32_t rtVal = 0x89FD3EAB;
    CpuReg rd = CpuReg::T0;
    CpuReg rs = CpuReg::T1;
    CpuReg rt = CpuReg::T2;

    cpu.setReg(rs, rsVal);
    cpu.setReg(rt, rtVal);
    runLogicalInstruction(SecondaryOpCode::AND, rd, rs, rt);
    EXPECT_EQ(cpu.getReg(rd), rsVal & rtVal);
}

TEST_F(CpuLogicalTest, OR_1)
{
    uint32_t rsVal = 0xFFFF0000;
    uint32_t rtVal = 0x0000FFFF;
    CpuReg rd = CpuReg::T0;
    CpuReg rs = CpuReg::T1;
    CpuReg rt = CpuReg::T2;

    cpu.setReg(rs, rsVal);
    cpu.setReg(rt, rtVal);
    runLogicalInstruction(SecondaryOpCode::OR, rd, rs, rt);
    EXPECT_EQ(cpu.getReg(rd), rsVal | rtVal);
}

TEST_F(CpuLogicalTest, OR_2)
{
    uint32_t rsVal = 0x0;
    uint32_t rtVal = 0x0000FFFF;
    CpuReg rd = CpuReg::T0;
    CpuReg rs = CpuReg::T1;
    CpuReg rt = CpuReg::T2;

    cpu.setReg(rs, rsVal);
    cpu.setReg(rt, rtVal);
    runLogicalInstruction(SecondaryOpCode::OR, rd, rs, rt);
    EXPECT_EQ(cpu.getReg(rd), rsVal | rtVal);
}


TEST_F(CpuLogicalTest, OR_3)
{
    uint32_t rsVal = 0x78492B32;
    uint32_t rtVal = 0x127F98BA;
    CpuReg rd = CpuReg::T0;
    CpuReg rs = CpuReg::T1;
    CpuReg rt = CpuReg::T2;

    cpu.setReg(rs, rsVal);
    cpu.setReg(rt, rtVal);
    runLogicalInstruction(SecondaryOpCode::OR, rd, rs, rt);
    EXPECT_EQ(cpu.getReg(rd), rsVal | rtVal);
}

TEST_F(CpuLogicalTest, XOR_1)
{
    uint32_t rsVal = 0xFFA43EBC;
    uint32_t rtVal = 0x0000FFFF;
    CpuReg rd = CpuReg::T0;
    CpuReg rs = CpuReg::T1;
    CpuReg rt = CpuReg::T2;

    cpu.setReg(rs, rsVal);
    cpu.setReg(rt, rtVal);
    runLogicalInstruction(SecondaryOpCode::XOR, rd, rs, rt);
    EXPECT_EQ(cpu.getReg(rd), rsVal ^ rtVal);
}

TEST_F(CpuLogicalTest, XOR_2)
{
    uint32_t rsVal = 0x0;
    uint32_t rtVal = 0x0;
    CpuReg rd = CpuReg::T0;
    CpuReg rs = CpuReg::T1;
    CpuReg rt = CpuReg::T2;

    cpu.setReg(rs, rsVal);
    cpu.setReg(rt, rtVal);
    runLogicalInstruction(SecondaryOpCode::XOR, rd, rs, rt);
    EXPECT_EQ(cpu.getReg(rd), rsVal ^ rtVal);
}

TEST_F(CpuLogicalTest, NOR_1)
{
    uint32_t rsVal = 0x1F1F1F1F;
    uint32_t rtVal = 0x78563412;
    CpuReg rd = CpuReg::T0;
    CpuReg rs = CpuReg::T1;
    CpuReg rt = CpuReg::T2;

    cpu.setReg(rs, rsVal);
    cpu.setReg(rt, rtVal);
    runLogicalInstruction(SecondaryOpCode::NOR, rd, rs, rt);
    EXPECT_EQ(cpu.getReg(rd), ~(rsVal | rtVal));
}

TEST_F(CpuLogicalTest, NOR_2)
{
    uint32_t rsVal = 0x11111111;
    uint32_t rtVal = 0x0000FFFF;
    CpuReg rd = CpuReg::T0;
    CpuReg rs = CpuReg::T1;
    CpuReg rt = CpuReg::T2;

    cpu.setReg(rs, rsVal);
    cpu.setReg(rt, rtVal);
    runLogicalInstruction(SecondaryOpCode::NOR, rd, rs, rt);
    EXPECT_EQ(cpu.getReg(rd), ~(rsVal | rtVal));
}
