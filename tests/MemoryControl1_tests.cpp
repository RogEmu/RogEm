#include <gtest/gtest.h>
#include "Core/MemoryControl1.hpp"

TEST(MemoryControl1, unsupportedReadWrites)
{
    MemoryControl1 memctrl(nullptr);

    EXPECT_EQ(memctrl.read8(0), 0);
    EXPECT_EQ(memctrl.read16(0), 0);
    memctrl.write8(0, 0);
    memctrl.write16(0, 0);
}

TEST(MemoryControl1, readWrite)
{
    MemoryControl1 memctrl(nullptr);
    uint32_t value = 0xCAFEBABE;
    uint32_t baseAddr = 0x1F801000;

    for (uint32_t i = 0; i <= 0x20; i += 4) {
        memctrl.write32(value, baseAddr + i);
        EXPECT_EQ(memctrl.read32(baseAddr + i), value);
    }
    memctrl.write32(value, baseAddr + 0x24);
    EXPECT_EQ(memctrl.read32(baseAddr + 0x24), 0);
}
