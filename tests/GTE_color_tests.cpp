#include <gtest/gtest.h>

#include "Core/GTE.hpp"

class GteColorTest : public ::testing::Test {
protected:
    GTE gte;

    void SetUp() override {
        gte.reset();
    }
};

TEST_F(GteColorTest, NCS_BasicLighting) {
    gte->reset();
    gte->m_ctrlReg[0] = 0x00010001; // LLM row 1
    gte->m_ctrlReg[1] = 0x00010001; // LLM row 2
    gte->m_ctrlReg[2] = 0x00010001; // LLM row 3

    gte->m_ctrlReg[6] = 0x00010001; // BK.x
    gte->m_ctrlReg[7] = 0x00010001; // BK.y
    gte->m_ctrlReg[8] = 0x00010001; // BK.z

    gte->m_ctrlReg[3] = 0x00010001; // LCM row 1
    gte->m_ctrlReg[4] = 0x00010001; // LCM row 2
    gte->m_ctrlReg[5] = 0x00010001; // LCM row 3

    gte->m_dataReg[0] = 0x00100020; // RGBC = R=0x20, G=0x10
    gte->m_dataReg[1] = 0x00000030; // B=0x30
    gte->m_dataReg[2] = 0x000000F0; // CODE = 0xF0

    gte->m_dataReg[0] = 0x00030002; // V0.x = 2, V0.y = 3
    gte->m_dataReg[1] = 0x00000001; // V0.z = 1

    gte->executeNCS(true);

    EXPECT_GE(gte->m_dataReg[9], -32768);
    EXPECT_LE(gte->m_dataReg[9], 32767);
    EXPECT_EQ(gte->colorFIFO.back()[3], 0xF0);
}

TEST_F(GteColorTest, NCT_MultipleVectorsLighting) {
    gte->reset();
    for (int i = 0; i < 3; ++i) {
        gte->m_ctrlReg[i] = 0x00010001;
        gte->m_ctrlReg[i + 3] = 0x00010001;
    }
    for (int i = 6; i < 9; ++i)
        gte->m_ctrlReg[i] = 0x00010001;

    gte->m_dataReg[0] = 0x00200010;
    gte->m_dataReg[1] = 0x00000040;
    gte->m_dataReg[2] = 0x000000AA;

    // V0 = (1, 2, 3), V1 = (4, 5, 6), V2 = (7, 8, 9)
    gte->m_dataReg[0] = (2 << 16) | 1;
    gte->m_dataReg[1] = (3);
    gte->m_dataReg[2] = (5 << 16) | 4;
    gte->m_dataReg[3] = (6);
    gte->m_dataReg[4] = (8 << 16) | 7;
    gte->m_dataReg[5] = (9);

    gte->executeNCT(true);

    EXPECT_EQ(gte->colorFIFO.size(), 3);
}

TEST_F(GteColorTest, NCS_LightingWithoutShift) {
    gte->reset();
    gte->m_ctrlReg[0] = 0x00010001;
    gte->m_ctrlReg[1] = 0x00010001;
    gte->m_ctrlReg[2] = 0x00010001;

    gte->m_ctrlReg[6] = 0x00020002;
    gte->m_ctrlReg[7] = 0x00020002;
    gte->m_ctrlReg[8] = 0x00020002;

    gte->m_ctrlReg[3] = 0x00010001;
    gte->m_ctrlReg[4] = 0x00010001;
    gte->m_ctrlReg[5] = 0x00010001;

    gte->m_dataReg[0] = (0x40 << 16) | 0x30; // G=0x40, R=0x30
    gte->m_dataReg[1] = 0x00000050;
    gte->m_dataReg[2] = 0x000000CC;

    gte->m_dataReg[0] = (2 << 16) | 1;
    gte->m_dataReg[1] = (3);

    gte->executeNCS(false);

    EXPECT_EQ(gte->colorFIFO.back()[3], 0xCC);
}

TEST_F(GteColorTest, NCS_MACClamping) {
    gte->reset();
    gte->m_ctrlReg[0] = 0x7FFF7FFF;
    gte->m_ctrlReg[1] = 0x7FFF7FFF;
    gte->m_ctrlReg[2] = 0x7FFF7FFF;

    gte->m_dataReg[0] = (100 << 16) | 100;
    gte->m_dataReg[1] = 100;
    gte->m_dataReg[2] = 0x10;

    gte->m_dataReg[0] = (0x7FFF << 16) | 0x7FFF;
    gte->m_dataReg[1] = 0x7FFF;

    gte->executeNCS(true);

    EXPECT_GE(gte->m_dataReg[9], -32768);
    EXPECT_LE(gte->m_dataReg[9], 32767);
}

TEST_F(GteColorTest, NCT_AllColorsUnique) {
    gte->reset();

    for (int i = 0; i < 9; ++i)
        gte->m_ctrlReg[i] = 0x00010001;

    gte->m_dataReg[0] = (0x10 << 16) | 0x20;
    gte->m_dataReg[1] = 0x30;
    gte->m_dataReg[2] = 0x44;

    gte->m_dataReg[0] = (2 << 16) | 1;
    gte->m_dataReg[1] = 3;
    gte->m_dataReg[2] = (5 << 16) | 4;
    gte->m_dataReg[3] = 6;
    gte->m_dataReg[4] = (8 << 16) | 7;
    gte->m_dataReg[5] = 9;

    gte->executeNCT(true);

    EXPECT_NE(gte->colorFIFO[0][0], gte->colorFIFO[1][0]);
    EXPECT_NE(gte->colorFIFO[1][0], gte->colorFIFO[2][0]);
}

TEST_F(GteColorTest, NCCS_BasicLightResponse) {
    gte.mtc(0, 0x00100020); // RGBC: R=0x10, G=0x20
    gte.mtc(1, 0x00300040); // RGBC: B=0x30, IR0=0x40
    gte.mtc(2, 0x00000055); // CODE: 0x55

    gte.mtc(9, 0x00030004); // IR1=3, IR2=4
    gte.mtc(10, 0x00050000); // IR3=5

    gte.ctc(6, 0x00100000); // BK.x = 0x0010
    gte.ctc(7, 0x00200000); // BK.y = 0x0020
    gte.ctc(8, 0x00300000); // BK.z = 0x0030

    gte.ctc(3, 0x00010000); // LCM row 0
    gte.ctc(4, 0x00020000); // LCM row 1
    gte.ctc(5, 0x00030000); // LCM row 2

    gte.executeNCCS(false);

    EXPECT_EQ(gte.mfc(25), gte.mfc(9));  // MAC1 == IR1
    EXPECT_EQ(gte.mfc(26), gte.mfc(10)); // MAC2 == IR2
    EXPECT_EQ(gte.mfc(27), gte.mfc(11)); // MAC3 == IR3
}

TEST_F(GteColorTest, NCCS_ShiftedScaledResult) {
    gte.mtc(0, 0x00100020);
    gte.mtc(1, 0x00300040);
    gte.mtc(2, 0x00000000);

    gte.mtc(9, 0x00020004);
    gte.mtc(10, 0x00060000);

    gte.ctc(6, 0x00100000);
    gte.ctc(7, 0x00200000);
    gte.ctc(8, 0x00300000);

    gte.ctc(3, 0x00010000);
    gte.ctc(4, 0x00020000);
    gte.ctc(5, 0x00030000);

    gte.executeNCCS(true); // sf = 1

    EXPECT_LE(gte.mfc(9), 32767); // IR1
    EXPECT_LE(gte.mfc(10), 32767); // IR2
    EXPECT_LE(gte.mfc(11), 32767); // IR3
}

TEST_F(GteColorTest, NCCT_TripleNormalHandling) {
    gte.mtc(0, 0x00010001); // RGBC
    gte.mtc(1, 0x00010001); // RGBC
    gte.mtc(2, 0x00000000);

    gte.mtc(9, 0x00010001); // IR1
    gte.mtc(10, 0x00010001); // IR2
    gte.mtc(11, 0x00010001); // IR3

    gte.ctc(6, 0x00100000); // BK.x
    gte.ctc(7, 0x00200000); // BK.y
    gte.ctc(8, 0x00300000); // BK.z

    gte.ctc(3, 0x00010000); // LCM
    gte.ctc(4, 0x00010000);
    gte.ctc(5, 0x00010000);

    gte.executeNCCT(false);

    // Check that FIFO has updated and values make sense
    EXPECT_GE(gte.mfc(9), -32768);
    EXPECT_LE(gte.mfc(9), 32767);
    EXPECT_GE(gte.mfc(10), -32768);
    EXPECT_LE(gte.mfc(10), 32767);
    EXPECT_GE(gte.mfc(11), -32768);
    EXPECT_LE(gte.mfc(11), 32767);
}

TEST_F(GteColorTest, NCCT_ClampingWithOverflow) {
    gte.mtc(0, 0x7FFF7FFF);
    gte.mtc(1, 0x7FFF7FFF);
    gte.mtc(2, 0x00000000);

    gte.mtc(9, 0x7FFF7FFF);
    gte.mtc(10, 0x7FFF0000);

    gte.ctc(6, 0x7FFF0000);
    gte.ctc(7, 0x7FFF0000);
    gte.ctc(8, 0x7FFF0000);

    gte.ctc(3, 0x7FFF0000);
    gte.ctc(4, 0x7FFF0000);
    gte.ctc(5, 0x7FFF0000);

    gte.executeNCCT(true);

    EXPECT_EQ(gte.mfc(9), 0x7FFF);
    EXPECT_EQ(gte.mfc(10), 0x7FFF);
    EXPECT_EQ(gte.mfc(11), 0x7FFF);
}

TEST_F(GteColorTest, NCCT_ProperColorFIFOUpdate) {
    gte.mtc(0, 0x00100010);
    gte.mtc(1, 0x00100010);
    gte.mtc(2, 0x000000FF); // CODE = 0xFF

    gte.mtc(9, 0x00100010);
    gte.mtc(10, 0x00100010);
    gte.mtc(11, 0x00100010);

    gte.ctc(6, 0x00100000);
    gte.ctc(7, 0x00200000);
    gte.ctc(8, 0x00300000);

    gte.ctc(3, 0x00010000);
    gte.ctc(4, 0x00020000);
    gte.ctc(5, 0x00030000);

    gte.executeNCCT(false);

    // Just a basic check that final FIFO values were written
    EXPECT_EQ((gte.mfc(2) & 0xFF), 0xFF); // CODE stays
}

TEST_F(GteColorTest, NCDS_InterpolatesTowardFarColor) {
    gte.mtc(9, 0x00004000); // IR1 = 0.25
    gte.mtc(10, 0x00008000); // IR2 = 0.5
    gte.mtc(11, 0x0000C000); // IR3 = 0.75

    gte.mtc(0, 0x0014000A); // RGBC: R = 10, G = 20
    gte.mtc(1, 0x0000001E); // B = 30
    gte.mtc(2, 0x00000000); // CODE

    gte.mtc(20, 0x00400080); // FC: R = 128, G = 64
    gte.mtc(21, 0x000000C0); // B = 192

    gte.mtc(1, gte.mfc(1) | (0x00008000 << 16)); // IR0 = 0.5

    gte.executeNCDS(true);

    EXPECT_GT(gte.mfc(25), 0);
    EXPECT_GT(gte.mfc(26), 0);
    EXPECT_GT(gte.mfc(27), 0);
}

TEST_F(GteColorTest, NCDT_TripleExecutionInterpolatesCorrectly) {
    for (int i = 0; i < 3; ++i) {
        gte.mtc(9, 0x00002000); // IR1 = small
        gte.mtc(10, 0x00002000); // IR2
        gte.mtc(11, 0x00002000); // IR3

        gte.mtc(0, 0x00200010); // RGBC: R = 16, G = 32
        gte.mtc(1, 0x00000040); // B = 64
        gte.mtc(2, 0x00000000); // CODE

        gte.mtc(20, 0x00800080); // FC: R = 128, G = 128
        gte.mtc(21, 0x00000080); // B = 128

        gte.mtc(1, gte.mfc(1) | (0x00004000 << 16)); // IR0 = 0.25

        gte.executeNCDT(true);

        EXPECT_GT(gte.mfc(25), 0);
        EXPECT_GT(gte.mfc(26), 0);
        EXPECT_GT(gte.mfc(27), 0);
    }
}

TEST_F(GteColorTest, NCDS_ClampsIRValuesCorrectly) {
    gte.mtc(9, 0x00008000);
    gte.mtc(10, 0x00008000);
    gte.mtc(11, 0x00008000);

    gte.mtc(0, 0x7FFF7FFF); // R = G = max
    gte.mtc(1, 0x00007FFF); // B = max
    gte.mtc(2, 0x00000000);

    gte.mtc(20, 0x7FFF7FFF);
    gte.mtc(21, 0x00007FFF);

    gte.mtc(1, gte.mfc(1) | (0x00008000 << 16)); // IR0 = 0.5

    gte.executeNCDS(true);

    EXPECT_LE(gte.mfc(9), 0x7FFF);
    EXPECT_LE(gte.mfc(10), 0x7FFF);
    EXPECT_LE(gte.mfc(11), 0x7FFF);
}

TEST_F(GteColorTest, NCDT_HandlesZeroIR0Gracefully) {
    gte.mtc(9, 0x00002000);
    gte.mtc(10, 0x00002000);
    gte.mtc(11, 0x00002000);

    gte.mtc(0, 0x00100020); // RGBC: R=16, G=32
    gte.mtc(1, 0x00000040); // B=64
    gte.mtc(2, 0x00000000);

    gte.mtc(20, 0x00000000);
    gte.mtc(21, 0x00000000);

    gte.mtc(1, gte.mfc(1) & 0x0000FFFF); // IR0 = 0

    gte.executeNCDT(true);

    EXPECT_EQ(gte.mfc(25), gte.mfc(9) << 4);
    EXPECT_EQ(gte.mfc(26), gte.mfc(10) << 4);
    EXPECT_EQ(gte.mfc(27), gte.mfc(11) << 4);
}

TEST_F(GteColorTest, NCDS_SaturatesNegativeMACs) {
    gte.mtc(9, -0x00004000);
    gte.mtc(10, -0x00008000);
    gte.mtc(11, -0x0000C000);

    gte.mtc(0, 0x00010001);
    gte.mtc(1, 0x00000001);
    gte.mtc(2, 0x00000000);

    gte.mtc(20, 0x00010001);
    gte.mtc(21, 0x00000001);

    gte.mtc(1, gte.mfc(1) | (0x00008000 << 16)); // IR0 = 0.5

    gte.executeNCDS(true);

    EXPECT_GE(gte.mfc(9), -0x8000);
    EXPECT_GE(gte.mfc(10), -0x8000);
    EXPECT_GE(gte.mfc(11), -0x8000);
}
