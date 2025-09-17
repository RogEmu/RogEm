#include <gtest/gtest.h>
#include "BIOS.hpp"
#include "Bus.hpp"
#include "CPU.hpp"
#include "RAM.hpp"

class CpuStoreTest : public testing::Test
{
    protected:
        Bus bus;
        CPU cpu;

        const uint32_t defaultRegVal = 0xDEADBEEF;

        CpuStoreTest() :
            cpu(&bus)
        {
            cpu.reset();
            cpu.setReg(CpuReg::PC, 0x10000);
        }

        void runStore(PrimaryOpCode opcode, CpuReg rs, CpuReg rt, int16_t imm)
        {
            Instruction i;
            i.i.opcode = static_cast<uint8_t>(opcode);
            i.i.rs = static_cast<uint8_t>(rs);
            i.i.rt = static_cast<uint8_t>(rt);
            i.i.immediate = static_cast<uint16_t>(imm);

            auto pc = cpu.getReg(CpuReg::PC);
            bus.storeWord(pc, i.raw);
            uint32_t base = cpu.getReg(rs);
            uint32_t address = base + imm;

            switch (opcode)
            {
                case PrimaryOpCode::SW:
                    bus.storeWord(address, defaultRegVal);
                    break;
                case PrimaryOpCode::SH:
                    bus.storeHalfWord(address, defaultRegVal & 0xFFFF);
                    break;
                case PrimaryOpCode::SB:
                    bus.storeByte(address, defaultRegVal & 0xFF);
                default:
                    break;
            }
            cpu.step();
        }
};

TEST_F(CpuStoreTest, SW_PositiveOffset)
{
    uint32_t base = 0x2000;
    uint32_t value = 0xCAFEBABE;
    int16_t offset = 0x0018;  // Positive offset
    CpuReg rs = CpuReg::T0;
    CpuReg rt = CpuReg::T1;

    cpu.setReg(rs, base);
    cpu.setReg(rt, value);
    runStore(PrimaryOpCode::SW, rs, rt, offset);

    EXPECT_EQ(bus.loadWord(base + offset), value);
}

TEST_F(CpuStoreTest, SW_NegativeOffset)
{
    uint32_t base = 0x3008;
    uint32_t value = 0xFEEDFACE;
    int16_t offset = -4164;  // Negative offset
    CpuReg rs = CpuReg::T0;
    CpuReg rt = CpuReg::T1;

    cpu.setReg(rs, base);
    cpu.setReg(rt, value);
    runStore(PrimaryOpCode::SW, rs, rt, offset);

    EXPECT_EQ(bus.loadWord(base + offset), value);
}

TEST_F(CpuStoreTest, SW_ZeroOffset)
{
    uint32_t base = 0x4000;
    uint32_t value = 0x12345678;
    int16_t offset = 0;  // Zero offset
    CpuReg rs = CpuReg::T0;
    CpuReg rt = CpuReg::T1;

    cpu.setReg(rs, base);
    cpu.setReg(rt, value);
    runStore(PrimaryOpCode::SW, rs, rt, offset);

    EXPECT_EQ(bus.loadWord(base + offset), value);
}

TEST_F(CpuStoreTest, SW_MaxOffset)
{
    uint32_t base = 0x5001;
    uint32_t value = 0x89ABCDEF;
    int16_t offset = INT16_MAX;  // Maximum positive offset
    CpuReg rs = CpuReg::T0;
    CpuReg rt = CpuReg::T1;

    cpu.setReg(rs, base);
    cpu.setReg(rt, value);
    runStore(PrimaryOpCode::SW, rs, rt, offset);

    EXPECT_EQ(bus.loadWord(base + offset), value);
}

TEST_F(CpuStoreTest, SW_MinOffset)
{
    uint32_t base = 0x16000;
    uint32_t value = 0x10203040;
    int16_t offset = INT16_MIN;  // Minimum negative offset
    CpuReg rs = CpuReg::T0;
    CpuReg rt = CpuReg::T1;

    cpu.setReg(rs, base);
    cpu.setReg(rt, value);
    runStore(PrimaryOpCode::SW, rs, rt, offset);

    EXPECT_EQ(bus.loadWord(base + offset), value);
}

TEST_F(CpuStoreTest, SH_PositiveOffset)
{
    uint32_t base = 0x2000;
    uint16_t value = 0x1234;
    int16_t offset = 0x0136;  // Positive offset
    CpuReg rs = CpuReg::T0;
    CpuReg rt = CpuReg::T1;

    cpu.setReg(rs, base);
    cpu.setReg(rt, value);
    runStore(PrimaryOpCode::SH, rs, rt, offset);

    EXPECT_EQ(bus.loadHalfWord(base + offset), value);
}

TEST_F(CpuStoreTest, SH_NegativeOffset)
{
    uint32_t base = 0x3004;
    uint16_t value = 0x5678;
    int16_t offset = -2654;  // Negative offset
    CpuReg rs = CpuReg::T0;
    CpuReg rt = CpuReg::T1;

    cpu.setReg(rs, base);
    cpu.setReg(rt, value);
    runStore(PrimaryOpCode::SH, rs, rt, offset);

    EXPECT_EQ(bus.loadHalfWord(base + offset), value);
}

TEST_F(CpuStoreTest, SH_ZeroOffset)
{
    uint32_t base = 0x4000;
    uint16_t value = 0x9ABC;
    int16_t offset = 0;  // Zero offset
    CpuReg rs = CpuReg::T0;
    CpuReg rt = CpuReg::T1;

    cpu.setReg(rs, base);
    cpu.setReg(rt, value);
    runStore(PrimaryOpCode::SH, rs, rt, offset);

    EXPECT_EQ(bus.loadHalfWord(base + offset), value);
}

TEST_F(CpuStoreTest, SH_MaxOffset)
{
    uint32_t base = 0x5001;
    uint16_t value = 0xDEAD;
    int16_t offset = INT16_MAX;  // Maximum positive offset
    CpuReg rs = CpuReg::T0;
    CpuReg rt = CpuReg::T1;

    cpu.setReg(rs, base);
    cpu.setReg(rt, value);
    runStore(PrimaryOpCode::SH, rs, rt, offset);

    EXPECT_EQ(bus.loadHalfWord(base + offset), value);
}

TEST_F(CpuStoreTest, SH_MinOffset)
{
    uint32_t base = 0x16000;
    uint16_t value = 0xBEEF;
    int16_t offset = INT16_MIN;  // Minimum negative offset
    CpuReg rs = CpuReg::T0;
    CpuReg rt = CpuReg::T1;

    cpu.setReg(rs, base);
    cpu.setReg(rt, value);
    runStore(PrimaryOpCode::SH, rs, rt, offset);

    EXPECT_EQ(bus.loadHalfWord(base + offset), value);
}

TEST_F(CpuStoreTest, SB_PositiveOffset)
{
    uint32_t base = 0x2000;
    uint8_t value = 0x34;
    int16_t offset = 0x01679; // Positive offset
    CpuReg rs = CpuReg::T0;
    CpuReg rt = CpuReg::T1;

    cpu.setReg(rs, base);
    cpu.setReg(rt, value);
    runStore(PrimaryOpCode::SB, rs, rt, offset);

    EXPECT_EQ(bus.loadByte(base + offset), value);
}

TEST_F(CpuStoreTest, SB_NegativeOffset)
{
    uint32_t base = 0x3004;
    uint8_t value = 0x78;
    int16_t offset = -3897; // Negative offset
    CpuReg rs = CpuReg::T0;
    CpuReg rt = CpuReg::T1;

    cpu.setReg(rs, base);
    cpu.setReg(rt, value);
    runStore(PrimaryOpCode::SB, rs, rt, offset);

    EXPECT_EQ(bus.loadByte(base + offset), value);
}

TEST_F(CpuStoreTest, SB_ZeroOffset)
{
    uint32_t base = 0x4000;
    uint8_t value = 0xBC;
    int16_t offset = 0; // Zero offset
    CpuReg rs = CpuReg::T0;
    CpuReg rt = CpuReg::T1;

    cpu.setReg(rs, base);
    cpu.setReg(rt, value);
    runStore(PrimaryOpCode::SB, rs, rt, offset);

    EXPECT_EQ(bus.loadByte(base + offset), value);
}

TEST_F(CpuStoreTest, SB_MaxOffset)
{
    uint32_t base = 0x5000;
    uint8_t value = 0xDE;
    int16_t offset = INT16_MAX; // Maximum positive offset
    CpuReg rs = CpuReg::T0;
    CpuReg rt = CpuReg::T1;

    cpu.setReg(rs, base);
    cpu.setReg(rt, value);
    runStore(PrimaryOpCode::SB, rs, rt, offset);

    EXPECT_EQ(bus.loadByte(base + offset), value);
}

TEST_F(CpuStoreTest, SB_MinOffset)
{
    uint32_t base = 0x36000;
    uint8_t value = 0xEF;
    int16_t offset = INT16_MIN; // Minimum negative offset
    CpuReg rs = CpuReg::T0;
    CpuReg rt = CpuReg::T1;

    cpu.setReg(rs, base);
    cpu.setReg(rt, value);
    runStore(PrimaryOpCode::SB, rs, rt, offset);

    EXPECT_EQ(bus.loadByte(base + offset), value);
}

TEST_F(CpuStoreTest, SWR_Offset0)
{
    uint32_t base = 0x1000;
    uint32_t value = 0xAABBCCDD;
    int16_t offset = 0;
    CpuReg rs = CpuReg::T0;
    CpuReg rt = CpuReg::T1;

    cpu.setReg(rs, base);
    cpu.setReg(rt, value);
    bus.storeWord(base, 0xCAFEBABE);
    runStore(PrimaryOpCode::SWR, rs, rt, offset);
    EXPECT_EQ(bus.loadWord(base), value);
}

TEST_F(CpuStoreTest, SWR_Offset1)
{
    uint32_t base = 0x1000;
    uint32_t value = 0xAABBCCDD;
    int16_t offset = 1;
    CpuReg rs = CpuReg::T0;
    CpuReg rt = CpuReg::T1;

    cpu.setReg(rs, base);
    cpu.setReg(rt, value);
    bus.storeWord(base, 0xCAFEBABE);
    runStore(PrimaryOpCode::SWR, rs, rt, offset);
    EXPECT_EQ(bus.loadWord(base), 0xBBCCDDBE);
}

TEST_F(CpuStoreTest, SWR_Offset2)
{
    uint32_t base = 0x1000;
    uint32_t value = 0xAABBCCDD;
    int16_t offset = 2;
    CpuReg rs = CpuReg::T0;
    CpuReg rt = CpuReg::T1;

    cpu.setReg(rs, base);
    cpu.setReg(rt, value);
    bus.storeWord(base, 0xCAFEBABE);
    runStore(PrimaryOpCode::SWR, rs, rt, offset);
    EXPECT_EQ(bus.loadWord(base), 0xCCDDBABE);
}

TEST_F(CpuStoreTest, SWR_Offset3)
{
    uint32_t base = 0x1000;
    uint32_t value = 0xAABBCCDD;
    int16_t offset = 3;
    CpuReg rs = CpuReg::T0;
    CpuReg rt = CpuReg::T1;

    cpu.setReg(rs, base);
    cpu.setReg(rt, value);
    bus.storeWord(base, 0xCAFEBABE);
    runStore(PrimaryOpCode::SWR, rs, rt, offset);
    EXPECT_EQ(bus.loadWord(base), 0xDDFEBABE);
}
