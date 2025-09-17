#include <gtest/gtest.h>

#include "Core/GTE.hpp"

class GteProjectionTest : public ::testing::Test {
protected:
    GTE gte;

    void SetUp() override {
        gte.reset();
    }
};

TEST_F(GteProjectionTest, AVSZ3_BasicAverageWithZSF3Equals1) {
    gte.mtc(16, static_cast<uint32_t>(100)); // SZ1
    gte.mtc(17, static_cast<uint32_t>(200)); // SZ2
    gte.mtc(18, static_cast<uint32_t>(300)); // SZ3
    gte.ctc(20, 0x0001); // ZSF3 = 1

    gte.execute(0x0000002D); // AVSZ3 opcode

    uint16_t expectedOTZ = static_cast<uint16_t>(((100 + 200 + 300) * 1) >> 12);
    EXPECT_EQ(gte.mfc(7) & 0xFFFF, expectedOTZ);

    int32_t expectedMAC0 = (100 + 200 + 300) * 1;
    EXPECT_EQ(gte.mfc(24), expectedMAC0);
}

TEST_F(GteProjectionTest, AVSZ3_WithZSF3ScalingFactor) {
    gte.mtc(16, static_cast<uint32_t>(500)); // SZ1
    gte.mtc(17, static_cast<uint32_t>(500)); // SZ2
    gte.mtc(18, static_cast<uint32_t>(500)); // SZ3
    gte.ctc(20, 0x0010); // ZSF3 = 16

    gte.execute(0x0000002D);

    int64_t sum = (500 + 500 + 500) * 16;
    uint16_t expectedOTZ = static_cast<uint16_t>(std::min<int64_t>(sum >> 12, 0xFFFF));
    EXPECT_EQ(gte.mfc(7) & 0xFFFF, expectedOTZ);

    int32_t expectedMAC0 = static_cast<int32_t>(sum);
    EXPECT_EQ(gte.mfc(24), expectedMAC0);
}

TEST_F(GteProjectionTest, AVSZ3_ClampsToMaximumWhenOverflowing) {
    gte.mtc(16, 0x7FFFFFFF); // SZ1
    gte.mtc(17, 0x7FFFFFFF); // SZ2
    gte.mtc(18, 0x7FFFFFFF); // SZ3
    gte.ctc(20, 0xFFFF); // ZSF3 = 65535

    gte.execute(0x0000002D);

    EXPECT_EQ(gte.mfc(7) & 0xFFFF, 0xFFFF); // Clamped to max

    int64_t sum = (static_cast<int64_t>(0x7FFFFFFF) * 3) * 65535;
    int32_t expectedMAC0 = static_cast<int32_t>(sum);
    EXPECT_EQ(gte.mfc(24), expectedMAC0);
}

TEST_F(GteProjectionTest, AVSZ3_ClampsToZeroWhenNegative) {
    gte.mtc(16, static_cast<uint32_t>(-500)); // SZ1
    gte.mtc(17, static_cast<uint32_t>(-500)); // SZ2
    gte.mtc(18, static_cast<uint32_t>(-500)); // SZ3
    gte.ctc(20, 0x0001); // ZSF3 = 1

    gte.execute(0x0000002D);

    EXPECT_EQ(gte.mfc(7) & 0xFFFF, 0x0000); // Clamped to 0

    int32_t expectedMAC0 = (static_cast<int32_t>(-500) * 3) * 1;
    EXPECT_EQ(gte.mfc(24), expectedMAC0);
}

TEST_F(GteProjectionTest, AVSZ3_MAC0_StoresUnclampedResult) {
    gte.mtc(16, static_cast<uint32_t>(100));
    gte.mtc(17, static_cast<uint32_t>(100));
    gte.mtc(18, static_cast<uint32_t>(100));
    gte.ctc(20, 0x0020); // ZSF3 = 32

    gte.execute(0x0000002D);

    int32_t expectedMAC0 = (100 + 100 + 100) * 32;
    EXPECT_EQ(gte.mfc(24), expectedMAC0);

    uint16_t expectedOTZ = static_cast<uint16_t>(std::min<int32_t>(expectedMAC0 >> 12, 0xFFFF));
    EXPECT_EQ(gte.mfc(7) & 0xFFFF, expectedOTZ);
}

TEST_F(GteProjectionTest, AVSZ4_BasicAverageWithZSF4Equals1) {
    gte.mtc(15, static_cast<uint32_t>(100)); // SZ0
    gte.mtc(16, static_cast<uint32_t>(200)); // SZ1
    gte.mtc(17, static_cast<uint32_t>(300)); // SZ2
    gte.mtc(18, static_cast<uint32_t>(400)); // SZ3
    gte.ctc(21, 0x0001); // ZSF4 = 1

    gte.execute(0x0000002E); // AVSZ4 opcode

    uint16_t expectedOTZ = static_cast<uint16_t>(((100 + 200 + 300 + 400) * 1) >> 12);
    EXPECT_EQ(gte.mfc(7) & 0xFFFF, expectedOTZ);

    int32_t expectedMAC0 = (100 + 200 + 300 + 400) * 1;
    EXPECT_EQ(gte.mfc(24), expectedMAC0);
}

TEST_F(GteProjectionTest, AVSZ4_WithZSF4ScalingFactor) {
    gte.mtc(15, static_cast<uint32_t>(500));
    gte.mtc(16, static_cast<uint32_t>(500));
    gte.mtc(17, static_cast<uint32_t>(500));
    gte.mtc(18, static_cast<uint32_t>(500));
    gte.ctc(21, 0x0010); // ZSF4 = 16

    gte.execute(0x0000002E);

    int64_t sum = (500 + 500 + 500 + 500) * 16;
    uint16_t expectedOTZ = static_cast<uint16_t>(std::min<int64_t>(sum >> 12, 0xFFFF));
    EXPECT_EQ(gte.mfc(7) & 0xFFFF, expectedOTZ);

    int32_t expectedMAC0 = static_cast<int32_t>(sum);
    EXPECT_EQ(gte.mfc(24), expectedMAC0);
}

TEST_F(GteProjectionTest, AVSZ4_ClampsToMaximumWhenOverflowing) {
    gte.mtc(15, 0x7FFFFFFF);
    gte.mtc(16, 0x7FFFFFFF);
    gte.mtc(17, 0x7FFFFFFF);
    gte.mtc(18, 0x7FFFFFFF);
    gte.ctc(21, 0xFFFF); // ZSF4 = 65535

    gte.execute(0x0000002E);

    EXPECT_EQ(gte.mfc(7) & 0xFFFF, 0xFFFF); // Clamped to max

    int64_t sum = (static_cast<int64_t>(0x7FFFFFFF) * 4) * 65535;
    int32_t expectedMAC0 = static_cast<int32_t>(sum);
    EXPECT_EQ(gte.mfc(24), expectedMAC0);
}

TEST_F(GteProjectionTest, AVSZ4_ClampsToZeroWhenNegative) {
    gte.mtc(15, static_cast<uint32_t>(-500));
    gte.mtc(16, static_cast<uint32_t>(-500));
    gte.mtc(17, static_cast<uint32_t>(-500));
    gte.mtc(18, static_cast<uint32_t>(-500));
    gte.ctc(21, 0x0001); // ZSF4 = 1

    gte.execute(0x0000002E);

    EXPECT_EQ(gte.mfc(7) & 0xFFFF, 0x0000); // Clamped to 0

    int32_t expectedMAC0 = (static_cast<int32_t>(-500) * 4) * 1;
    EXPECT_EQ(gte.mfc(24), expectedMAC0);
}

TEST_F(GteProjectionTest, AVSZ4_MAC0_StoresUnclampedResult) {
    gte.mtc(15, static_cast<uint32_t>(100));
    gte.mtc(16, static_cast<uint32_t>(100));
    gte.mtc(17, static_cast<uint32_t>(100));
    gte.mtc(18, static_cast<uint32_t>(100));
    gte.ctc(21, 0x0020); // ZSF4 = 32

    gte.execute(0x0000002E);

    int32_t expectedMAC0 = (100 + 100 + 100 + 100) * 32;
    EXPECT_EQ(gte.mfc(24), expectedMAC0); // MAC0

    uint16_t expectedOTZ = static_cast<uint16_t>(std::min<int32_t>(expectedMAC0 >> 12, 0xFFFF));
    EXPECT_EQ(gte.mfc(7) & 0xFFFF, expectedOTZ);
}
