#include <gtest/gtest.h>
#include "Utils.h"

TEST(CpuTest, MULT_NEGATIVE)
{
    BIOS bios;
    Bus bus(bios);
    CPU cpu(bus);
    Instruction i;

    int32_t left = -1612441061;
    int32_t right = 1613441061;
    int64_t res = static_cast<int64_t>(left) * static_cast<int64_t>(right);

    loadImmediate(cpu, 8, static_cast<uint32_t>(left));
    loadImmediate(cpu, 9, static_cast<uint32_t>(right));
    i.r.rs = 8;
    i.r.rt = 9;
    cpu.multiply(i);

    EXPECT_EQ(cpu.m_lo, static_cast<uint32_t>(res & 0xFFFFFFFF));
    EXPECT_EQ(cpu.m_hi, static_cast<uint32_t>(res >> 32));
}

TEST(CpuTest, MULT_MAXIMUM)
{
    BIOS bios;
    Bus bus(bios);
    CPU cpu(bus);
    Instruction i;

    int32_t left = 0x7FFFFFFF;
    int32_t right = 0x00000002;
    int64_t res = static_cast<int64_t>(left) * static_cast<int64_t>(right);

    loadImmediate(cpu, 8, static_cast<uint32_t>(left));
    loadImmediate(cpu, 9, static_cast<uint32_t>(right));
    i.r.rs = 8;
    i.r.rt = 9;
    cpu.multiply(i);

    EXPECT_EQ(cpu.m_lo, static_cast<uint32_t>(res & 0xFFFFFFFF));
    EXPECT_EQ(cpu.m_hi, static_cast<uint32_t>(res >> 32));
}

TEST(CpuTest, MULT_MAXIMUM_NEGATIVE)
{
    BIOS bios;
    Bus bus(bios);
    CPU cpu(bus);
    Instruction i;

    int32_t left = 0x80000000;
    int32_t right = 0xFFFFFFFF;
    int64_t res = static_cast<int64_t>(left) * static_cast<int64_t>(right);

    loadImmediate(cpu, 8, static_cast<uint32_t>(left));
    loadImmediate(cpu, 9, static_cast<uint32_t>(right));
    i.r.rs = 8;
    i.r.rt = 9;
    cpu.multiply(i);

    EXPECT_EQ(cpu.m_lo, static_cast<uint32_t>(res & 0xFFFFFFFF));
    EXPECT_EQ(cpu.m_hi, static_cast<uint32_t>(res >> 32));
}

TEST(CpuTest, MULT_NULL)
{
    BIOS bios;
    Bus bus(bios);
    CPU cpu(bus);
    Instruction i;

    int32_t left = 0;
    int32_t right = 0;
    int64_t res = static_cast<int64_t>(left) * static_cast<int64_t>(right);

    loadImmediate(cpu, 8, static_cast<uint32_t>(left));
    loadImmediate(cpu, 9, static_cast<uint32_t>(right));
    i.r.rs = 8;
    i.r.rt = 9;
    cpu.multiply(i);

    EXPECT_EQ(cpu.m_lo, static_cast<uint32_t>(res & 0xFFFFFFFF));
    EXPECT_EQ(cpu.m_hi, static_cast<uint32_t>(res >> 32));
}

TEST(CpuTest, MULTU_NEGATIVE)
{
    BIOS bios;
    Bus bus(bios);
    CPU cpu(bus);
    Instruction i;

    uint32_t left = (uint32_t)-1612441061;
    uint32_t right = 1612441061;
    uint64_t res = static_cast<uint64_t>(left) * static_cast<uint64_t>(right);

    loadImmediate(cpu, 8, left);
    loadImmediate(cpu, 9, right);
    i.r.rs = 8;
    i.r.rt = 9;
    cpu.multiplyUnsigned(i);

    EXPECT_EQ(cpu.m_lo, static_cast<uint32_t>(res & 0xFFFFFFFF));
    EXPECT_EQ(cpu.m_hi, static_cast<uint32_t>(res >> 32));
}

TEST(CpuTest, MULTU_NULL)
{
    BIOS bios;
    Bus bus(bios);
    CPU cpu(bus);
    Instruction i;

    uint32_t left = 0;
    uint32_t right = 0;
    uint64_t res = static_cast<uint64_t>(left) * static_cast<uint64_t>(right);

    loadImmediate(cpu, 8, left);
    loadImmediate(cpu, 9, right);
    i.r.rs = 8;
    i.r.rt = 9;
    cpu.multiplyUnsigned(i);

    EXPECT_EQ(cpu.m_lo, static_cast<uint32_t>(res & 0xFFFFFFFF));
    EXPECT_EQ(cpu.m_hi, static_cast<uint32_t>(res >> 32));
}
