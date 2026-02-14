#include <gtest/gtest.h>
#include "Core/SPU.hpp"

TEST(SPU, spuStub)
{
    SPU spu(nullptr);

    spu.write8(0, 0);
    spu.write16(0, 0);
    spu.write32(0, 0);

    EXPECT_EQ(spu.read8(0), 0);
    EXPECT_EQ(spu.read16(0), 0);
    EXPECT_EQ(spu.read32(0), 0);
}
