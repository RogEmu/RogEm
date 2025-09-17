#include <gtest/gtest.h>
#include "BIOS.hpp"
#include "Bus.hpp"
#include "CPU.hpp"
#include "RAM.hpp"

class CpuMuldivTest : public testing::Test
{
    protected:
        Bus bus;
        CPU cpu;

        const uint32_t defaultRegVal = 0xDEADBEEF;

        CpuMuldivTest() :
            cpu(&bus)
        {
            cpu.reset();
            cpu.setReg(CpuReg::PC, 0x10000);
        }

        void runMuldiv(SecondaryOpCode opcode, CpuReg rs, CpuReg rt)
        {
            Instruction i;
            i.r.opcode = static_cast<uint8_t>(PrimaryOpCode::SPECIAL);
            i.r.rs = static_cast<uint8_t>(rs);
            i.r.rt = static_cast<uint8_t>(rt);
            i.r.funct = static_cast<uint8_t>(opcode);

            auto pc = cpu.getReg(CpuReg::PC);
            bus.storeWord(pc, i.raw);
            cpu.setReg(CpuReg::HI, defaultRegVal);
            cpu.setReg(CpuReg::LO, defaultRegVal);
            cpu.step();
        }

        void runMove(SecondaryOpCode opcode, CpuReg reg)
        {
            Instruction i;
            i.r.opcode = static_cast<uint8_t>(PrimaryOpCode::SPECIAL);
            i.r.funct = static_cast<uint8_t>(opcode);

            switch (opcode)
            {
                case SecondaryOpCode::MTLO:
                case SecondaryOpCode::MTHI:
                    i.r.rs = static_cast<uint8_t>(reg);
                    break;
                case SecondaryOpCode::MFHI:
                case SecondaryOpCode::MFLO:
                    i.r.rd = static_cast<uint8_t>(reg);
                    break;
                default:
                    break;
            }

            auto pc = cpu.getReg(CpuReg::PC);
            bus.storeWord(pc, i.raw);
            cpu.setReg(CpuReg::HI, defaultRegVal);
            cpu.setReg(CpuReg::LO, defaultRegVal);
            cpu.step();
        }
};

TEST_F(CpuMuldivTest, MULT_BASIC)
{
    int32_t rsVal = 15;
    int32_t rtVal = 42;
    CpuReg rs = CpuReg::T0;
    CpuReg rt = CpuReg::T1;

    cpu.setReg(rs, rsVal);
    cpu.setReg(rt, rtVal);
    runMuldiv(SecondaryOpCode::MULT, rs, rt);
    EXPECT_EQ(cpu.getReg(CpuReg::HI), 0);
    EXPECT_EQ(cpu.getReg(CpuReg::LO), rsVal * rtVal);
}

TEST_F(CpuMuldivTest, MULT_BASIC_NEGATIVE)
{
    int32_t rsVal = 5;
    int32_t rtVal = -3;
    CpuReg rs = CpuReg::T0;
    CpuReg rt = CpuReg::T1;

    cpu.setReg(rs, rsVal);
    cpu.setReg(rt, rtVal);
    runMuldiv(SecondaryOpCode::MULT, rs, rt);
    EXPECT_EQ(cpu.getReg(CpuReg::HI), 0xFFFFFFFF);
    EXPECT_EQ(cpu.getReg(CpuReg::LO), static_cast<uint32_t>(-15));
}

TEST_F(CpuMuldivTest, MULT_NEGATIVE)
{
    int32_t rsVal = -1612441061;
    int32_t rtVal = 1613441061;
    int64_t res = static_cast<int64_t>(rsVal) * static_cast<int64_t>(rtVal);
    CpuReg rs = CpuReg::T0;
    CpuReg rt = CpuReg::T1;

    cpu.setReg(rs, rsVal);
    cpu.setReg(rt, rtVal);
    runMuldiv(SecondaryOpCode::MULT, rs, rt);
    EXPECT_EQ(cpu.getReg(CpuReg::LO), static_cast<uint32_t>(res & 0xFFFFFFFF));
    EXPECT_EQ(cpu.getReg(CpuReg::HI), static_cast<uint32_t>(res >> 32));
}

TEST_F(CpuMuldivTest, MULT_MAXIMUM)
{
    int32_t rsVal = INT32_MAX;
    int32_t rtVal = INT32_MAX;
    int64_t res = static_cast<int64_t>(rsVal) * static_cast<int64_t>(rtVal);
    CpuReg rs = CpuReg::T0;
    CpuReg rt = CpuReg::T1;

    cpu.setReg(rs, rsVal);
    cpu.setReg(rt, rtVal);
    runMuldiv(SecondaryOpCode::MULT, rs, rt);
    EXPECT_EQ(cpu.getReg(CpuReg::LO), static_cast<uint32_t>(res & 0xFFFFFFFF));
    EXPECT_EQ(cpu.getReg(CpuReg::HI), static_cast<uint32_t>(res >> 32));
}

TEST_F(CpuMuldivTest, MULT_MININMUM)
{
    int32_t rsVal = INT32_MAX;
    int32_t rtVal = INT32_MIN;
    int64_t res = static_cast<int64_t>(rsVal) * static_cast<int64_t>(rtVal);
    CpuReg rs = CpuReg::T0;
    CpuReg rt = CpuReg::T1;

    cpu.setReg(rs, rsVal);
    cpu.setReg(rt, rtVal);
    runMuldiv(SecondaryOpCode::MULT, rs, rt);
    EXPECT_EQ(cpu.getReg(CpuReg::LO), static_cast<uint32_t>(res & 0xFFFFFFFF));
    EXPECT_EQ(cpu.getReg(CpuReg::HI), static_cast<uint32_t>(res >> 32));
}

TEST_F(CpuMuldivTest, MULT_MAXIMUM_NEGATIVE)
{
    int32_t rsVal = INT32_MIN;
    int32_t rtVal = -1;
    int64_t res = static_cast<int64_t>(rsVal) * static_cast<int64_t>(rtVal);
    CpuReg rs = CpuReg::T0;
    CpuReg rt = CpuReg::T1;

    cpu.setReg(rs, rsVal);
    cpu.setReg(rt, rtVal);
    runMuldiv(SecondaryOpCode::MULT, rs, rt);
    EXPECT_EQ(cpu.getReg(CpuReg::LO), static_cast<uint32_t>(res & 0xFFFFFFFF));
    EXPECT_EQ(cpu.getReg(CpuReg::HI), static_cast<uint32_t>(res >> 32));
}

TEST_F(CpuMuldivTest, MULT_ZERO)
{
    int32_t rsVal = 0x29F1;
    int32_t rtVal = 0;
    CpuReg rs = CpuReg::T0;
    CpuReg rt = CpuReg::T1;

    cpu.setReg(rs, rsVal);
    cpu.setReg(rt, rtVal);
    runMuldiv(SecondaryOpCode::MULT, rs, rt);
    EXPECT_EQ(cpu.getReg(CpuReg::LO), 0);
    EXPECT_EQ(cpu.getReg(CpuReg::HI), 0);
}

TEST_F(CpuMuldivTest, MULTU_BASIC)
{
    uint32_t rsVal = 1453;
    uint32_t rtVal = 87492;
    CpuReg rs = CpuReg::T0;
    CpuReg rt = CpuReg::T1;

    cpu.setReg(rs, rsVal);
    cpu.setReg(rt, rtVal);
    runMuldiv(SecondaryOpCode::MULTU, rs, rt);
    EXPECT_EQ(cpu.getReg(CpuReg::LO), rtVal * rsVal);
    EXPECT_EQ(cpu.getReg(CpuReg::HI), 0);
}

TEST_F(CpuMuldivTest, MULTU_MAX)
{
    uint32_t rsVal = UINT32_MAX;
    uint32_t rtVal = UINT32_MAX;
    CpuReg rs = CpuReg::T0;
    CpuReg rt = CpuReg::T1;

    cpu.setReg(rs, rsVal);
    cpu.setReg(rt, rtVal);
    runMuldiv(SecondaryOpCode::MULTU, rs, rt);
    EXPECT_EQ(cpu.getReg(CpuReg::LO), 0x1);
    EXPECT_EQ(cpu.getReg(CpuReg::HI), 0xFFFFFFFE);
}

TEST_F(CpuMuldivTest, MULTU_ZERO)
{
    uint32_t rsVal = 0;
    uint32_t rtVal = 0x2804FA;
    CpuReg rs = CpuReg::T0;
    CpuReg rt = CpuReg::T1;

    cpu.setReg(rs, rsVal);
    cpu.setReg(rt, rtVal);
    runMuldiv(SecondaryOpCode::MULTU, rs, rt);
    EXPECT_EQ(cpu.getReg(CpuReg::LO), 0);
    EXPECT_EQ(cpu.getReg(CpuReg::HI), 0);
}

TEST_F(CpuMuldivTest, DIV_BASIC)
{
    int32_t rsVal = 79869;
    int32_t rtVal = 141;
    CpuReg rs = CpuReg::T0;
    CpuReg rt = CpuReg::T1;

    cpu.setReg(rs, rsVal);
    cpu.setReg(rt, rtVal);
    runMuldiv(SecondaryOpCode::DIV, rs, rt);
    EXPECT_EQ(cpu.getReg(CpuReg::LO), rsVal / rtVal);
    EXPECT_EQ(cpu.getReg(CpuReg::HI), rsVal % rtVal);
}

TEST_F(CpuMuldivTest, DIV_BASIC_NEGATIVE)
{
    int32_t rsVal = -79869;
    int32_t rtVal = 141;
    CpuReg rs = CpuReg::T0;
    CpuReg rt = CpuReg::T1;

    cpu.setReg(rs, rsVal);
    cpu.setReg(rt, rtVal);
    runMuldiv(SecondaryOpCode::DIV, rs, rt);
    EXPECT_EQ(cpu.getReg(CpuReg::LO), rsVal / rtVal);
    EXPECT_EQ(cpu.getReg(CpuReg::HI), rsVal % rtVal);
}

TEST_F(CpuMuldivTest, DIV_ZERO)
{
    int32_t rsVal = 0;
    int32_t rtVal = 0;
    CpuReg rs = CpuReg::T0;
    CpuReg rt = CpuReg::T1;

    cpu.setReg(rs, rsVal);
    cpu.setReg(rt, rtVal);
    runMuldiv(SecondaryOpCode::DIV, rs, rt);
    EXPECT_EQ(cpu.getReg(CpuReg::LO), static_cast<uint32_t>(-1));
    EXPECT_EQ(cpu.getReg(CpuReg::HI), rsVal);
}

TEST_F(CpuMuldivTest, DIV_ZERO_POSITIVE)
{
    int32_t rsVal = 14809098;
    int32_t rtVal = 0;
    CpuReg rs = CpuReg::T0;
    CpuReg rt = CpuReg::T1;

    cpu.setReg(rs, rsVal);
    cpu.setReg(rt, rtVal);
    runMuldiv(SecondaryOpCode::DIV, rs, rt);
    EXPECT_EQ(cpu.getReg(CpuReg::LO), static_cast<uint32_t>(-1));
    EXPECT_EQ(cpu.getReg(CpuReg::HI), rsVal);
}

TEST_F(CpuMuldivTest, DIV_ZERO_NEGATIVE)
{
    int32_t rsVal = -757986;
    int32_t rtVal = 0;
    CpuReg rs = CpuReg::T0;
    CpuReg rt = CpuReg::T1;

    cpu.setReg(rs, rsVal);
    cpu.setReg(rt, rtVal);
    runMuldiv(SecondaryOpCode::DIV, rs, rt);
    EXPECT_EQ(cpu.getReg(CpuReg::LO), 1);
    EXPECT_EQ(cpu.getReg(CpuReg::HI), rsVal);
}

TEST_F(CpuMuldivTest, DIV_ZERO_OVERFLOW)
{
    int32_t rsVal = INT32_MIN;
    int32_t rtVal = -1;
    CpuReg rs = CpuReg::T0;
    CpuReg rt = CpuReg::T1;

    cpu.setReg(rs, rsVal);
    cpu.setReg(rt, rtVal);
    runMuldiv(SecondaryOpCode::DIV, rs, rt);
    EXPECT_EQ(cpu.getReg(CpuReg::LO), INT32_MIN);
    EXPECT_EQ(cpu.getReg(CpuReg::HI), 0);
}

TEST_F(CpuMuldivTest, DIVU_BASIC)
{
    uint32_t rsVal = 1747908;
    uint32_t rtVal = 8709;
    CpuReg rs = CpuReg::T0;
    CpuReg rt = CpuReg::T1;

    cpu.setReg(rs, rsVal);
    cpu.setReg(rt, rtVal);
    runMuldiv(SecondaryOpCode::DIVU, rs, rt);
    EXPECT_EQ(cpu.getReg(CpuReg::LO), rsVal / rtVal);
    EXPECT_EQ(cpu.getReg(CpuReg::HI), rsVal % rtVal);
}

TEST_F(CpuMuldivTest, DIVU_ZERO)
{
    uint32_t rsVal = 1747908;
    uint32_t rtVal = 0;
    CpuReg rs = CpuReg::T0;
    CpuReg rt = CpuReg::T1;

    cpu.setReg(rs, rsVal);
    cpu.setReg(rt, rtVal);
    runMuldiv(SecondaryOpCode::DIVU, rs, rt);
    EXPECT_EQ(cpu.getReg(CpuReg::LO), UINT32_MAX);
    EXPECT_EQ(cpu.getReg(CpuReg::HI), rsVal);
}

TEST_F(CpuMuldivTest, MFHI)
{
    runMove(SecondaryOpCode::MFHI, CpuReg::T8);
    EXPECT_EQ(cpu.getReg(CpuReg::T8), defaultRegVal);
}

TEST_F(CpuMuldivTest, MFHI_Zero)
{
    runMove(SecondaryOpCode::MFHI, CpuReg::ZERO);
    EXPECT_EQ(cpu.getReg(CpuReg::T8), 0);
}

TEST_F(CpuMuldivTest, MFLO)
{
    runMove(SecondaryOpCode::MFLO, CpuReg::T8);
    EXPECT_EQ(cpu.getReg(CpuReg::T8), defaultRegVal);
}

TEST_F(CpuMuldivTest, MFLO_Zero)
{
    runMove(SecondaryOpCode::MFLO, CpuReg::ZERO);
    EXPECT_EQ(cpu.getReg(CpuReg::T8), 0);
}

TEST_F(CpuMuldivTest, MTHI)
{
    uint32_t value = 0xCAFEBABE;
    cpu.setReg(CpuReg::T0, value);
    runMove(SecondaryOpCode::MTHI, CpuReg::T0);
    EXPECT_EQ(cpu.getReg(CpuReg::HI), value);
}

TEST_F(CpuMuldivTest, MTHI_Zero)
{
    runMove(SecondaryOpCode::MTHI, CpuReg::ZERO);
    EXPECT_EQ(cpu.getReg(CpuReg::HI), 0);
}

TEST_F(CpuMuldivTest, MTLO)
{
    uint32_t value = 0xCAFEBABE;
    cpu.setReg(CpuReg::T0, value);
    runMove(SecondaryOpCode::MTLO, CpuReg::T0);
    EXPECT_EQ(cpu.getReg(CpuReg::LO), value);
}

TEST_F(CpuMuldivTest, MTLO_Zero)
{
    runMove(SecondaryOpCode::MTLO, CpuReg::ZERO);
    EXPECT_EQ(cpu.getReg(CpuReg::LO), 0);
}
