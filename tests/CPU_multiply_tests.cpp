#include <gtest/gtest.h>
#include "Utils.h"
#include "BIOS.h"
#include "Bus.h"
#include "CPU.h"

TEST(CpuTest, MULT_NEGATIVE)
{
    auto bios = BIOS();
    auto bus = Bus(&bios, nullptr);
    CPU cpu(&bus);
    Instruction i;

    int32_t left = -1612441061;
    int32_t right = 1613441061;
    int64_t res = static_cast<int64_t>(left) * static_cast<int64_t>(right);

    loadImmediate(cpu, 8, static_cast<uint32_t>(left));
    loadImmediate(cpu, 9, static_cast<uint32_t>(right));
    i.r.rs = 8;
    i.r.rt = 9;
    cpu.multiply(i);

    EXPECT_EQ(cpu.lo, static_cast<uint32_t>(res & 0xFFFFFFFF));
    EXPECT_EQ(cpu.hi, static_cast<uint32_t>(res >> 32));
}

TEST(CpuTest, MULT_MAXIMUM)
{
    auto bios = BIOS();
    auto bus = Bus(&bios, nullptr);
    CPU cpu(&bus);
    Instruction i;

    int32_t left = 0x7FFFFFFF;
    int32_t right = 0x00000002;
    int64_t res = static_cast<int64_t>(left) * static_cast<int64_t>(right);

    loadImmediate(cpu, 8, static_cast<uint32_t>(left));
    loadImmediate(cpu, 9, static_cast<uint32_t>(right));
    i.r.rs = 8;
    i.r.rt = 9;
    cpu.multiply(i);

    EXPECT_EQ(cpu.lo, static_cast<uint32_t>(res & 0xFFFFFFFF));
    EXPECT_EQ(cpu.hi, static_cast<uint32_t>(res >> 32));
}

TEST(CpuTest, MULT_MAXIMUM_NEGATIVE)
{
    auto bios = BIOS();
    auto bus = Bus(&bios, nullptr);
    CPU cpu(&bus);
    Instruction i;

    int32_t left = 0x80000000;
    int32_t right = 0xFFFFFFFF;
    int64_t res = static_cast<int64_t>(left) * static_cast<int64_t>(right);

    loadImmediate(cpu, 8, static_cast<uint32_t>(left));
    loadImmediate(cpu, 9, static_cast<uint32_t>(right));
    i.r.rs = 8;
    i.r.rt = 9;
    cpu.multiply(i);

    EXPECT_EQ(cpu.lo, static_cast<uint32_t>(res & 0xFFFFFFFF));
    EXPECT_EQ(cpu.hi, static_cast<uint32_t>(res >> 32));
}

TEST(CpuTest, MULT_NULL)
{
    auto bios = BIOS();
    auto bus = Bus(&bios, nullptr);
    CPU cpu(&bus);
    Instruction i;

    int32_t left = 0;
    int32_t right = 0;
    int64_t res = static_cast<int64_t>(left) * static_cast<int64_t>(right);

    loadImmediate(cpu, 8, static_cast<uint32_t>(left));
    loadImmediate(cpu, 9, static_cast<uint32_t>(right));
    i.r.rs = 8;
    i.r.rt = 9;
    cpu.multiply(i);

    EXPECT_EQ(cpu.lo, static_cast<uint32_t>(res & 0xFFFFFFFF));
    EXPECT_EQ(cpu.hi, static_cast<uint32_t>(res >> 32));
}

TEST(CpuTest, MULTU_NEGATIVE)
{
    auto bios = BIOS();
    auto bus = Bus(&bios, nullptr);
    CPU cpu(&bus);
    Instruction i;

    uint32_t left = (uint32_t)-1612441061;
    uint32_t right = 1612441061;
    uint64_t res = static_cast<uint64_t>(left) * static_cast<uint64_t>(right);

    loadImmediate(cpu, 8, left);
    loadImmediate(cpu, 9, right);
    i.r.rs = 8;
    i.r.rt = 9;
    cpu.multiplyUnsigned(i);

    EXPECT_EQ(cpu.lo, static_cast<uint32_t>(res & 0xFFFFFFFF));
    EXPECT_EQ(cpu.hi, static_cast<uint32_t>(res >> 32));
}

TEST(CpuTest, MULTU_NULL)
{
    auto bios = BIOS();
    auto bus = Bus(&bios, nullptr);
    CPU cpu(&bus);
    Instruction i;

    uint32_t left = 0;
    uint32_t right = 0;
    uint64_t res = static_cast<uint64_t>(left) * static_cast<uint64_t>(right);

    loadImmediate(cpu, 8, left);
    loadImmediate(cpu, 9, right);
    i.r.rs = 8;
    i.r.rt = 9;
    cpu.multiplyUnsigned(i);

    EXPECT_EQ(cpu.lo, static_cast<uint32_t>(res & 0xFFFFFFFF));
    EXPECT_EQ(cpu.hi, static_cast<uint32_t>(res >> 32));
}

TEST(CpuTest, DIV_NEGATIVE)
{
    auto bios = BIOS();
    auto bus = Bus(&bios, nullptr);
    CPU cpu(&bus);
    Instruction i;

    int32_t left = -1612441061;
    int32_t right = 1613441061;

    loadImmediate(cpu, 8, static_cast<uint32_t>(left));
    loadImmediate(cpu, 9, static_cast<uint32_t>(right));
    i.r.rs = 8;
    i.r.rt = 9;
    cpu.divide(i);

    EXPECT_EQ(cpu.lo, left / right);
    EXPECT_EQ(cpu.hi, left % right);
}

TEST(CpuTest, DIV_MAXIMUM)
{
    auto bios = BIOS();
    auto bus = Bus(&bios, nullptr);
    CPU cpu(&bus);
    Instruction i;

    int32_t left = 0x7FFFFFFF;
    int32_t right = 0x00000002;

    loadImmediate(cpu, 8, static_cast<uint32_t>(left));
    loadImmediate(cpu, 9, static_cast<uint32_t>(right));
    i.r.rs = 8;
    i.r.rt = 9;
    cpu.divide(i);

    EXPECT_EQ(cpu.lo, left / right);
    EXPECT_EQ(cpu.hi, left % right);
}

TEST(CpuTest, DIVU_NEGATIVE)
{
    auto bios = BIOS();
    auto bus = Bus(&bios, nullptr);
    CPU cpu(&bus);
    Instruction i;

    uint32_t left = (uint32_t)-1612441061;
    uint32_t right = 1612441061;

    loadImmediate(cpu, 8, left);
    loadImmediate(cpu, 9, right);
    i.r.rs = 8;
    i.r.rt = 9;
    cpu.divideUnsigned(i);

    EXPECT_EQ(cpu.lo, left / right);
    EXPECT_EQ(cpu.hi, left % right);
}

TEST(CpuTest, MFHI)
{
    auto bios = BIOS();
    auto bus = Bus(&bios, nullptr);
    CPU cpu(&bus);
    Instruction i;

    uint32_t value = 0x12345678;
    loadImmediate(cpu, 8, value);
    cpu.hi = value;

    i.r.rd = 9;
    cpu.moveFromHi(i);

    EXPECT_EQ(cpu.getReg(9), value);
}

TEST(CpuTest, MFLO)
{
    auto bios = BIOS();
    auto bus = Bus(&bios, nullptr);
    CPU cpu(&bus);
    Instruction i;

    uint32_t value = 0x12345678;
    loadImmediate(cpu, 8, value);
    cpu.lo = value;

    i.r.rd = 9;
    cpu.moveFromLo(i);

    EXPECT_EQ(cpu.getReg(9), value);
}

TEST(CpuTest, MTHI)
{
    auto bios = BIOS();
    auto bus = Bus(&bios, nullptr);
    CPU cpu(&bus);
    Instruction i;

    uint32_t value = 0x12345678;
    loadImmediate(cpu, 8, value);

    i.r.rs = 8;
    cpu.moveToHi(i);

    EXPECT_EQ(cpu.hi, value);
}

TEST(CpuTest, MTLO)
{
    auto bios = BIOS();
    auto bus = Bus(&bios, nullptr);
    CPU cpu(&bus);
    Instruction i;

    uint32_t value = 0x12345678;
    loadImmediate(cpu, 8, value);

    i.r.rs = 8;
    cpu.moveToLo(i);

    EXPECT_EQ(cpu.lo, value);
}
