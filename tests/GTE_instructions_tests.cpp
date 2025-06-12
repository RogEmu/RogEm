#include <gtest/gtest.h>
#include "GTE.h"

class GteInstructionTest : public ::testing::Test {
protected:
    GTE gte;

    void SetUp() override {
        gte.reset();
    }
};

TEST_F(GteInstructionTest, MTC2AndMFC2_BasicTransfer) {
    uint8_t reg = 5;
    uint32_t value = 0x12345678;

    gte.mtc(reg, value);
    EXPECT_EQ(gte.mfc(reg), value);
}

TEST_F(GteInstructionTest, CTC2AndCFC2_BasicTransfer) {
    uint8_t reg = 7;
    uint32_t value = 0xABCDEF12;

    gte.ctc(reg, value);
    EXPECT_EQ(gte.cfc(reg), value);
}

/* OBSOLETE TO REDO
TEST_F(GteInstructionTest, RTPS_ExecutesWithExpectedProjection) {
    gte.mtc(0, (2 << 16) | 1);
    gte.mtc(1, 3);

    gte.ctc(0, (0x1000 << 16)); // R11 = 1.0, R12 = 0
    gte.ctc(1, 0x00000000);     // R13 = 0, R21 = 0
    gte.ctc(2, (0x1000 << 16)); // R22 = 1.0, R23 = 0
    gte.ctc(3, 0x00000000);     // R31 = 0, R32 = 0
    gte.ctc(4, (0x1000 << 16)); // R33 = 1.0

    gte.ctc(5, 0);  // TRX
    gte.ctc(6, 0);  // TRY
    gte.ctc(7, 0);  // TRZ
    
    gte.ctc(24, 100);   // OFX
    gte.ctc(25, 100);   // OFY
    gte.ctc(26, 100);   // H

    gte.execute(0x00000001);

    EXPECT_EQ(gte.mfc(19), 48);
    EXPECT_EQ(gte.mfc(7) & 0xFFFF, 48);

    uint32_t sxy2 = gte.mfc(14);
    uint16_t sx = sxy2 & 0xFFFF;
    uint16_t sy = sxy2 >> 16;
    EXPECT_GT(sx, 100);
    EXPECT_GT(sy, 100);
}

TEST_F(GteInstructionTest, RTPS_HandlesZeroOrNegativeZCorrectly) {
    gte.mtc(0, (2 << 16) | 1);
    gte.mtc(1, static_cast<uint32_t>(-5));

    gte.ctc(0, (0x1000 << 16));
    gte.ctc(1, 0x00000000);
    gte.ctc(2, (0x1000 << 16));
    gte.ctc(3, 0x00000000);
    gte.ctc(4, (0x1000 << 16));
    gte.ctc(5, 0);
    gte.ctc(6, 0);
    gte.ctc(7, 0);
    gte.ctc(24, 100);
    gte.ctc(25, 100);
    gte.ctc(26, 100);

    gte.execute(0x00000001);

    EXPECT_LT(static_cast<int32_t>(gte.mfc(19)), 0);
    EXPECT_LT(static_cast<int32_t>(gte.mfc(7) & 0xFFFF), 0xFFFF);

    uint32_t sxy2 = gte.mfc(14);
    uint16_t sx = sxy2 & 0xFFFF;
    uint16_t sy = sxy2 >> 16;
    EXPECT_GE(sx, 100);
    EXPECT_GE(sy, 100);
}
*/

TEST_F(GteInstructionTest, ClampMAC_SetsFlagWhenOverflowingIR) {
    int64_t overflowed = static_cast<int64_t>(GTE::IR_LIMIT_HIGH) + 1000;
    int32_t clamped = gte.clampMAC(overflowed, GTE::IR_LIMIT_HIGH, GTE::IR_LIMIT_LOW, 0x01, 0x01);
    EXPECT_EQ(clamped, GTE::IR_LIMIT_HIGH);
    EXPECT_TRUE(gte.cfc(31) & 0x01);
}

TEST_F(GteInstructionTest, ClampMAC_SetsFlagWhenUnderflowingIR) {
    int64_t underflowed = static_cast<int64_t>(GTE::IR_LIMIT_LOW) - 1000;
    int32_t clamped = gte.clampMAC(underflowed, GTE::IR_LIMIT_HIGH, GTE::IR_LIMIT_LOW, 0x02, 0x02);
    EXPECT_EQ(clamped, GTE::IR_LIMIT_LOW);
    EXPECT_TRUE(gte.cfc(31) & 0x02);
}

/* OBSOLETE TO REDO
TEST_F(GteInstructionTest, RTPT_ExecutesWithThreeProjectedPoints) {
    gte.mtc(0, (10 << 16) | 5);
    gte.mtc(1, 3);
    gte.mtc(2, (20 << 16) | 15);
    gte.mtc(3, 3);
    gte.mtc(4, (30 << 16) | 25);
    gte.mtc(5, 3);

    gte.ctc(0, (0x1000 << 16));
    gte.ctc(1, 0x00000000);
    gte.ctc(2, (0x1000 << 16));
    gte.ctc(3, 0x00000000);
    gte.ctc(4, (0x1000 << 16));
    gte.ctc(5, 0);
    gte.ctc(6, 0);
    gte.ctc(7, 0);
    gte.ctc(24, 100);
    gte.ctc(25, 100);
    gte.ctc(26, 100);

    gte.execute(0x00000030);

    EXPECT_EQ(gte.mfc(16), 48);
    EXPECT_EQ(gte.mfc(17), 48);
    EXPECT_EQ(gte.mfc(18), 48);
    EXPECT_EQ(gte.mfc(19), 48);
    EXPECT_EQ(gte.mfc(7) & 0xFFFF, 48);

    uint32_t sxy0 = gte.mfc(12);
    uint32_t sxy1 = gte.mfc(13);
    uint32_t sxy2 = gte.mfc(14);
    EXPECT_GT(sxy0 & 0xFFFF, 100);
    EXPECT_GT(sxy1 & 0xFFFF, 100);
    EXPECT_GT(sxy2 & 0xFFFF, 100);
}

TEST_F(GteInstructionTest, RTPT_HandlesZeroOrNegativeZCorrectly) {
    gte.mtc(0, (10 << 16) | 5);
    gte.mtc(1, 0);
    gte.mtc(2, (20 << 16) | 15);
    gte.mtc(3, static_cast<uint32_t>(-1));
    gte.mtc(4, (30 << 16) | 25);
    gte.mtc(5, 0);

    gte.ctc(0, (0x1000 << 16));
    gte.ctc(1, 0x00000000);
    gte.ctc(2, (0x1000 << 16));
    gte.ctc(3, 0x00000000);
    gte.ctc(4, (0x1000 << 16));
    gte.ctc(5, 0);
    gte.ctc(6, 0);
    gte.ctc(7, 0);
    gte.ctc(24, 100);
    gte.ctc(25, 100);
    gte.ctc(26, 100);

    gte.execute(0x00000030);

    uint32_t sxy0 = gte.mfc(12);
    uint32_t sxy1 = gte.mfc(13);
    uint32_t sxy2 = gte.mfc(14);

    EXPECT_GT(sxy0 & 0xFFFF, 100);
    EXPECT_GT(sxy1 & 0xFFFF, 100);
    EXPECT_GT(sxy2 & 0xFFFF, 100);
}
*/
