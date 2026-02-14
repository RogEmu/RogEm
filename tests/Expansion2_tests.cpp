#include <gtest/gtest.h>
#include "Core/Expansion2.hpp"

TEST(Expansion2, unsupportedWrites)
{
    Expansion2 exp(nullptr);

    exp.write8(0, 0);
    exp.write16(0, 0);
    exp.write32(0, 0);
    EXPECT_EQ(exp.read8(0), 0);
    EXPECT_EQ(exp.read16(0), 0);
    EXPECT_EQ(exp.read32(0), 0);
}

