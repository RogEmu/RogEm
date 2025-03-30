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

TEST(CpuTest, LH_BasicLoad_PositiveOffset)
{
    auto bios = BIOS();
    auto ram = RAM();
    auto bus = Bus(&bios, &ram);
    CPU cpu(&bus);
    Instruction i;

    uint32_t base_address = 0x1000;  // Address in valid RAM space (KUSEG)
    uint16_t halfword_value = 0x1234;  // A simple 16-bit value
    uint16_t offset = 4;  // A simple 16-bit value

    ram.storeHalfWord(base_address + offset, halfword_value); // Store the halfword with positive offset

    cpu.setReg(static_cast<uint8_t>(GprIndex::SP), base_address); // Set base register (SP)
    i.i.rs = static_cast<uint8_t>(GprIndex::SP);  // rs = SP
    i.i.rt = static_cast<uint8_t>(GprIndex::T0); // rt = T0
    i.i.immediate = offset; // Positive offset

    cpu.loadHalfWord(i); // Execute LH instruction

    EXPECT_EQ(cpu.getReg(static_cast<uint8_t>(GprIndex::T0)), static_cast<int32_t>(halfword_value)); // Verify the value is correctly loaded
}

TEST(CpuTest, LH_BasicLoad_NegativeOffset)
{
    auto bios = BIOS();
    auto ram = RAM();
    auto bus = Bus(&bios, &ram);
    CPU cpu(&bus);
    Instruction i;

    uint32_t base_address = 0x1000;  // Address in valid RAM space (KUSEG)
    uint16_t halfword_value = 0x5678;  // Another simple 16-bit value
    int16_t offset = -4;  // Negative 16-bit value

    ram.storeHalfWord(base_address + offset, halfword_value); // Store the halfword with negative offset

    cpu.setReg(static_cast<uint8_t>(GprIndex::SP), base_address); // Set base register (SP)
    i.i.rs = static_cast<uint8_t>(GprIndex::SP);  // rs = SP
    i.i.rt = static_cast<uint8_t>(GprIndex::T0); // rt = T0
    i.i.immediate = (uint16_t)offset; // Negative offset

    cpu.loadHalfWord(i); // Execute LH instruction

    EXPECT_EQ(cpu.getReg(static_cast<uint8_t>(GprIndex::T0)), static_cast<int32_t>(halfword_value)); // Verify the value is correctly loaded
}

TEST(CpuTest, LH_NegativeLoad_PositiveOffset)
{
    auto bios = BIOS();
    auto ram = RAM();
    auto bus = Bus(&bios, &ram);
    CPU cpu(&bus);
    Instruction i;

    uint32_t base_address = 0x1000;  // Address in valid RAM space (KUSEG)
    int16_t halfword_value = -32768;  // Smallest possible 16-bit signed value (-32768 = 0x8000)
    int16_t offset = 2;  // Positive offset

    ram.storeHalfWord(base_address + offset, halfword_value); // Store the negative value with positive offset

    cpu.setReg(static_cast<uint8_t>(GprIndex::SP), base_address); // Set base register (SP)
    i.i.rs = static_cast<uint8_t>(GprIndex::SP);  // rs = SP
    i.i.rt = static_cast<uint8_t>(GprIndex::T0); // rt = T0
    i.i.immediate = (uint16_t)offset;

    cpu.loadHalfWord(i); // Execute LH instruction

    EXPECT_EQ(cpu.getReg(static_cast<uint8_t>(GprIndex::T0)), static_cast<int32_t>(halfword_value)); // Verify the negative value is correctly sign-extended
}

TEST(CpuTest, LH_NegativeLoad_NegativeOffset)
{
    auto bios = BIOS();
    auto ram = RAM();
    auto bus = Bus(&bios, &ram);
    CPU cpu(&bus);
    Instruction i;

    uint32_t base_address = 0x1000;  // Address in valid RAM space (KUSEG)
    int16_t halfword_value = -32768;  // Smallest possible 16-bit signed value (-32768 = 0x8000)
    int16_t offset = -2;  // Negative offset

    ram.storeHalfWord(base_address + offset, halfword_value); // Store the negative value with negative offset

    cpu.setReg(static_cast<uint8_t>(GprIndex::SP), base_address); // Set base register (SP)
    i.i.rs = static_cast<uint8_t>(GprIndex::SP);  // rs = SP
    i.i.rt = static_cast<uint8_t>(GprIndex::T0); // rt = T0
    i.i.immediate = (uint16_t)offset; // Negative offset

    cpu.loadHalfWord(i); // Execute LH instruction

    EXPECT_EQ(cpu.getReg(static_cast<uint8_t>(GprIndex::T0)), static_cast<int32_t>(halfword_value)); // Verify the negative value is correctly sign-extended
}

TEST(CpuTest, LH_UnalignedLoad_PositiveOffset)
{
    auto bios = BIOS();
    auto ram = RAM();
    auto bus = Bus(&bios, &ram);
    CPU cpu(&bus);
    Instruction i;

    uint32_t base_address = 0x1001;  // Unaligned base address
    int16_t offset = 130;  // Positive offset

    // Store an initial value in the destination register (T0) to check if it's untouched
    cpu.setReg(static_cast<uint8_t>(GprIndex::T0), 0xDEADBEEF);  // Set T0 to a known value

    // Set the base register (SP) to the unaligned address
    cpu.setReg(static_cast<uint8_t>(GprIndex::SP), base_address); // Set base register (SP)
    i.i.rs = static_cast<uint8_t>(GprIndex::SP);  // rs = SP
    i.i.rt = static_cast<uint8_t>(GprIndex::T0); // rt = T0
    i.i.immediate = static_cast<int16_t>(offset); // Positive unaligned offset

    // Attempt to execute LH instruction with an unaligned base address
    cpu.loadHalfWord(i);  // Execute LH instruction

    // Check if the value of T0 is unchanged (it should remain 0xDEADBEEF)
    EXPECT_EQ(cpu.getReg(static_cast<uint8_t>(GprIndex::T0)), 0xDEADBEEF);
}

TEST(CpuTest, LH_UnalignedLoad_NegativeOffset)
{
    auto bios = BIOS();
    auto ram = RAM();
    auto bus = Bus(&bios, &ram);
    CPU cpu(&bus);
    Instruction i;

    uint32_t base_address = 0x1001;  // Unaligned base address
    int16_t offset = -130;  // Positive offset

    // Store an initial value in the destination register (T0) to check if it's untouched
    cpu.setReg(static_cast<uint8_t>(GprIndex::T0), 0xDEADBEEF);  // Set T0 to a known value

    // Set the base register (SP) to the unaligned address
    cpu.setReg(static_cast<uint8_t>(GprIndex::SP), base_address); // Set base register (SP)
    i.i.rs = static_cast<uint8_t>(GprIndex::SP);  // rs = SP
    i.i.rt = static_cast<uint8_t>(GprIndex::T0); // rt = T0
    i.i.immediate = static_cast<int16_t>(offset); // Positive unaligned offset

    // Attempt to execute LH instruction with an unaligned base address
    cpu.loadHalfWord(i);  // Execute LH instruction

    // Check if the value of T0 is unchanged (it should remain 0xDEADBEEF)
    EXPECT_EQ(cpu.getReg(static_cast<uint8_t>(GprIndex::T0)), 0xDEADBEEF);
}

TEST(CpuTest, LH_MaxPositiveOffset)
{
    auto bios = BIOS();
    auto ram = RAM();
    auto bus = Bus(&bios, &ram);
    CPU cpu(&bus);
    Instruction i;

    uint32_t base_address = 0x1001;  // Address in valid RAM space (KUSEG)
    uint16_t halfword_value = 0x1234;  // A simple 16-bit value

    // Store with the max positive offset (INT16_MAX)
    ram.storeHalfWord(base_address + INT16_MAX, halfword_value);

    cpu.setReg(static_cast<uint8_t>(GprIndex::SP), base_address); // Set base register (SP)
    i.i.rs = static_cast<uint8_t>(GprIndex::SP);  // rs = SP
    i.i.rt = static_cast<uint8_t>(GprIndex::T0); // rt = T0
    i.i.immediate = (uint16_t)INT16_MAX; // Maximum positive offset

    cpu.loadHalfWord(i); // Execute LH instruction

    EXPECT_EQ(cpu.getReg(static_cast<uint8_t>(GprIndex::T0)), static_cast<int32_t>(halfword_value)); // Verify the value is correctly loaded
}

TEST(CpuTest, LH_MaxNegativeOffset)
{
    auto bios = BIOS();
    auto ram = RAM();
    auto bus = Bus(&bios, &ram);
    CPU cpu(&bus);
    Instruction i;

    uint32_t base_address = 0x1FFFF0;  // Address in valid RAM space (KUSEG)
    uint16_t halfword_value = 0x5678;  // Another simple 16-bit value

    // Store with the max negative offset (INT16_MIN)
    ram.storeHalfWord(base_address + INT16_MIN, halfword_value);

    cpu.setReg(static_cast<uint8_t>(GprIndex::SP), base_address); // Set base register (SP)
    i.i.rs = static_cast<uint8_t>(GprIndex::SP);  // rs = SP
    i.i.rt = static_cast<uint8_t>(GprIndex::T0); // rt = T0
    i.i.immediate = (uint16_t)INT16_MIN; // Maximum negative offset

    cpu.loadHalfWord(i); // Execute LH instruction

    EXPECT_EQ(cpu.getReg(static_cast<uint8_t>(GprIndex::T0)), static_cast<int32_t>(halfword_value)); // Verify the value is correctly loaded
}

TEST(CpuTest, LH_NegativeOffset_SignExtension)
{
    auto bios = BIOS();
    auto ram = RAM();
    auto bus = Bus(&bios, &ram);
    CPU cpu(&bus);
    Instruction i;

    uint32_t base_address = 0x1000;  // Address in valid RAM space (KUSEG)
    uint16_t halfword_value = 0xFABC;  // A negative value in 16-bit (sign bit set)
    int16_t offset = -2424;  // A negative value in 16-bit (sign bit set)

    // Store a negative value with a negative offset
    ram.storeHalfWord(base_address + offset, halfword_value);

    cpu.setReg(static_cast<uint8_t>(GprIndex::SP), base_address); // Set base register (SP)
    i.i.rs = static_cast<uint8_t>(GprIndex::SP);  // rs = SP
    i.i.rt = static_cast<uint8_t>(GprIndex::T0); // rt = T0
    i.i.immediate = (uint16_t)offset; // Maximum negative offset

    cpu.loadHalfWord(i); // Execute LH instruction

    // Check if the value was correctly sign-extended into a 32-bit register
    // The 16-bit value 0xFABC should be sign-extended to 0xFFFFFABC (32-bit value)
    EXPECT_EQ(cpu.getReg(static_cast<uint8_t>(GprIndex::T0)), 0xFFFFFABC); // Verify the sign extension
}
