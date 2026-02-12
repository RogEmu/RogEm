#include <gtest/gtest.h>
#include "Core/MemoryControl2.hpp"

TEST(MemoryControl2, unsupportedReadWrites)
{
    MemoryControl2 memctrl(nullptr);

    memctrl.write8(0, 0);
    EXPECT_EQ(memctrl.read8(0), 0);
}

TEST(MemoryControl2, readWrites)
{
    MemoryControl2 memctrl(nullptr);

    memctrl.write16(0xBABE, 0x1F801060);
    EXPECT_EQ(memctrl.read16(0x1F801060), 0xBABE);

    memctrl.write32(0xDEADBEEF, 0x1F801060);
    EXPECT_EQ(memctrl.read16(0x1F801060), 0xBEEF);

    memctrl.write32(0xCAFEBABE, 0x1F801060);
    EXPECT_EQ(memctrl.read32(0x1F801060), 0xCAFEBABE);
}
