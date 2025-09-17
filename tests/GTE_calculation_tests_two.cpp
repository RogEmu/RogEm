#include "gtest/gtest.h"
#include "GTE.hpp"

// !!! IMPORTANT !!!   This file is meant to be temporary, all test here are meant to be move to GTE_calculation_tests.cpp,
// !!! IMPORTANT !!!   it was only created while waiting for the main test file to be merged in the dev branch.

class GteCalculationTestTwo : public ::testing::Test {
protected:
    GTE gte;
};

TEST_F(GteCalculationTestTwo, OP_BasicCrossProduct_NoShift) {
    // Simple cross product without shift (sf = 0)
    gte.mtc(9, static_cast<uint32_t>(1));  // IR1
    gte.mtc(10, static_cast<uint32_t>(0)); // IR2
    gte.mtc(11, static_cast<uint32_t>(0)); // IR3
    gte.ctc(0, static_cast<uint32_t>(0 << 16)); // RT11
    gte.ctc(2, static_cast<uint32_t>(1 << 16)); // RT22
    gte.ctc(4, static_cast<uint32_t>(0 << 16)); // RT33

    gte.execute(0x0000000C); // sf = 0

    EXPECT_EQ(static_cast<int32_t>(gte.mfc(25)), 0); // MAC1
    EXPECT_EQ(static_cast<int32_t>(gte.mfc(26)), 0); // MAC2
    EXPECT_EQ(static_cast<int32_t>(gte.mfc(27)), -1); // MAC3
    EXPECT_EQ(static_cast<int32_t>(gte.mfc(9)), 0); // IR1
    EXPECT_EQ(static_cast<int32_t>(gte.mfc(10)), 0); // IR2
    EXPECT_EQ(static_cast<int32_t>(gte.mfc(11)), -1); // IR3
}

TEST_F(GteCalculationTestTwo, OP_BasicCrossProduct_WithShift) {
    // Cross product with sf = 1, right shift by 12
    gte.mtc(9, static_cast<uint32_t>(1));  // IR1
    gte.mtc(10, static_cast<uint32_t>(0)); // IR2
    gte.mtc(11, static_cast<uint32_t>(0)); // IR3
    gte.ctc(0, static_cast<uint32_t>(0 << 16)); // RT11
    gte.ctc(2, static_cast<uint32_t>(1 << 16)); // RT22
    gte.ctc(4, static_cast<uint32_t>(0 << 16)); // RT33

    gte.execute(0x0008000C); // sf = 1

    EXPECT_EQ(static_cast<int32_t>(gte.mfc(25)), 0); // MAC1
    EXPECT_EQ(static_cast<int32_t>(gte.mfc(26)), 0); // MAC2
    EXPECT_EQ(static_cast<int32_t>(gte.mfc(27)), -1); // MAC3
    EXPECT_EQ(static_cast<int32_t>(gte.mfc(9)), 0); // IR1
    EXPECT_EQ(static_cast<int32_t>(gte.mfc(10)), 0); // IR2
    EXPECT_EQ(static_cast<int32_t>(gte.mfc(11)), -1); // IR3
}

TEST_F(GteCalculationTestTwo, OP_NegativeVector_WithShift) {
    // Cross product with negative values and sf = 1
    gte.mtc(9, static_cast<uint32_t>(-1));  // IR1
    gte.mtc(10, static_cast<uint32_t>(0)); // IR2
    gte.mtc(11, static_cast<uint32_t>(0)); // IR3
    gte.ctc(0, static_cast<uint32_t>(0 << 16)); // RT11
    gte.ctc(2, static_cast<uint32_t>(1 << 16)); // RT22
    gte.ctc(4, static_cast<uint32_t>(0 << 16)); // RT33

    gte.execute(0x0008000C); // sf = 1

    EXPECT_EQ(static_cast<int32_t>(gte.mfc(25)), 0); // MAC1
    EXPECT_EQ(static_cast<int32_t>(gte.mfc(26)), 0); // MAC2
    EXPECT_EQ(static_cast<int32_t>(gte.mfc(27)), 0); // MAC3
    EXPECT_EQ(static_cast<int32_t>(gte.mfc(9)), 0); // IR1
    EXPECT_EQ(static_cast<int32_t>(gte.mfc(10)), 0); // IR2
    EXPECT_EQ(static_cast<int32_t>(gte.mfc(11)), 0); // IR3
}

TEST_F(GteCalculationTestTwo, OP_ResultClampedToMax) {
    // Large input that causes result > 0x7FFF
    // The cross product is [0, 0, IR2 * D1 = 32767 * 32767 = ~1 billion], should be clamped
    gte.mtc(9, static_cast<uint32_t>(0));  // IR1
    gte.mtc(10, static_cast<uint32_t>(32767)); // IR2
    gte.mtc(11, static_cast<uint32_t>(0)); // IR3
    gte.ctc(0, static_cast<uint32_t>(32767 << 16)); // RT11
    gte.ctc(2, static_cast<uint32_t>(0 << 16)); // RT22
    gte.ctc(4, static_cast<uint32_t>(0 << 16)); // RT33

    gte.execute(0x0000000C); // sf = 0

    EXPECT_EQ(static_cast<int32_t>(gte.mfc(25)), 0); // MAC1
    EXPECT_EQ(static_cast<int32_t>(gte.mfc(26)), 0); // MAC2
    EXPECT_EQ(static_cast<int32_t>(gte.mfc(27)), 1073676289); // MAC3 (Not Clamped)
    EXPECT_EQ(static_cast<int32_t>(gte.mfc(9)), 0); // IR1
    EXPECT_EQ(static_cast<int32_t>(gte.mfc(10)), 0); // IR2
    EXPECT_EQ(static_cast<int32_t>(gte.mfc(11)), 32767); // IR3 (MAC3 Clamped)
}

TEST_F(GteCalculationTestTwo, OP_CrossProductOfParallelVectors) {
    // Cross product of two parallel vectors should be zero
    gte.mtc(9, static_cast<uint32_t>(1000));  // IR1
    gte.mtc(10, static_cast<uint32_t>(1000)); // IR2
    gte.mtc(11, static_cast<uint32_t>(1000)); // IR3
    gte.ctc(0, static_cast<uint32_t>(2000 << 16)); // RT11
    gte.ctc(2, static_cast<uint32_t>(2000 << 16)); // RT22
    gte.ctc(4, static_cast<uint32_t>(2000 << 16)); // RT33

    gte.execute(0x0000000C); // sf = 0

    EXPECT_EQ(static_cast<int32_t>(gte.mfc(25)), 0); // MAC1
    EXPECT_EQ(static_cast<int32_t>(gte.mfc(26)), 0); // MAC2
    EXPECT_EQ(static_cast<int32_t>(gte.mfc(27)), 0); // MAC3
    EXPECT_EQ(static_cast<int32_t>(gte.mfc(9)), 0); // IR1
    EXPECT_EQ(static_cast<int32_t>(gte.mfc(10)), 0); // IR2
    EXPECT_EQ(static_cast<int32_t>(gte.mfc(11)), 0); // IR3
}

TEST_F(GteCalculationTestTwo, OP_CrossProductOfParallelVectors_OppositeDirection) {
    // Cross product of two parallel but opposite vectors should be zero
    gte.mtc(9, static_cast<uint32_t>(1000));  // IR1
    gte.mtc(10, static_cast<uint32_t>(1000)); // IR2
    gte.mtc(11, static_cast<uint32_t>(1000)); // IR3
    gte.ctc(0, static_cast<uint32_t>(static_cast<uint16_t>(-1000) << 16)); // RT11
    gte.ctc(2, static_cast<uint32_t>(static_cast<uint16_t>(-1000) << 16)); // RT22
    gte.ctc(4, static_cast<uint32_t>(static_cast<uint16_t>(-1000) << 16)); // RT33

    gte.execute(0x0000000C); // sf = 0

    EXPECT_EQ(static_cast<int32_t>(gte.mfc(25)), 0); // MAC1
    EXPECT_EQ(static_cast<int32_t>(gte.mfc(26)), 0); // MAC2
    EXPECT_EQ(static_cast<int32_t>(gte.mfc(27)), 0); // MAC3
    EXPECT_EQ(static_cast<int32_t>(gte.mfc(9)), 0); // IR1
    EXPECT_EQ(static_cast<int32_t>(gte.mfc(10)), 0); // IR2
    EXPECT_EQ(static_cast<int32_t>(gte.mfc(11)), 0); // IR3
}

TEST_F(GteCalculationTestTwo, OP_CrossProduct_GeneralVectors_NoShift) {
    // Cross product of [1,2,3] and [4,5,6] without shift
    gte.mtc(9, static_cast<uint32_t>(1));  // IR1
    gte.mtc(10, static_cast<uint32_t>(2)); // IR2
    gte.mtc(11, static_cast<uint32_t>(3)); // IR3
    gte.ctc(0, static_cast<uint32_t>(4 << 16)); // RT11
    gte.ctc(2, static_cast<uint32_t>(5 << 16)); // RT22
    gte.ctc(4, static_cast<uint32_t>(6 << 16)); // RT33

    gte.execute(0x0000000C); // sf = 0

    EXPECT_EQ(static_cast<int32_t>(gte.mfc(25)), 3); // MAC1
    EXPECT_EQ(static_cast<int32_t>(gte.mfc(26)), -6); // MAC2
    EXPECT_EQ(static_cast<int32_t>(gte.mfc(27)), 3); // MAC3
    EXPECT_EQ(static_cast<int32_t>(gte.mfc(9)), 3); // IR1
    EXPECT_EQ(static_cast<int32_t>(gte.mfc(10)), -6); // IR2
    EXPECT_EQ(static_cast<int32_t>(gte.mfc(11)), 3); // IR3
}

TEST_F(GteCalculationTestTwo, OP_CrossProduct_GeneralVectors_WithShift) {
    // Cross product of large values with shift reducing result
    gte.mtc(9, static_cast<uint32_t>(4096));  // IR1
    gte.mtc(10, static_cast<uint32_t>(8192)); // IR2
    gte.mtc(11, static_cast<uint32_t>(12288)); // IR3
    gte.ctc(0, static_cast<uint32_t>(2048 << 16)); // RT11
    gte.ctc(2, static_cast<uint32_t>(1024 << 16)); // RT22
    gte.ctc(4, static_cast<uint32_t>(512 << 16)); // RT33

    gte.execute(0x0008000C); // sf = 1

    EXPECT_EQ(static_cast<int32_t>(gte.mfc(25)), 2048); // MAC1
    EXPECT_EQ(static_cast<int32_t>(gte.mfc(26)), -5632); // MAC2
    EXPECT_EQ(static_cast<int32_t>(gte.mfc(27)), 3072); // MAC3
    EXPECT_EQ(static_cast<int32_t>(gte.mfc(9)), 2048); // IR1
    EXPECT_EQ(static_cast<int32_t>(gte.mfc(10)), -5632); // IR2
    EXPECT_EQ(static_cast<int32_t>(gte.mfc(11)), 3072); // IR3
}

TEST_F(GteCalculationTestTwo, OP_CrossProduct_ResultBecomesZeroWithShift) {
    // Small values result in zero after shift (sf=1)
    gte.mtc(9, static_cast<uint32_t>(1));  // IR1
    gte.mtc(10, static_cast<uint32_t>(2)); // IR2
    gte.mtc(11, static_cast<uint32_t>(3)); // IR3
    gte.ctc(0, static_cast<uint32_t>(4 << 16)); // RT11
    gte.ctc(2, static_cast<uint32_t>(5 << 16)); // RT22
    gte.ctc(4, static_cast<uint32_t>(6 << 16)); // RT33

    gte.execute(0x0008000C); // sf = 1

    EXPECT_EQ(static_cast<int32_t>(gte.mfc(25)), 0); // MAC1
    EXPECT_EQ(static_cast<int32_t>(gte.mfc(26)), -1); // MAC2
    EXPECT_EQ(static_cast<int32_t>(gte.mfc(27)), 0); // MAC3
    EXPECT_EQ(static_cast<int32_t>(gte.mfc(9)), 0); // IR1
    EXPECT_EQ(static_cast<int32_t>(gte.mfc(10)), -1); // IR2
    EXPECT_EQ(static_cast<int32_t>(gte.mfc(11)), 0); // IR3
}
