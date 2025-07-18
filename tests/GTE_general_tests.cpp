#include <gtest/gtest.h>
#include "GTE.h"

class GteGeneralTest : public testing::Test {
protected:
    GTE gte;

    GteGeneralTest()
    {
        gte.reset();
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

    void setVectorIR(Vector3<int16_t> &v) {
        gte.mtc(9, v.x); // IR1
        gte.mtc(10, v.y); // IR2
        gte.mtc(11, v.z); // IR3
    }

    void setVectorMAC(Vector3<int32_t> &v)
    {
        gte.mtc(25, v.x); // MAC1
        gte.mtc(26, v.y); // MAC2
        gte.mtc(27, v.z); // MAC3
    }

    void setVector(uint8_t base, Vector3<int16_t> &v)
    {
        uint32_t packedXY = (static_cast<uint16_t>(v.x) & 0xFFFF) | (static_cast<uint32_t>(static_cast<uint16_t>(v.y)) << 16);
        gte.mtc(0 + base, packedXY);
        gte.mtc(1 + base, v.z);
    }

    void runMvmvaTest(Vector3<int16_t> &expectedIR, Vector3<int32_t> &expectedMAC, Flag &f)
    {
        uint32_t opcode = 0x4A000012; // Base MVMVA opcode
        opcode |= (f.sf << 19);
        opcode |= (f.mx << 17);
        opcode |= (f.v << 15);
        opcode |= (f.cv << 13);
        opcode |= (f.lm << 10);

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
    }

    void runGpTest(bool base, Vector3<int16_t> &expectedIR, Vector3<int32_t> &expectedMAC, Rgbc &expectedFifo, Flag &f)
    {
        uint32_t opcode;

        if (base)
            opcode = 0x4A00003E; //GPL
        else
            opcode = 0x4A00003D; //GPF

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

        int32_t fifo = gte.mfc(20);

        EXPECT_EQ((fifo >> 24) & 0xFF, expectedFifo.c);
        EXPECT_EQ((fifo >> 16) & 0xFF, expectedFifo.r);
        EXPECT_EQ((fifo >> 8) & 0xFF, expectedFifo.g);
        EXPECT_EQ(fifo & 0xFF, expectedFifo.b);
    }

};

TEST_F(GteGeneralTest, MVMVA_Identity_Basic_Flag)
{
    Mat3x3 m(1, 0, 0, 0, 1, 0, 0, 0, 1);
    Vector3 t(0, 0, 0);
    Vector3<int16_t> v(1, 1, 1);
    Vector3<int16_t> expectedIR(1, 1, 1);
    Vector3<int32_t> expectedMAC(1, 1, 1);
    Flag f(0, 0, 0, 0, 0);
    setMatrix(0, m);
    setTranslation(5, t);
    setVector(0, v);
    runMvmvaTest(expectedIR, expectedMAC, f);
}

TEST_F(GteGeneralTest, MVMVA_Basic_Flag)
{
    Mat3x3 m(5, 8, 7, 6, 4, 10, 4, 5, 6);
    Vector3 t(0, 20, 30);
    Vector3<int16_t> v(5, 10, 20);
    Vector3<int16_t> expectedIR(245, 290, 220);
    Vector3<int32_t> expectedMAC(245, 290, 220);
    Flag f(0, 0, 0, 0, 0);
    setMatrix(0, m);
    setTranslation(5, t);
    setVector(0, v);
    runMvmvaTest(expectedIR, expectedMAC, f);
}

TEST_F(GteGeneralTest, MVMVA_Negative_V1)
{
    Mat3x3 m(5, 8, 7, 6, 4, 10, 4, 5, 6);
    Vector3 t(0, -20, -30);
    Vector3<int16_t> v(-5, -10, -20);
    Vector3<int16_t> expectedIR(-245, -290, -220);
    Vector3<int32_t> expectedMAC(-245, -290, -220);
    Flag f(0, 0, 1, 0, 0);
    setMatrix(0, m);
    setTranslation(5, t);
    setVector(2, v);
    runMvmvaTest(expectedIR, expectedMAC, f);
}

TEST_F(GteGeneralTest, MVMVA_SF_Flag)
{
    Mat3x3 m(5, 8, 7, 6, 4, 10, 4, 5, 6);
    Vector3 t(0, 20, -4000);
    Vector3<int16_t> v(100, 200, 400);
    Vector3<int16_t> expectedIR(1, 1, -1);
    Vector3<int32_t> expectedMAC(1, 1, -1);
    Flag f(1, 0, 0, 0, 0);
    setMatrix(0, m);
    setTranslation(5, t);
    setVector(0, v);
    runMvmvaTest(expectedIR, expectedMAC, f);
}

TEST_F(GteGeneralTest, MVMVA_Lm_High_Different_MAC)
{
    Mat3x3 m(5, 8, 7, 6, 4, 10, 4, 5, 6);
    Vector3 t(0x7FFF, 20, 30);
    Vector3<int16_t> v(5, 10, 20);
    Vector3<int16_t> expectedIR(0x7FFF, 290, 220);
    Vector3<int32_t> expectedMAC(0x7FFF + 245, 290, 220);
    Flag f(0, 0, 0, 0, 1);
    setMatrix(0, m);
    setTranslation(5, t);
    setVector(0, v);
    runMvmvaTest(expectedIR, expectedMAC, f);
}

TEST_F(GteGeneralTest, MVMVA_Lm_Low_Different_MAC)
{
    Mat3x3 m(5, 8, 7, 6, 4, 10, 4, 5, 6);
    Vector3 t(0, -20, -0x8000);
    Vector3<int16_t> v(-5, -10, -20);
    Vector3<int16_t> expectedIR(0, 0, 0);
    Vector3<int32_t> expectedMAC(-245, -290, -190 - 0x8000);
    Flag f(0, 0, 0, 0, 1);
    setMatrix(0, m);
    setTranslation(5, t);
    setVector(0, v);
    runMvmvaTest(expectedIR, expectedMAC, f);
}

TEST_F(GteGeneralTest, MVMVA_LightMatrix_V2)
{
    Mat3x3 m(5, 8, 7, 6, 4, 10, 4, 5, 6);
    Vector3 t(0, 20, 30);
    Vector3<int16_t> v(5, 10, 20);
    Vector3<int16_t> expectedIR(245, 290, 220);
    Vector3<int32_t> expectedMAC(245, 290, 220);
    Flag f(0, 1, 2, 0, 0);
    setMatrix(8, m);
    setTranslation(5, t);
    setVector(4, v);
    runMvmvaTest(expectedIR, expectedMAC, f);
}

TEST_F(GteGeneralTest, MVMVA_Negative_ColorMatrix_IR)
{
    Mat3x3 m(5, 8, 7, 6, 4, 10, 4, 5, 6);
    Vector3 t(0, -20, -30);
    Vector3<int16_t> v(-5, -10, -20);
    Vector3<int16_t> expectedIR(-245, -290, -220);
    Vector3<int32_t> expectedMAC(-245, -290, -220);
    Flag f(0, 2, 3, 0, 0);
    setMatrix(16, m);
    setTranslation(5, t);
    setVectorIR(v);
    runMvmvaTest(expectedIR, expectedMAC, f);
}

TEST_F(GteGeneralTest, MVMVA_GarbageMatrix_BK_Translation)
{
    Mat3x3 m(5, 8, 10, 6, 20, 10, 4, 5, 6);
    Vector3 t(0, 20, 30);
    Vector3<int16_t> v(5, 10, 20);
    Vector3<int16_t> expectedIR(580, 370, 730);
    Vector3<int32_t> expectedMAC(580, 370, 730);
    Flag f(0, 3, 0, 1, 0);
    setMatrix(0, m);
    setTranslation(13, t);
    setVector(0, v);
    gte.mtc(8, 5);
    runMvmvaTest(expectedIR, expectedMAC, f);
}

TEST_F(GteGeneralTest, MVMVA_FC_Translation) {
    Mat3x3 m(5, 8, 7, 6, 4, 10, 4, 5, 6);
    Vector3 t(0, 20, 30);
    Vector3<int16_t> v(5, 10, 20);
    Vector3<int16_t> expectedIR(140, 200, 120);
    Vector3<int32_t> expectedMAC(140, 200, 120);
    Flag f(0, 0, 0, 2, 0);
    setMatrix(0, m);
    setTranslation(21, t);
    setVector(0, v);
    runMvmvaTest(expectedIR, expectedMAC, f);
}

TEST_F(GteGeneralTest, GPF_Basic_test)
{
    Vector3<int16_t> IR(2, 3, 5);
    Vector3<int16_t> expectedIR(8, 12, 20);
    Vector3<int32_t> expectedMAC(8, 12, 20);
    Rgbc expectedFifo(0, 0, 1, 0);
    Flag f(0, 0, 0, 0, 0);
    gte.mtc(8, 4); //IR0
    setVectorIR(IR);
    runGpTest(false, expectedIR, expectedMAC, expectedFifo, f);
}

TEST_F(GteGeneralTest, GPF_Sf)
{
    Vector3<int16_t> IR(0x0010, 0x0020, 0x0030);
    Vector3<int16_t> expectedIR(16, 32, 48);
    Vector3<int32_t> expectedMAC(16, 32, 48);
    Rgbc expectedFifo(1, 2, 3, 0);
    Flag f(1, 0, 0, 0, 0);
    gte.mtc(8, 0x1000); //IR0
    setVectorIR(IR);
    runGpTest(false, expectedIR, expectedMAC, expectedFifo, f);
}

TEST_F(GteGeneralTest, GPL_Basic_test)
{
    Vector3<int16_t> IR(2, 3, 5);
    Vector3<int32_t> MAC(10, 20, 30);
    Vector3<int16_t> expectedIR(18, 32, 50);
    Vector3<int32_t> expectedMAC(18, 32, 50);
    Rgbc expectedFifo(1, 2, 3, 0);
    Flag f(0, 0, 0, 0, 0);
    gte.mtc(8, 4); //IR0
    setVectorIR(IR);
    setVectorMAC(MAC);
    runGpTest(true, expectedIR, expectedMAC, expectedFifo, f);
}

TEST_F(GteGeneralTest, GPL_Sf_with_code)
{
    Vector3<int16_t> IR(0x0001, 0x0002, 0x0003);
    Vector3<int32_t> MAC(0x0010, 0x0020, 0x0030);
    Vector3<int16_t> expectedIR(16, 32, 48);
    Vector3<int32_t> expectedMAC(16, 32, 48);
    Rgbc expectedFifo(1, 2, 3, 1);
    Flag f(1, 0, 0, 0, 0);
    gte.mtc(8, 0x0100); //IR0
    gte.mtc(20, 0x01000000); // fifo CODE = 1
    setVectorIR(IR);
    setVectorMAC(MAC);
    runGpTest(true, expectedIR, expectedMAC, expectedFifo, f);
}
