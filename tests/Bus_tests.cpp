#include <gtest/gtest.h>

#include "Core/MemoryMap.hpp"
#include "Core/Bus.hpp"
#include "Core/BIOS.hpp"
#include "Core/CPU.hpp"

TEST(BusTests, BusMapAddress_KUSEG_Lower_Bound)
{
    Bus bus;

    uint32_t address = 0;
    uint32_t mappedAddr = MemoryMap::mapAddress(address);

    EXPECT_EQ(address, mappedAddr);
}

TEST(BusTests, BusMapAddress_KUSEG_Upper_Bound)
{
    Bus bus;

    uint32_t address = 0x7FFFFFFF;
    uint32_t mappedAddr = MemoryMap::mapAddress(address);

    EXPECT_EQ(address, mappedAddr);
}

TEST(BusTests, BusMapAddress_KSEG0_Lower_Bound)
{
    Bus bus;

    uint32_t address = 0x80000000;
    uint32_t mappedAddr = MemoryMap::mapAddress(address);

    EXPECT_EQ(0, mappedAddr);
}

TEST(BusTests, BusMapAddress_KSEG0_Upper_Bound)
{
    Bus bus;

    uint32_t address = 0x9FFFFFFF;
    uint32_t mappedAddr = MemoryMap::mapAddress(address);

    EXPECT_EQ(0x1FFFFFFF, mappedAddr);
}

TEST(BusTests, BusMapAddress_KSEG1_Lower_Bound)
{
    Bus bus;

    uint32_t address = 0xA0000000;
    uint32_t mappedAddr = MemoryMap::mapAddress(address);

    EXPECT_EQ(0, mappedAddr);
}

TEST(BusTests, BusMapAddress_KSEG1_Upper_Bound)
{
    Bus bus;

    uint32_t address = 0xBFFFFFFF;
    uint32_t mappedAddr = MemoryMap::mapAddress(address);

    EXPECT_EQ(0x1FFFFFFF, mappedAddr);
}

TEST(BusTests, BusMapAddress_KSEG2_Lower_Bound)
{
    Bus bus;

    uint32_t address = 0xC0000000;
    uint32_t mappedAddr = MemoryMap::mapAddress(address);

    EXPECT_EQ(address, mappedAddr);
}

TEST(BusTests, BusMapAddress_KSEG2_Upper_Bound)
{
    Bus bus;

    uint32_t address = 0xFFFFFFFF;
    uint32_t mappedAddr = MemoryMap::mapAddress(address);

    EXPECT_EQ(address, mappedAddr);
}

TEST(BusTests, ResetBus)
{
    Bus bus;
    bus.reset();
}

TEST(BusTests, UpdateDevices)
{
    Bus bus;
    bus.updateDevices(2);
}

TEST(BusTests, CpuConnection)
{
    Bus bus;
    EXPECT_EQ(bus.getCpu(), nullptr);
    CPU cpu(&bus);
    bus.connectCpu(&cpu);
    EXPECT_EQ(bus.getCpu(), &cpu);
}

TEST(BusTests, UnalignedLoads)
{
    Bus bus;

    EXPECT_EQ(bus.loadHalfWord(0xBFC00001), 0);
    EXPECT_EQ(bus.loadHalfWord(0xBFC00003), 0);

    EXPECT_EQ(bus.loadWord(0xBFC00001), 0);
    EXPECT_EQ(bus.loadWord(0xBFC00002), 0);
    EXPECT_EQ(bus.loadWord(0xBFC00003), 0);
}

TEST(BusTests, addressNotSupportedLoads)
{
    Bus bus;

    EXPECT_EQ(bus.loadByte(0x1FA00000), 0);
    EXPECT_EQ(bus.loadHalfWord(0x1FA00000), 0);
    EXPECT_EQ(bus.loadWord(0x1FA00000), 0);
}

TEST(BusTests, UnalignedStores)
{
    Bus bus;

    bus.storeHalfWord(0xBFC00001, 0xBEEF);
    bus.storeHalfWord(0xBFC00003, 0xBEEF);

    bus.storeWord(0xBFC00001, 0xDEADBEEF);
    bus.storeWord(0xBFC00002, 0xDEADBEEF);
    bus.storeWord(0xBFC00003, 0xDEADBEEF);
}

TEST(BusTests, addressNotSupportedStores)
{
    Bus bus;

    bus.storeByte(0x1FA00000, 0xAB);
    bus.storeHalfWord(0x1FA00000, 0xABCD);
    bus.storeWord(0x1FA00000, 0xABCD1234);
}

TEST(BusTest, getDeviceFail)
{
    Bus bus;

    auto device = bus.getDevice<Bus>();
    EXPECT_EQ(device, nullptr);
}

TEST(BusTest, getDeviceSuccess)
{
    Bus bus;

    auto device = bus.getDevice<BIOS>();
    EXPECT_NE(device, nullptr);
}
