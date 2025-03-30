#include <gtest/gtest.h>
#include "Utils.h"
#include "BIOS.h"
#include "RAM.h"
#include "Bus.h"
#include "CPU.h"

#include <sstream>

namespace testing {
    template <>
    std::string PrintToString<uint32_t>(const uint32_t& value) {
        std::stringstream ss;
        ss << "0x" << std::hex << value;
        return ss.str();
    }
}

TEST(CpuTest, LW_AlignedAddress)
{
    auto bios = BIOS();
    auto ram = RAM();
    auto bus = Bus(&bios, &ram);
    CPU cpu(&bus);
    Instruction i;

    uint32_t base_address = 0x1000;
    uint32_t offset = 0x4;
    uint32_t address = base_address + offset;
    uint32_t expected_value = 0xDEADBEEF;

    ram.storeWord(address, expected_value); // Store value in RAM

    cpu.setReg(static_cast<uint8_t>(GprIndex::SP), base_address); // Set base register (SP)
    i.i.rs = static_cast<uint8_t>(GprIndex::SP);  // rs = SP
    i.i.rt = static_cast<uint8_t>(GprIndex::T0); // rt = T0
    i.i.immediate = offset;

    cpu.loadWord(i);

    EXPECT_EQ(cpu.getReg(static_cast<uint8_t>(GprIndex::T0)), expected_value); // Compare T0 register value
}

TEST(CpuTest, LW_UnalignedAddress)
{
    auto bios = BIOS();
    auto ram = RAM();
    auto bus = Bus(&bios, &ram);
    CPU cpu(&bus);
    Instruction i;

    uint32_t base_address = 0x1000;
    uint32_t offset = 0x3; // Unaligned
    uint32_t initial_rt_value = 0xBADF00D;

    cpu.setReg(static_cast<uint8_t>(GprIndex::SP), base_address); // Set base register (SP)
    cpu.setReg(static_cast<uint8_t>(GprIndex::T0), initial_rt_value); // Set initial value in T0

    i.i.rs = static_cast<uint8_t>(GprIndex::SP);  // rs = SP
    i.i.rt = static_cast<uint8_t>(GprIndex::T0); // rt = T0
    i.i.immediate = offset;

    cpu.loadWord(i);

    EXPECT_EQ(cpu.getReg(static_cast<uint8_t>(GprIndex::T0)), initial_rt_value); // Register should remain unchanged
}

TEST(CpuTest, LW_NegativeOffset)
{
    auto bios = BIOS();
    auto ram = RAM();
    auto bus = Bus(&bios, &ram);
    CPU cpu(&bus);
    Instruction i;

    uint32_t base_address = 0x2008;
    uint32_t offset = -4; // Negative offset
    uint32_t target_address = base_address + offset;
    uint32_t expected_value = 0x12345678;

    ram.storeWord(target_address, expected_value); // Store word at target address

    cpu.setReg(static_cast<uint8_t>(GprIndex::SP), base_address); // Set base register (SP)
    i.i.rs = static_cast<uint8_t>(GprIndex::SP);  // rs = SP
    i.i.rt = static_cast<uint8_t>(GprIndex::T0); // rt = T0
    i.i.immediate = offset;

    cpu.loadWord(i);

    EXPECT_EQ(cpu.getReg(static_cast<uint8_t>(GprIndex::T0)), expected_value); // Verify loaded value
}

TEST(CpuTest, LW_MaxAddress)
{
    auto bios = BIOS();
    auto ram = RAM();
    auto bus = Bus(&bios, &ram);
    CPU cpu(&bus);
    Instruction i;

    uint32_t base_address = 0x1FFFC;
    uint32_t offset = 0x0; // No offset
    uint32_t address = base_address + offset;
    uint32_t expected_value = 0xAABBCCDD;

    ram.storeWord(address, expected_value);

    cpu.setReg(static_cast<uint8_t>(GprIndex::SP), base_address); // Set base register (SP)
    i.i.rs = static_cast<uint8_t>(GprIndex::SP);  // rs = SP
    i.i.rt = static_cast<uint8_t>(GprIndex::T0); // rt = T0
    i.i.immediate = offset;

    cpu.loadWord(i);

    EXPECT_EQ(cpu.getReg(static_cast<uint8_t>(GprIndex::T0)), expected_value); // Verify loaded value
}

TEST(CpuTest, LW_ZeroRegister)
{
    auto bios = BIOS();
    auto ram = RAM();
    auto bus = Bus(&bios, &ram);
    CPU cpu(&bus);
    Instruction i;

    uint32_t base_address = 0x1000;
    uint32_t offset = 0x4;
    uint32_t address = base_address + offset;
    uint32_t value = 0xCAFEBABE;

    ram.storeWord(address, value);

    cpu.setReg(static_cast<uint8_t>(GprIndex::SP), base_address); // Set base register (SP)
    i.i.rs = static_cast<uint8_t>(GprIndex::SP);  // rs = SP
    i.i.rt = static_cast<uint8_t>(GprIndex::ZERO); // Writing to $zero
    i.i.immediate = offset;

    cpu.loadWord(i);

    EXPECT_EQ(cpu.getReg(static_cast<uint8_t>(GprIndex::ZERO)), 0); // $zero must remain 0
}
