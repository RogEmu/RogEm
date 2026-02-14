#include <gtest/gtest.h>
#include "Core/SystemControlCop.hpp"

TEST(COP0, execute)
{
    SystemControlCop cop0;

    cop0.execute(0);
}

TEST(COP0, mfcOutOfBounds)
{
    SystemControlCop cop0;

    auto result = cop0.mfc(32);
    EXPECT_EQ(result, 0);
}

TEST(COP0, mtcOutOfBounds)
{
    SystemControlCop cop0;

    cop0.mtc(32, 0xCAFEBABE);
}

TEST(COP0, cfc)
{
    SystemControlCop cop0;

    auto result = cop0.cfc(1);
    EXPECT_EQ(result, 0);
}

TEST(COP0, ctc)
{
    SystemControlCop cop0;

    cop0.ctc(1, 0xCAFEBABE);
}
