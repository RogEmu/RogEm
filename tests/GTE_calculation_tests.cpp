#include <gtest/gtest.h>
#include "GTE.h"

class GteCalculationTest : public ::testing::Test {
protected:
    GTE gte;

    void SetUp() override {
        gte.reset();
    }
};

TEST_F(GteCalculationTest, SQR_SquaresAllIRs_NoShift) {
    gte.mtc(9, static_cast<uint32_t>(10));
    gte.mtc(10, static_cast<uint32_t>(20));
    gte.mtc(11, static_cast<uint32_t>(30));

    gte.execute(0x00000028); // SQR with sf = 0

    EXPECT_EQ(gte.mfc(9), 100);
    EXPECT_EQ(gte.mfc(10), 400);
    EXPECT_EQ(gte.mfc(11), 900);

    EXPECT_EQ(gte.mfc(25), 100);
    EXPECT_EQ(gte.mfc(26), 400);
    EXPECT_EQ(gte.mfc(27), 900);
}

TEST_F(GteCalculationTest, SQR_SquaresAllIRs_WithShift) {
    gte.mtc(9, static_cast<uint32_t>(100));
    gte.mtc(10, static_cast<uint32_t>(200));
    gte.mtc(11, static_cast<uint32_t>(300));

    gte.execute(0x00080028); // sf = 1 (bit 19 set)

    EXPECT_EQ(gte.mfc(9), 2);
    EXPECT_EQ(gte.mfc(10), 9);
    EXPECT_EQ(gte.mfc(11), 21);

    EXPECT_EQ(gte.mfc(25), 2);
    EXPECT_EQ(gte.mfc(26), 9);
    EXPECT_EQ(gte.mfc(27), 21);
}

TEST_F(GteCalculationTest, SQR_NegativeInputsStillPositiveResults) {
    gte.mtc(9, static_cast<uint32_t>(-50));
    gte.mtc(10, static_cast<uint32_t>(-60));
    gte.mtc(11, static_cast<uint32_t>(-70));

    gte.execute(0x00000028); // sf = 0

    EXPECT_EQ(gte.mfc(9), 2500);
    EXPECT_EQ(gte.mfc(10), 3600);
    EXPECT_EQ(gte.mfc(11), 4900);
}

TEST_F(GteCalculationTest, SQR_ClampsToMaxValue) {
    gte.mtc(9, 0x7FFF);
    gte.mtc(10, 0x7FFF);
    gte.mtc(11, 0x7FFF);

    gte.execute(0x00000028);

    EXPECT_GT(static_cast<int32_t>(gte.mfc(25)), 0x7FFF);
    EXPECT_GT(static_cast<int32_t>(gte.mfc(26)), 0x7FFF);
    EXPECT_GT(static_cast<int32_t>(gte.mfc(27)), 0x7FFF);

    EXPECT_EQ(gte.mfc(9), 0x7FFF);
    EXPECT_EQ(gte.mfc(10), 0x7FFF);
    EXPECT_EQ(gte.mfc(11), 0x7FFF);
}

TEST_F(GteCalculationTest, SQR_SmallValuesClampToZeroWhenShifted) {
    gte.mtc(9, static_cast<uint32_t>(1));
    gte.mtc(10, static_cast<uint32_t>(2));
    gte.mtc(11, static_cast<uint32_t>(3));

    gte.execute(0x00080028); // sf = 1

    EXPECT_EQ(gte.mfc(9), 0);
    EXPECT_EQ(gte.mfc(10), 0);
    EXPECT_EQ(gte.mfc(11), 0);
}
