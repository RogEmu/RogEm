#include <gtest/gtest.h>
#include "BIOS.h"
#include "RAM.h"
#include "Bus.h"
#include "CPU.h"

class CpuLoadTest : public testing::Test
{
    protected:
        Bus bus;
        BIOS bios;
        RAM ram;
        CPU cpu;

        const uint32_t defaultRegVal = 0xDEADBEEF;

        CpuLoadTest() :
            bus(&bios, &ram),
            cpu(&bus)
        {
            cpu.reset();
            cpu.setReg(CpuReg::PC, 0x10000);
        }

        void runLoad(PrimaryOpCode opcode, CpuReg rs, CpuReg rt, int16_t imm)
        {
            Instruction i;
            i.r.opcode = static_cast<uint8_t>(opcode);
            i.i.rs = static_cast<uint8_t>(rs);
            i.i.rt = static_cast<uint8_t>(rt);
            i.i.immediate = static_cast<uint16_t>(imm);

            auto pc = cpu.getReg(CpuReg::PC);
            bus.storeWord(pc, i.raw);
            cpu.setReg(rt, defaultRegVal);
            cpu.step();
        }

        void runLW(CpuReg rt, CpuReg rs, int16_t imm)
        {
            runLoad(PrimaryOpCode::LW, rs, rt, imm);
        }

        void runLH(CpuReg rt, CpuReg rs, int16_t imm)
        {
            runLoad(PrimaryOpCode::LH, rs, rt, imm);
        }

        void runLB(CpuReg rt, CpuReg rs, int16_t imm)
        {
            runLoad(PrimaryOpCode::LB, rs, rt, imm);
        }

        void runLHU(CpuReg rt, CpuReg rs, int16_t imm)
        {
            runLoad(PrimaryOpCode::LHU, rs, rt, imm);
        }

        void runLBU(CpuReg rt, CpuReg rs, int16_t imm)
        {
            runLoad(PrimaryOpCode::LBU, rs, rt, imm);
        }

        void runLWL(CpuReg rt, CpuReg rs, int16_t imm)
        {
            runLoad(PrimaryOpCode::LWL, rs, rt, imm);
        }

        void runLWR(CpuReg rt, CpuReg rs, int16_t imm)
        {
            runLoad(PrimaryOpCode::LWR, rs, rt, imm);
        }
};

TEST_F(CpuLoadTest, LW_AlignedAddress)
{
    uint32_t base = 0x1000;
    int16_t offset = 0x10;
    uint32_t value = 0x12345678;
    CpuReg rs = CpuReg::T0;
    CpuReg rt = CpuReg::T1;

    cpu.setReg(rs, base);
    bus.storeWord(base + offset, value);
    runLW(rt, rs, offset);
    EXPECT_EQ(cpu.getReg(rt), value);
}

TEST_F(CpuLoadTest, LW_Unaligned_BaseRegister)
{
    uint32_t base = 0x1003;  // unaligned (not divisible by 4)
    int16_t offset = 0;
    CpuReg rs = CpuReg::T0;
    CpuReg rt = CpuReg::T1;

    cpu.setReg(rs, base);
    runLW(rt, rs, offset);

    // Expect an alignment exception or error flag due to unaligned base
    EXPECT_EQ(cpu.getReg(rt), defaultRegVal);
    // Optionally, expect target register unchanged or zero
}

TEST_F(CpuLoadTest, LW_Unaligned_Offset)
{
    uint32_t base = 0x1000;  // aligned
    int16_t offset = 3;      // unaligned offset
    CpuReg rs = CpuReg::T2;
    CpuReg rt = CpuReg::T3;

    cpu.setReg(rs, base);
    runLW(rt, rs, offset);

    // Expect alignment exception due to unaligned effective address
    EXPECT_EQ(cpu.getReg(rt), defaultRegVal);
}

TEST_F(CpuLoadTest, LW_Unaligned_BaseAndOffset)
{
    uint32_t base = 0x1003;  // unaligned base
    int16_t offset = 3;      // unaligned offset
    CpuReg rs = CpuReg::T4;
    CpuReg rt = CpuReg::T5;

    cpu.setReg(rs, base);
    runLW(rt, rs, offset);

    // Expect alignment exception due to combined unaligned address
    EXPECT_EQ(cpu.getReg(rt), defaultRegVal);
}

TEST_F(CpuLoadTest, LW_PositiveOffset)
{
    uint32_t base = 0x2000;
    int16_t offset = 16;
    uint32_t value = 0xCAFEBABE;
    CpuReg rs = CpuReg::T2;
    CpuReg rt = CpuReg::T3;

    cpu.setReg(rs, base);
    bus.storeWord(base + offset, value);
    runLW(rt, rs, offset);
    EXPECT_EQ(cpu.getReg(rt), value);
}

TEST_F(CpuLoadTest, LW_NegativeOffset)
{
    uint32_t base = 0x3000;
    int16_t offset = -16;
    uint32_t value = 0xDEADC0DE;
    CpuReg rs = CpuReg::T4;
    CpuReg rt = CpuReg::T5;

    cpu.setReg(rs, base);
    bus.storeWord(base + offset, value);  // base - 16
    runLW(rt, rs, offset);
    EXPECT_EQ(cpu.getReg(rt), value);
}

TEST_F(CpuLoadTest, LW_MaxOffset)
{
    uint32_t base = 0x4001;
    int16_t offset = INT16_MAX; // Offset leads to misaligned address but compensated by base address
    uint32_t value = 0xFEEDFACE;
    CpuReg rs = CpuReg::T6;
    CpuReg rt = CpuReg::T7;

    cpu.setReg(rs, base);
    bus.storeWord(base + offset, value);
    runLW(rt, rs, offset);
    EXPECT_EQ(cpu.getReg(rt), value);
}

TEST_F(CpuLoadTest, LW_MinOffset)
{
    uint32_t base = 0x8000;
    int16_t offset = INT16_MIN;
    uint32_t value = 0xBAADF00D;
    CpuReg rs = CpuReg::S0;
    CpuReg rt = CpuReg::S1;

    cpu.setReg(rs, base);
    bus.storeWord(base + offset, value);  // base - 32768
    runLW(rt, rs, offset);
    EXPECT_EQ(cpu.getReg(rt), value);
}

TEST_F(CpuLoadTest, LW_BaseRegisterZero)
{
    uint32_t base = 0;     // $zero register always contains 0
    int16_t offset = 0x20;
    uint32_t value = 0xDEADBEEF;
    CpuReg rs = CpuReg::ZERO;
    CpuReg rt = CpuReg::T0;

    bus.storeWord(base + offset, value);
    runLW(rt, rs, offset);
    EXPECT_EQ(cpu.getReg(rt), value);
}

TEST_F(CpuLoadTest, LW_TargetRegisterZero)
{
    uint32_t base = 0x1000;
    int16_t offset = 0x20;
    uint32_t value = 0xDEADBEEF;
    CpuReg rs = CpuReg::T0;
    CpuReg rt = CpuReg::ZERO;

    cpu.setReg(rs, base);
    bus.storeWord(base + offset, value);
    runLW(rt, rs, offset);
    EXPECT_EQ(cpu.getReg(rt), 0);
}

TEST_F(CpuLoadTest, LH_PositiveOffset)
{
    uint32_t base = 0x1000;
    int16_t offset = 0x4;
    uint16_t value = 0x1234;
    CpuReg rs = CpuReg::T0;
    CpuReg rt = CpuReg::T1;

    cpu.setReg(rs, base);
    bus.storeHalfWord(base + offset, value);
    runLH(rt, rs, offset);
    EXPECT_EQ(cpu.getReg(rt), static_cast<uint32_t>(value));
}

TEST_F(CpuLoadTest, LH_NegativeOffset)
{
    uint32_t base = 0x1000;
    uint16_t value = 0x5678;
    int16_t offset = -42;
    CpuReg rs = CpuReg::T0;
    CpuReg rt = CpuReg::T1;

    cpu.setReg(rs, base);
    bus.storeHalfWord(base + offset, value);
    runLH(rt, rs, offset);
    EXPECT_EQ(cpu.getReg(rt), static_cast<uint32_t>(value));
}

TEST_F(CpuLoadTest, LH_MinNegativeValue)
{
    uint32_t base = 0xF000;
    uint16_t value = static_cast<uint16_t>(INT16_MIN);
    int16_t offset = 0x24;
    CpuReg rs = CpuReg::T0;
    CpuReg rt = CpuReg::T1;

    cpu.setReg(rs, base);
    bus.storeHalfWord(base + offset, value);
    runLH(rt, rs, offset);
    EXPECT_EQ(cpu.getReg(rt), 0xFFFF8000);
}

TEST_F(CpuLoadTest, LH_UnalignedLoad_PositiveOffset)
{
    uint32_t base = 0x1001;
    int16_t offset = 130;
    CpuReg rs = CpuReg::T0;
    CpuReg rt = CpuReg::T1;

    cpu.setReg(rs, base);
    runLH(rt, rs, offset);
    EXPECT_EQ(cpu.getReg(rt), defaultRegVal);
}

TEST_F(CpuLoadTest, LH_UnalignedLoad_NegativeOffset)
{
    uint32_t base = 0x1001;
    int16_t offset = -130;
    CpuReg rs = CpuReg::T0;
    CpuReg rt = CpuReg::T1;

    cpu.setReg(rs, base);
    runLH(rt, rs, offset);
    EXPECT_EQ(cpu.getReg(rt), defaultRegVal);
}

TEST_F(CpuLoadTest, LH_MaxPositiveOffset)
{
    uint32_t base = 0x1001;
    uint16_t value = 0x1234;
    int16_t offset = INT16_MAX;
    CpuReg rs = CpuReg::T0;
    CpuReg rt = CpuReg::T1;

    cpu.setReg(rs, base);
    bus.storeHalfWord(base + offset, value);
    runLH(rt, rs, offset);
    EXPECT_EQ(cpu.getReg(rt), static_cast<uint32_t>(value));
}

TEST_F(CpuLoadTest, LH_MaxNegativeOffset)
{
    uint32_t base = 0x1FFFF0;
    uint16_t value = 0x5678;
    int16_t offset = INT16_MIN;
    CpuReg rs = CpuReg::T0;
    CpuReg rt = CpuReg::T1;

    cpu.setReg(rs, base);
    bus.storeHalfWord(base + offset, value);
    runLH(rt, rs, offset);
    EXPECT_EQ(cpu.getReg(rt), static_cast<uint32_t>(value));
}

TEST_F(CpuLoadTest, LH_SignExtension)
{
    uint32_t base = 0x1000;
    uint16_t value = 0xFABC;
    int16_t offset = 2424;
    CpuReg rs = CpuReg::T0;
    CpuReg rt = CpuReg::T1;

    cpu.setReg(rs, base);
    bus.storeHalfWord(base + offset, value);
    runLH(rt, rs, offset);
    EXPECT_EQ(cpu.getReg(rt), 0xFFFFFABC);
}

TEST_F(CpuLoadTest, LB_RegularLoad)
{
    uint32_t base = 0x1000;
    int16_t offset = 0x131;
    uint8_t value = 0x17;
    CpuReg rs = CpuReg::T0;
    CpuReg rt = CpuReg::T1;

    cpu.setReg(rs, base);
    bus.storeByte(base + offset, value);
    runLB(rt, rs, offset);
    EXPECT_EQ(cpu.getReg(rt), static_cast<uint32_t>(value));
}

TEST_F(CpuLoadTest, LB_SignExtension_NegativeValue)
{
    uint32_t base = 0x1000;
    uint8_t value = 0xAB;
    int16_t offset = 0x87;
    CpuReg rs = CpuReg::T0;
    CpuReg rt = CpuReg::T1;

    cpu.setReg(rs, base);
    bus.storeByte(base + offset, value);
    runLB(rt, rs, offset);
    EXPECT_EQ(cpu.getReg(rt), 0xFFFFFFAB);
}

TEST_F(CpuLoadTest, LB_SignExtension_PositiveValue)
{
    uint32_t base = 0x1000;
    uint8_t value = 0x7F;
    int16_t offset = 0xAB;
    CpuReg rs = CpuReg::T0;
    CpuReg rt = CpuReg::T1;

    cpu.setReg(rs, base);
    bus.storeByte(base + offset, value);
    runLB(rt, rs, offset);
    EXPECT_EQ(cpu.getReg(rt), 0x0000007F);
}

TEST_F(CpuLoadTest, LHU_PositiveOffset)
{
    uint32_t base = 0x2000;
    uint16_t value = 0x1234;
    int16_t offset = 0x136;
    CpuReg rs = CpuReg::T0;
    CpuReg rt = CpuReg::T1;

    cpu.setReg(rs, base);
    bus.storeHalfWord(base + offset, value);
    runLHU(rt, rs, offset);
    EXPECT_EQ(cpu.getReg(rt), static_cast<uint32_t>(value));
}

TEST_F(CpuLoadTest, LHU_NegativeOffset)
{
    uint32_t base = 0x3004;
    uint16_t value = 0x5678;
    int16_t offset = -1738;
    CpuReg rs = CpuReg::T0;
    CpuReg rt = CpuReg::T1;

    cpu.setReg(rs, base);
    bus.storeHalfWord(base + offset, value);
    runLHU(rt, rs, offset);
    EXPECT_EQ(cpu.getReg(rt), static_cast<uint32_t>(value));
}

TEST_F(CpuLoadTest, LHU_ZeroOffset)
{
    uint32_t base = 0x4000;
    uint16_t value = 0x9ABC;
    int16_t offset = 0;
    CpuReg rs = CpuReg::T0;
    CpuReg rt = CpuReg::T1;

    cpu.setReg(rs, base);
    bus.storeHalfWord(base + offset, value);
    runLHU(rt, rs, offset);
    EXPECT_EQ(cpu.getReg(rt), static_cast<uint32_t>(value));
}

TEST_F(CpuLoadTest, LHU_MaxOffset)
{
    uint32_t base = 0x5001;
    uint16_t value = 0xDEAD;
    int16_t offset = INT16_MAX;
    CpuReg rs = CpuReg::T0;
    CpuReg rt = CpuReg::T1;

    cpu.setReg(rs, base);
    bus.storeHalfWord(base + offset, value);
    runLHU(rt, rs, offset);
    EXPECT_EQ(cpu.getReg(rt), static_cast<uint32_t>(value));
}

TEST_F(CpuLoadTest, LHU_MinOffset)
{
    uint32_t base = 0xF080;
    uint16_t value = 0xBEEF;
    int16_t offset = INT16_MIN;
    CpuReg rs = CpuReg::T0;
    CpuReg rt = CpuReg::T1;

    cpu.setReg(rs, base);
    bus.storeHalfWord(base + offset, value);
    runLHU(rt, rs, offset);
    EXPECT_EQ(cpu.getReg(rt), static_cast<uint32_t>(value));
}

TEST_F(CpuLoadTest, LHU_UnalignedAddress_PositiveOffset)
{
    uint32_t base = 0x7008;
    int16_t offset = 0xB;
    CpuReg rs = CpuReg::T0;
    CpuReg rt = CpuReg::T1;

    cpu.setReg(rs, base);
    runLHU(rt, rs, offset);
    EXPECT_EQ(cpu.getReg(rt), defaultRegVal);
}

TEST_F(CpuLoadTest, LHU_UnalignedAddress_NegativeOffset)
{
    uint32_t base = 0x7008;
    int16_t offset = -0xB;
    CpuReg rs = CpuReg::T0;
    CpuReg rt = CpuReg::T1;

    cpu.setReg(rs, base);
    runLHU(rt, rs, offset);
    EXPECT_EQ(cpu.getReg(rt), defaultRegVal);
}

TEST_F(CpuLoadTest, LHU_ZeroExtension)
{
    uint32_t base = 0x8000;
    uint16_t value = 0xABCD;
    int16_t offset = 2;
    CpuReg rs = CpuReg::T0;
    CpuReg rt = CpuReg::T1;

    cpu.setReg(rs, base);
    bus.storeHalfWord(base + offset, value);
    runLHU(rt, rs, offset);
    EXPECT_EQ(cpu.getReg(rt), 0X0000ABCD);
}

TEST_F(CpuLoadTest, LBU_PositiveOffset)
{
    uint32_t base = 0xA000;
    uint8_t value = 0xBB;
    int16_t offset = 0x0AF4;
    CpuReg rs = CpuReg::T0;
    CpuReg rt = CpuReg::T1;

    cpu.setReg(rs, base);
    bus.storeByte(base + offset, value);
    runLBU(rt, rs, offset);
    EXPECT_EQ(cpu.getReg(rt), static_cast<uint32_t>(value));
}

TEST_F(CpuLoadTest, LBU_NegativeOffset)
{
    uint32_t base = 0x3004;
    uint8_t value = 0x56;
    int16_t offset = -478;
    CpuReg rs = CpuReg::T0;
    CpuReg rt = CpuReg::T1;

    cpu.setReg(rs, base);
    bus.storeByte(base + offset, value);
    runLBU(rt, rs, offset);
    EXPECT_EQ(cpu.getReg(rt), static_cast<uint32_t>(value));
}

TEST_F(CpuLoadTest, LBU_ZeroOffset)
{
    uint32_t base = 0x4000;
    uint8_t value = 0x9A;
    int16_t offset = 0; // Zero offset
    CpuReg rs = CpuReg::T0;
    CpuReg rt = CpuReg::T1;

    cpu.setReg(rs, base);
    bus.storeByte(base + offset, value);
    runLBU(rt, rs, offset);
    EXPECT_EQ(cpu.getReg(rt), static_cast<uint32_t>(value));
}

TEST_F(CpuLoadTest, LBU_MaxOffset)
{
    uint32_t base = 0x5000;
    uint8_t value = 0xDE;
    int16_t offset = INT16_MAX;
    CpuReg rs = CpuReg::T0;
    CpuReg rt = CpuReg::T1;

    cpu.setReg(rs, base);
    bus.storeByte(base + offset, value);
    runLBU(rt, rs, offset);
    EXPECT_EQ(cpu.getReg(rt), static_cast<uint32_t>(value));
}

TEST_F(CpuLoadTest, LBU_MinOffset)
{
    uint32_t base = 0xF0F0;
    uint8_t value = 0xBE;
    int16_t offset = INT16_MIN;
    CpuReg rs = CpuReg::T0;
    CpuReg rt = CpuReg::T1;

    cpu.setReg(rs, base);
    bus.storeByte(base + offset, value);
    runLBU(rt, rs, offset);
    EXPECT_EQ(cpu.getReg(rt), static_cast<uint32_t>(value));
}

TEST_F(CpuLoadTest, LBU_ZeroExtension)
{
    uint32_t base = 0x7000;
    uint8_t value = 0xAB;
    int16_t offset = 3;
    CpuReg rs = CpuReg::T0;
    CpuReg rt = CpuReg::T1;

    cpu.setReg(rs, base);
    bus.storeByte(base + offset, value);
    runLBU(rt, rs, offset);
    EXPECT_EQ(cpu.getReg(rt), 0x000000AB);
}

TEST_F(CpuLoadTest, LWL_AlignedLoad)
{
    uint32_t base = 0x1000;
    uint32_t value = 0xAABBCCDD;
    int16_t offset = 0;
    CpuReg rs = CpuReg::T0;
    CpuReg rt = CpuReg::T1;

    cpu.setReg(rs, base);
    bus.storeWord(base, value);
    runLWL(rt, rs, offset);
    EXPECT_EQ(cpu.getReg(rt), 0xDDADBEEF);
}

TEST_F(CpuLoadTest, LWL_Offset1)
{
    uint32_t base = 0x1000;
    uint32_t value = 0xAABBCCDD;
    int16_t offset = 1;
    CpuReg rs = CpuReg::T0;
    CpuReg rt = CpuReg::T1;

    cpu.setReg(rs, base);
    bus.storeWord(base, value);
    runLWL(rt, rs, offset);
    EXPECT_EQ(cpu.getReg(rt), 0xCCDDBEEF);
}

TEST_F(CpuLoadTest, LWL_Offset2)
{
    uint32_t base = 0x1000;
    uint32_t value = 0xAABBCCDD;
    int16_t offset = 2;
    CpuReg rs = CpuReg::T0;
    CpuReg rt = CpuReg::T1;

    cpu.setReg(rs, base);
    bus.storeWord(base, value);
    runLWL(rt, rs, offset);
    EXPECT_EQ(cpu.getReg(rt), 0xBBCCDDEF);
}

TEST_F(CpuLoadTest, LWL_Offset3)
{
    uint32_t base = 0x1000;
    uint32_t value = 0xAABBCCDD;
    int16_t offset = 3;
    CpuReg rs = CpuReg::T0;
    CpuReg rt = CpuReg::T1;

    cpu.setReg(rs, base);
    bus.storeWord(base, value);
    runLWL(rt, rs, offset);
    EXPECT_EQ(cpu.getReg(rt), value);
}

TEST_F(CpuLoadTest, LWR_AlignedLoad)
{
    uint32_t base = 0x1000;
    uint32_t value = 0xAABBCCDD;  // Full word in memory
    int16_t offset = 0;
    CpuReg rs = CpuReg::T0;
    CpuReg rt = CpuReg::T1;

    cpu.setReg(rs, base);
    bus.storeWord(base, value);
    runLWR(rt, rs, offset);
    EXPECT_EQ(cpu.getReg(rt), value);
}

TEST_F(CpuLoadTest, LWR_Offset1)
{
    uint32_t base = 0x1000;
    uint32_t value = 0xAABBCCDD;
    int16_t offset = 1;
    CpuReg rs = CpuReg::T0;
    CpuReg rt = CpuReg::T1;

    cpu.setReg(rs, base);
    bus.storeWord(base, value);
    runLWR(rt, rs, offset);
    EXPECT_EQ(cpu.getReg(rt), 0xDEAABBCC);
}

TEST_F(CpuLoadTest, LWR_Offset2)
{
    uint32_t base = 0x1000;
    uint32_t value = 0xAABBCCDD;
    int16_t offset = 2;
    CpuReg rs = CpuReg::T0;
    CpuReg rt = CpuReg::T1;

    cpu.setReg(rs, base);
    bus.storeWord(base, value);
    runLWR(rt, rs, offset);
    EXPECT_EQ(cpu.getReg(rt), 0xDEADAABB);
}

TEST_F(CpuLoadTest, LWR_Offset3)
{
    uint32_t base = 0x1000;
    uint32_t value = 0xAABBCCDD;
    int16_t offset = 3;
    CpuReg rs = CpuReg::T0;
    CpuReg rt = CpuReg::T1;

    cpu.setReg(rs, base);
    bus.storeWord(base, value);
    runLWR(rt, rs, offset);
    EXPECT_EQ(cpu.getReg(rt), 0xDEADBEAA);
}
