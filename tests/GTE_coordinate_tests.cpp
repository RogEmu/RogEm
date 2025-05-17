#include <gtest/gtest.h>
#include "GTE.h"

class GteCoordinateTest : public testing::Test
{
protected:
    GTE gte;

    void setVertex(uint8_t index, int16_t sx, int16_t sy)
    {
        uint32_t packed = (static_cast<uint32_t>(static_cast<uint16_t>(sy)) << 16) |
                          static_cast<uint16_t>(sx);
        gte.mtc(index, packed);
    }

    void runNclipTest(int16_t sx0, int16_t sy0,
                      int16_t sx1, int16_t sy1,
                      int16_t sx2, int16_t sy2,
                      int32_t expected)
    {
        setVertex(12, sx0, sy0);
        setVertex(13, sx1, sy1);
        setVertex(14, sx2, sy2);

        gte.execute(0x4A000006); // NCLIP opcode

        int32_t mac0 = gte.mfc(24); // MAC0
        EXPECT_EQ(mac0, expected);
    }
};

TEST_F(GteCoordinateTest, PositiveAreaTriangle)
{
    runNclipTest(0, 0, 10, 0, 5, 10, 100);
}

TEST_F(GteCoordinateTest, NegativeAreaTriangle)
{
    runNclipTest(0, 0, 5, 10, 10, 0, -100);
}

TEST_F(GteCoordinateTest, ZeroAreaColinear)
{
    runNclipTest(0, 0, 10, 10, 20, 20, 0);
}

TEST_F(GteCoordinateTest, ReversedWindingSameAreaMagnitude)
{
    runNclipTest(0, 0, 10, 0, 5, 10, 100);
    runNclipTest(0, 0, 5, 10, 10, 0, -100);
}
