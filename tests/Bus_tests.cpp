#include <gtest/gtest.h>
#include "Bus.h"
#include "BIOS.h"

TEST(BusTests, BusMapAddress_KUSEG_Lower_Bound)
{
    auto bios = std::make_shared<BIOS>();
    Bus bus(bios, nullptr);

    uint32_t address = 0;
    uint32_t mappedAddr = bus.mapAddress(address);

    EXPECT_EQ(address, mappedAddr);
}

TEST(BusTests, BusMapAddress_KUSEG_Upper_Bound)
{
    auto bios = std::make_shared<BIOS>();
    Bus bus(bios, nullptr);

    uint32_t address = 0x7FFFFFFF;
    uint32_t mappedAddr = bus.mapAddress(address);

    EXPECT_EQ(address, mappedAddr);
}

TEST(BusTests, BusMapAddress_KSEG0_Lower_Bound)
{
    auto bios = std::make_shared<BIOS>();
    Bus bus(bios, nullptr);

    uint32_t address = 0x80000000;
    uint32_t mappedAddr = bus.mapAddress(address);

    EXPECT_EQ(0, mappedAddr);
}

TEST(BusTests, BusMapAddress_KSEG0_Upper_Bound)
{
    auto bios = std::make_shared<BIOS>();
    Bus bus(bios, nullptr);

    uint32_t address = 0x9FFFFFFF;
    uint32_t mappedAddr = bus.mapAddress(address);

    EXPECT_EQ(0x1FFFFFFF, mappedAddr);
}

TEST(BusTests, BusMapAddress_KSEG1_Lower_Bound)
{
    auto bios = std::make_shared<BIOS>();
    Bus bus(bios, nullptr);

    uint32_t address = 0xA0000000;
    uint32_t mappedAddr = bus.mapAddress(address);

    EXPECT_EQ(0, mappedAddr);
}

TEST(BusTests, BusMapAddress_KSEG1_Upper_Bound)
{
    auto bios = std::make_shared<BIOS>();
    Bus bus(bios, nullptr);

    uint32_t address = 0xBFFFFFFF;
    uint32_t mappedAddr = bus.mapAddress(address);

    EXPECT_EQ(0x1FFFFFFF, mappedAddr);
}

TEST(BusTests, BusMapAddress_KSEG2_Lower_Bound)
{
    auto bios = std::make_shared<BIOS>();
    Bus bus(bios, nullptr);

    uint32_t address = 0xC0000000;
    uint32_t mappedAddr = bus.mapAddress(address);

    EXPECT_EQ(address, mappedAddr);
}

TEST(BusTests, BusMapAddress_KSEG2_Upper_Bound)
{
    auto bios = std::make_shared<BIOS>();
    Bus bus(bios, nullptr);

    uint32_t address = 0xFFFFFFFF;
    uint32_t mappedAddr = bus.mapAddress(address);

    EXPECT_EQ(address, mappedAddr);
}
