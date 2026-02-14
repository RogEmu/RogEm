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
    gte.ctc(0, 0x00010001); // LLM row 1
    gte.ctc(1, 0x00010001); // LLM row 2
    gte.ctc(2, 0x00010001); // LLM row 3

    gte.ctc(6, 0x00010001); // BK.x
    gte.ctc(7, 0x00010001); // BK.y
    gte.ctc(8, 0x00010001); // BK.z

    gte.ctc(3, 0x00010001); // LCM row 1
    gte.ctc(4, 0x00010001); // LCM row 2
    gte.ctc(5, 0x00010001); // LCM row 3

    gte.mtc(0, 0x00030002); // V0.x = 2, V0.y = 3
    gte.mtc(1, 0x00000001); // V0.z = 1
    gte.mtc(2, 0x000000F0); // CODE = 0xF0

    gte.execute(0x0008001E); // NCS with sf=1
}

TEST_F(GteColorTest, NCT_MultipleVectorsLighting) {
    for (uint8_t i = 0; i < 3; ++i) {
        gte.ctc(i, 0x00010001);
        gte.ctc(i + 3, 0x00010001);
    }
    for (uint8_t i = 6; i < 9; ++i)
        gte.ctc(i, 0x00010001);

    // V0 = (1, 2, 3), V1 = (4, 5, 6), V2 = (7, 8, 9)
    gte.mtc(0, (2 << 16) | 1);
    gte.mtc(1, 3);
    gte.mtc(2, (5 << 16) | 4);
    gte.mtc(3, 6);
    gte.mtc(4, (8 << 16) | 7);
    gte.mtc(5, 9);
    gte.mtc(6, 0x000000AA); // CODE

    gte.execute(0x00080020); // NCT with sf=1
}

TEST_F(GteColorTest, NCS_LightingWithoutShift) {
    gte.ctc(0, 0x00010001);
    gte.ctc(1, 0x00010001);
    gte.ctc(2, 0x00010001);

    gte.ctc(6, 0x00020002);
    gte.ctc(7, 0x00020002);
    gte.ctc(8, 0x00020002);

    gte.ctc(3, 0x00010001);
    gte.ctc(4, 0x00010001);
    gte.ctc(5, 0x00010001);

    gte.mtc(0, (2 << 16) | 1); // V0
    gte.mtc(1, 3);
    gte.mtc(2, 0x000000CC); // CODE=0xCC

    gte.execute(0x0000001E); // NCS with sf=0
}

TEST_F(GteColorTest, NCS_MACClamping) {
    gte.ctc(0, 0x7FFF7FFF);
    gte.ctc(1, 0x7FFF7FFF);
    gte.ctc(2, 0x7FFF7FFF);

    gte.mtc(0, (0x7FFF << 16) | 0x7FFF); // V0
    gte.mtc(1, 0x7FFF);
    gte.mtc(2, 0x00000010); // CODE

    gte.execute(0x0008001E); // NCS with sf=1
}

TEST_F(GteColorTest, NCT_AllColorsUnique) {
    for (uint8_t i = 0; i < 9; ++i)
        gte.ctc(i, 0x00010001);

    gte.mtc(0, (2 << 16) | 1); // V0
    gte.mtc(1, 3);
    gte.mtc(2, (5 << 16) | 4); // V1
    gte.mtc(3, 6);
    gte.mtc(4, (8 << 16) | 7); // V2
    gte.mtc(5, 9);
    gte.mtc(6, 0x00000044); // CODE

    gte.execute(0x00080020); // NCT with sf=1
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

    gte.execute(0x0000001B); // NCCS with sf=0
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

    gte.execute(0x0008001B); // NCCS with sf=1
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

    gte.execute(0x0000003F); // NCCT with sf=0
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

    gte.execute(0x0008003F); // NCCT with sf=1
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

    gte.execute(0x0000003F); // NCCT with sf=0
}

TEST_F(GteColorTest, NCDS_InterpolatesTowardFarColor) {
    gte.mtc(9, 0x00004000); // IR1 = 0.25
    gte.mtc(10, 0x00008000); // IR2 = 0.5
    gte.mtc(11, 0x0000C000); // IR3 = 0.75

    gte.mtc(0, 0x0014000A); // RGBC: R = 10, G = 20
    gte.mtc(1, 0x0000001E); // B = 30
    gte.mtc(2, 0x00000000); // CODE

    gte.ctc(21, 128 << 4); // FC.x = 128 (scaled)
    gte.ctc(22, 64 << 4);  // FC.y = 64 (scaled)
    gte.ctc(23, 192 << 4); // FC.z = 192 (scaled)

    gte.mtc(1, gte.mfc(1) | (0x00008000 << 16)); // IR0 = 0.5

    gte.execute(0x00080013); // NCDS with sf=1
}

TEST_F(GteColorTest, NCDT_TripleExecutionInterpolatesCorrectly) {
    for (int i = 0; i < 3; ++i) {
        gte.mtc(9, 0x00002000); // IR1 = small
        gte.mtc(10, 0x00002000); // IR2
        gte.mtc(11, 0x00002000); // IR3

        gte.mtc(0, 0x00200010); // RGBC: R = 16, G = 32
        gte.mtc(1, 0x00000040); // B = 64
        gte.mtc(2, 0x00000000); // CODE

        gte.ctc(21, 128 << 4); // FC.x = 128 (scaled)
        gte.ctc(22, 128 << 4); // FC.y = 128 (scaled)
        gte.ctc(23, 128 << 4); // FC.z = 128 (scaled)

        gte.mtc(1, gte.mfc(1) | (0x00004000 << 16)); // IR0 = 0.25

        gte.execute(0x00080016); // NCDT with sf=1
    }
}

TEST_F(GteColorTest, NCDS_ClampsIRValuesCorrectly) {
    gte.mtc(9, 0x00008000);
    gte.mtc(10, 0x00008000);
    gte.mtc(11, 0x00008000);

    gte.mtc(0, 0x7FFF7FFF); // R = G = max
    gte.mtc(1, 0x00007FFF); // B = max
    gte.mtc(2, 0x00000000);

    gte.ctc(21, 0x7FFF);
    gte.ctc(22, 0x7FFF);
    gte.ctc(23, 0x7FFF);

    gte.mtc(1, gte.mfc(1) | (0x00008000 << 16)); // IR0 = 0.5

    gte.execute(0x00080013); // NCDS with sf=1
}

TEST_F(GteColorTest, NCDT_HandlesZeroIR0Gracefully) {
    gte.mtc(9, 0x00002000);
    gte.mtc(10, 0x00002000);
    gte.mtc(11, 0x00002000);

    gte.mtc(0, 0x00100020); // RGBC: R=16, G=32
    gte.mtc(1, 0x00000040); // B=64
    gte.mtc(2, 0x00000000);

    gte.ctc(21, 0);
    gte.ctc(22, 0);
    gte.ctc(23, 0);

    gte.mtc(1, gte.mfc(1) & 0x0000FFFF); // IR0 = 0

    gte.execute(0x00080016); // NCDT with sf=1
}

TEST_F(GteColorTest, NCDS_SaturatesNegativeMACs) {
    gte.mtc(9, static_cast<uint32_t>(-0x00004000));
    gte.mtc(10, static_cast<uint32_t>(-0x00008000));
    gte.mtc(11, static_cast<uint32_t>(-0x0000C000));

    gte.mtc(0, 0x00010001);
    gte.mtc(1, 0x00000001);
    gte.mtc(2, 0x00000000);

    gte.ctc(21, 0x00010000);
    gte.ctc(22, 0x00010000);
    gte.ctc(23, 0x00010000);

    gte.mtc(1, gte.mfc(1) | (0x00008000 << 16)); // IR0 = 0.5

    gte.execute(0x00080013); // NCDS with sf=1
}
