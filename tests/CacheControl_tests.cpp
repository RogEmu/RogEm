#include <gtest/gtest.h>
#include <memory>

#include "Core/CacheControl.hpp"
#include "Core/Bus.hpp"

// Test fixture for CacheControl tests
class CacheControlTest : public ::testing::Test {
protected:
    void SetUp() override {
        bus = std::make_unique<Bus>();
        cacheControl = std::make_unique<CacheControl>(bus.get());
    }

    std::unique_ptr<Bus> bus;
    std::unique_ptr<CacheControl> cacheControl;
};

// Test unhandled cases

TEST_F(CacheControlTest, UnhandledWrite8)
{
    // write8 should be unhandled and should not crash
    EXPECT_NO_THROW(cacheControl->write8(0x42, 0xFFFE0130));
}

TEST_F(CacheControlTest, UnhandledRead8)
{
    // read8 should be unhandled and return 0
    uint8_t result = cacheControl->read8(0xFFFE0130);
    EXPECT_EQ(0, result);
}

TEST_F(CacheControlTest, UnhandledWrite16InvalidAddress)
{
    // Writing to an invalid address should not crash
    EXPECT_NO_THROW(cacheControl->write16(0x1234, 0xFFFE0000));
}

TEST_F(CacheControlTest, UnhandledWrite32InvalidAddress)
{
    // Writing to an invalid address should not crash
    EXPECT_NO_THROW(cacheControl->write32(0x12345678, 0xFFFE0100));
}

TEST_F(CacheControlTest, UnhandledRead16InvalidAddress)
{
    // Reading from an invalid address should return 0
    uint16_t result = cacheControl->read16(0xFFFE0000);
    EXPECT_EQ(0, result);
}

TEST_F(CacheControlTest, UnhandledRead32InvalidAddress)
{
    // Reading from an invalid address should return 0
    uint32_t result = cacheControl->read32(0xFFFE0100);
    EXPECT_EQ(0, result);
}

// Test writing then reading values

TEST_F(CacheControlTest, Write32ThenRead32)
{
    // Write a 32-bit value to cache control
    uint32_t testValue = 0x12345678;
    cacheControl->write32(testValue, 0xFFFE0130);

    // Read it back
    uint32_t result = cacheControl->read32(0xFFFE0130);
    EXPECT_EQ(testValue, result);
}

TEST_F(CacheControlTest, Write16ThenRead16)
{
    // Write a 16-bit value to cache control
    uint16_t testValue = 0xABCD;
    cacheControl->write16(testValue, 0xFFFE0130);

    // Read it back
    uint16_t result = cacheControl->read16(0xFFFE0130);
    EXPECT_EQ(testValue, result);
}

TEST_F(CacheControlTest, Write32ThenRead16)
{
    // Write a 32-bit value
    uint32_t testValue = 0x12345678;
    cacheControl->write32(testValue, 0xFFFE0130);

    // Read lower 16 bits
    uint16_t result = cacheControl->read16(0xFFFE0130);
    EXPECT_EQ(0x5678, result);
}

TEST_F(CacheControlTest, Write16ThenRead32)
{
    // First clear with a write32
    cacheControl->write32(0x0, 0xFFFE0130);

    // Write a 16-bit value
    uint16_t testValue = 0xABCD;
    cacheControl->write16(testValue, 0xFFFE0130);

    // Read as 32-bit (upper 16 bits should be preserved from previous write)
    uint32_t result = cacheControl->read32(0xFFFE0130);
    EXPECT_EQ(0x0000ABCD, result);
}

TEST_F(CacheControlTest, MultipleWritesAndReads)
{
    // Test sequence of writes and reads
    cacheControl->write32(0xDEADBEEF, 0xFFFE0130);
    EXPECT_EQ(0xDEADBEEF, cacheControl->read32(0xFFFE0130));

    cacheControl->write32(0xCAFEBABE, 0xFFFE0130);
    EXPECT_EQ(0xCAFEBABE, cacheControl->read32(0xFFFE0130));

    cacheControl->write16(0x1234, 0xFFFE0130);
    EXPECT_EQ(0xCAFE1234, cacheControl->read32(0xFFFE0130));
}

TEST_F(CacheControlTest, InitialValueIsZero)
{
    // Cache control should be initialized to 0
    uint32_t result = cacheControl->read32(0xFFFE0130);
    EXPECT_EQ(0, result);
}
