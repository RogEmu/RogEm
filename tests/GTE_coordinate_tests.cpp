#include <gtest/gtest.h>
#include "GTE.h"

class GteCoordinateTest : public testing::Test
{
protected:
    GTE gte;

    GteCoordinateTest()
    {
        gte.reset();
    }

    void setVertex(uint8_t index, int16_t sx, int16_t sy)
    {
        uint32_t packed = (static_cast<uint32_t>(static_cast<uint16_t>(sy)) << 16) |
                          static_cast<uint16_t>(sx);
        gte.mtc(index, packed);
    }

    void setMatrix(int8_t baseIndex, Mat3x3 &m)
    {
        gte.ctc(baseIndex, (static_cast<uint32_t>(m.r11) << 16) | (m.r12 & 0xFFFF));
        gte.ctc(baseIndex + 1, (static_cast<uint32_t>(m.r13) << 16) | (m.r21 & 0xFFFF));
        gte.ctc(baseIndex + 2, (static_cast<uint32_t>(m.r22) << 16) | (m.r23 & 0xFFFF));
        gte.ctc(baseIndex + 3, (static_cast<uint32_t>(m.r31) << 16) | (m.r32 & 0xFFFF));
        gte.ctc(baseIndex + 4, (static_cast<uint32_t>(m.r33) << 16));
    }

    void setTranslation(uint8_t base, Vector3<int32_t> &v) //base 5 for tr, 13 for bk, 21 for fc
    {
        gte.ctc(base, v.x);
        gte.ctc(base + 1, v.y);
        gte.ctc(base + 2, v.z);
    }

    void setVector(uint8_t base, Vector3<int16_t> &v)
    {
        uint32_t packedXY = (static_cast<uint16_t>(v.x) & 0xFFFF) | (static_cast<uint32_t>(static_cast<uint16_t>(v.y)) << 16);
        gte.mtc(0 + base, packedXY);
        gte.mtc(1 + base, v.z);
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

    void runRtpTest(int base, Flag f, Vector3<int32_t> expectedMAC, Vector3<int16_t> expectedIR,
                    int32_t expectedSZ3, int32_t expectedSXY2, int32_t expectedIR0, int32_t expectedMAC0) //ir0-3 mac0-3 sz3 sx2 sy2
    {
        uint32_t opcode;

        if (base)
            opcode = 0x4A000030; //RTPT
        else
            opcode = 0x4A000001; //RTPS

        opcode |= (f.sf << 19);

        gte.execute(opcode);

        int32_t mac1 = static_cast<int32_t>(gte.mfc(25));
        int32_t mac2 = static_cast<int32_t>(gte.mfc(26));
        int32_t mac3 = static_cast<int32_t>(gte.mfc(27));

        EXPECT_EQ(mac1, expectedMAC.x);
        EXPECT_EQ(mac2, expectedMAC.y);
        EXPECT_EQ(mac3, expectedMAC.z);

        int16_t ir1 = static_cast<int16_t>(gte.mfc(9));
        int16_t ir2 = static_cast<int16_t>(gte.mfc(10));
        int16_t ir3 = static_cast<int16_t>(gte.mfc(11));

        EXPECT_EQ(ir1, expectedIR.x);
        EXPECT_EQ(ir2, expectedIR.y);
        EXPECT_EQ(ir3, expectedIR.z);

        int32_t sz3 = static_cast<int32_t>(gte.mfc(19));
        int32_t sxy2 = static_cast<int32_t>(gte.mfc(14));
        int32_t ir0 = static_cast<int32_t>(gte.mfc(8));
        int32_t mac0 = static_cast<int32_t>(gte.mfc(24));

        EXPECT_EQ(sz3, expectedSZ3);
        EXPECT_EQ(sxy2, expectedSXY2);
        EXPECT_EQ(ir0, expectedIR0);
        EXPECT_EQ(mac0, expectedMAC0);
    }
};

TEST_F(GteCoordinateTest, NCLIP_PositiveAreaTriangle)
{
    runNclipTest(0, 0, 10, 0, 5, 10, 100);
}

TEST_F(GteCoordinateTest, NCLIP_NegativeAreaTriangle)
{
    runNclipTest(0, 0, 5, 10, 10, 0, -100);
}

TEST_F(GteCoordinateTest, NCLIP_ZeroAreaColinear)
{
    runNclipTest(0, 0, 10, 10, 20, 20, 0);
}

TEST_F(GteCoordinateTest, NCLIP_ReversedWindingSameAreaMagnitude)
{
    runNclipTest(0, 0, 10, 0, 5, 10, 100);
    runNclipTest(0, 0, 5, 10, 10, 0, -100);
}

TEST_F(GteCoordinateTest, RtpRtpsBasic)
{
    Vector3<int16_t> v0(100, 200, 300);
    setVector(0, v0);

    Mat3x3 rotation(0x1000, 0, 0, 0, 0x1000, 0, 0, 0, 0x1000);
    setMatrix(0, rotation);

    Vector3<int32_t> translation(0x100, 0x200, 0x300);
    setTranslation(5, translation);

    gte.ctc(26, 0x100);     // H = projection plane distance
    gte.ctc(24, 0x10);    // X screen offset
    gte.ctc(25, 0x20);    // Y screen offset
    gte.ctc(27, 0x30);    // DQA
    gte.ctc(28, 0x40);    // DQB

    Vector3<int32_t> expectedMAC = {0x200000, 0x400000, 0x600000};
    Vector3<int16_t> expectedIR = {0x0200, 0x0400, 0x0600};

    int32_t sz3 = 0x600000 >> 0;
    int32_t h_div_sz3 = (0x100 * 0x20000) / sz3;
    if (h_div_sz3 > 0x1FFFF) h_div_sz3 = 0x1FFFF;
    int32_t scale = (h_div_sz3 + 1) / 2;

    int32_t mac0_x = scale * expectedIR.x + 0x10;
    int32_t mac0_y = scale * expectedIR.y + 0x20;
    int32_t sx2 = mac0_x >> 16;
    int32_t sy2 = mac0_y >> 16;

    int32_t mac0_ir0 = scale * 0x30 + 0x40;
    int32_t ir0 = mac0_ir0 >> 12;
    Flag f(1, 0, 0, 0, 0);

    runRtpTest(
        0,
        f,
        expectedMAC,
        expectedIR,
        sz3,
        (sy2 << 16) | (sx2 & 0xFFFF),
        ir0,
        mac0_ir0
    );
}
