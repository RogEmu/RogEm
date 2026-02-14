#include <gtest/gtest.h>

#include "Core/GTE.hpp"

/**
 * Test suite for CC (Color Color) and CDP (Color Depth Cue) instructions
 *
 * CC instruction:
 *   - Applies background color and color matrix to IR values
 *   - Multiplies result by RGBC components and shifts
 *   - Does NOT use normal/lighting transformation
 *
 * CDP instruction:
 *   - Applies background color and color matrix to IR values
 *   - Interpolates with far color using IR0 for depth cueing
 *   - Does NOT use normal/lighting transformation
 */
class GteCCCDPTest : public ::testing::Test {
protected:
    GTE gte;

    void SetUp() override {
        gte.reset();
    }

    /**
     * Helper to set background color (BK) components
     * BK is stored in ctrl registers 6-8 as signed 16-bit values
     */
    void setBackgroundColor(int16_t r, int16_t g, int16_t b) {
        gte.ctc(6, static_cast<uint32_t>(r) & 0xFFFF);
        gte.ctc(7, static_cast<uint32_t>(g) & 0xFFFF);
        gte.ctc(8, static_cast<uint32_t>(b) & 0xFFFF);
    }

    /**
     * Helper to set color matrix (LCM)
     * LCM is a 3x3 matrix stored in ctrl registers 3-5
     */
    void setColorMatrix(int16_t r11, int16_t r12, int16_t r13,
                        int16_t r21, int16_t r22, int16_t r23,
                        int16_t r31, int16_t r32, int16_t r33) {
        gte.ctc(3, (static_cast<uint32_t>(static_cast<uint16_t>(r11)) << 16) | (static_cast<uint16_t>(r12)));
        gte.ctc(4, (static_cast<uint32_t>(static_cast<uint16_t>(r13)) << 16) | (static_cast<uint16_t>(r21)));
        gte.ctc(5, (static_cast<uint32_t>(static_cast<uint16_t>(r22)) << 16) | (static_cast<uint16_t>(r23)));
        gte.ctc(6, (static_cast<uint32_t>(static_cast<uint16_t>(r31)) << 16) | (static_cast<uint16_t>(r32)));
        gte.ctc(7, (static_cast<uint32_t>(static_cast<uint16_t>(r33)) << 16));
    }

    /**
     * Helper to set IR values (intermediate results)
     * IR1, IR2, IR3 are in data registers 9-11
     */
    void setIR(int16_t ir1, int16_t ir2, int16_t ir3) {
        gte.mtc(9, static_cast<int32_t>(ir1));
        gte.mtc(10, static_cast<int32_t>(ir2));
        gte.mtc(11, static_cast<int32_t>(ir3));
    }

    /**
     * Helper to set RGBC (color register)
     * RGBC is in data register 0 (R, G) and 1 (B, CODE)
     */
    void setRGBC(uint8_t r, uint8_t g, uint8_t b, uint8_t code) {
        gte.mtc(0, (static_cast<uint32_t>(g) << 16) | r);
        gte.mtc(1, (static_cast<uint32_t>(code) << 16) | b);
    }

    /**
     * Helper to set IR0 (interpolation factor for depth cueing)
     * IR0 is the upper 16 bits of data register 1
     */
    void setIR0(int16_t ir0) {
        uint32_t reg1 = gte.mfc(1);
        reg1 = (reg1 & 0xFFFF) | (static_cast<uint32_t>(static_cast<uint16_t>(ir0)) << 16);
        gte.mtc(1, reg1);
    }

    /**
     * Helper to set far color (FC) for depth cueing
     * FC is stored in ctrl registers 21-23
     */
    void setFarColor(int32_t r, int32_t g, int32_t b) {
        gte.ctc(21, r);
        gte.ctc(22, g);
        gte.ctc(23, b);
    }
};

// ==========================
// CC (Color Color) Tests
// ==========================

TEST_F(GteCCCDPTest, CC_BasicColorMultiplication_NoShift) {
    // Set up IR values
    setIR(0x1000, 0x2000, 0x3000);

    // Set background color
    setBackgroundColor(0, 0, 0);

    // Set identity color matrix
    setColorMatrix(0x1000, 0, 0,
                   0, 0x1000, 0,
                   0, 0, 0x1000);

    // Set RGBC values
    setRGBC(0x80, 0x40, 0x20, 0xAA);

    // Execute CC with sf=0 (no shift)
    gte.execute(0x0000001C); // CC opcode with sf=0
}

TEST_F(GteCCCDPTest, CC_BasicColorMultiplication_WithShift) {
    // Set up IR values
    setIR(0x2000, 0x3000, 0x4000);

    // Set background color (small values)
    setBackgroundColor(0x10, 0x20, 0x30);

    // Set identity color matrix
    setColorMatrix(0x1000, 0, 0,
                   0, 0x1000, 0,
                   0, 0, 0x1000);

    // Set RGBC values
    setRGBC(0xFF, 0x80, 0x40, 0xBB);

    // Execute CC with sf=1 (with shift >> 12)
    gte.execute(0x0008001C); // CC opcode with sf=1
}

TEST_F(GteCCCDPTest, CC_ColorMatrixTransformation) {
    // Set up IR values
    setIR(0x1000, 0x1000, 0x1000);

    // Set zero background
    setBackgroundColor(0, 0, 0);

    // Set non-identity color matrix (scales R by 2, G by 3, B by 4)
    setColorMatrix(0x2000, 0, 0,
                   0, 0x3000, 0,
                   0, 0, 0x4000);

    // Set uniform RGBC values
    setRGBC(0x80, 0x80, 0x80, 0xCC);

    // Execute CC with sf=1
    gte.execute(0x0008001C);
}

TEST_F(GteCCCDPTest, CC_BackgroundColorAddition) {
    // Set up small IR values
    setIR(0x100, 0x100, 0x100);

    // Set significant background color
    setBackgroundColor(0x1000, 0x2000, 0x3000);

    // Set identity color matrix
    setColorMatrix(0x1000, 0, 0,
                   0, 0x1000, 0,
                   0, 0, 0x1000);

    // Set RGBC values
    setRGBC(0x80, 0x80, 0x80, 0xDD);

    // Execute CC with sf=1
    gte.execute(0x0008001C);
}

TEST_F(GteCCCDPTest, CC_ClampingOnOverflow) {
    // Set up maximum IR values
    setIR(0x7FFF, 0x7FFF, 0x7FFF);

    // Set maximum background
    setBackgroundColor(0x7FFF, 0x7FFF, 0x7FFF);

    // Set maximum color matrix values
    setColorMatrix(0x7FFF, 0, 0,
                   0, 0x7FFF, 0,
                   0, 0, 0x7FFF);

    // Set maximum RGBC values
    setRGBC(0xFF, 0xFF, 0xFF, 0xEE);

    // Execute CC with sf=1
    gte.execute(0x0008001C);
}

TEST_F(GteCCCDPTest, CC_NegativeIRValues) {
    // Set up negative IR values
    setIR(-0x1000, -0x2000, -0x3000);

    // Set background color
    setBackgroundColor(0x1000, 0x2000, 0x3000);

    // Set identity color matrix
    setColorMatrix(0x1000, 0, 0,
                   0, 0x1000, 0,
                   0, 0, 0x1000);

    // Set RGBC values
    setRGBC(0x80, 0x80, 0x80, 0xFF);

    // Execute CC with sf=1
    gte.execute(0x0008001C);
}

TEST_F(GteCCCDPTest, CC_CodePreservation) {
    // Set up IR values and colors
    setIR(0x1000, 0x1000, 0x1000);
    setBackgroundColor(0, 0, 0);
    setColorMatrix(0x1000, 0, 0, 0, 0x1000, 0, 0, 0, 0x1000);

    // Set specific CODE value
    uint8_t expectedCode = 0xAB;
    setRGBC(0x80, 0x80, 0x80, expectedCode);

    // Execute CC
    gte.execute(0x0008001C);
}

// ==========================
// CDP (Color Depth Cue) Tests
// ==========================

TEST_F(GteCCCDPTest, CDP_BasicDepthInterpolation) {
    // Set up IR values
    setIR(0x1000, 0x2000, 0x3000);

    // Set background color
    setBackgroundColor(0, 0, 0);

    // Set identity color matrix
    setColorMatrix(0x1000, 0, 0,
                   0, 0x1000, 0,
                   0, 0, 0x1000);

    // Set far color (target color for depth cueing)
    setFarColor(0x4000, 0x5000, 0x6000);

    // Set IR0 to 0.5 (interpolation factor) - 0x1000 in 4.12 fixed point = 0.25
    setIR0(0x2000); // 0.5 in fixed point

    // Set RGBC
    setRGBC(0x80, 0x80, 0x80, 0xCD);

    // Execute CDP with sf=1
    gte.execute(0x00000014); // CDP opcode with sf=0
}

TEST_F(GteCCCDPTest, CDP_NoInterpolationWhenIR0IsZero) {
    // Set up IR values
    setIR(0x1000, 0x2000, 0x3000);

    // Set background color
    setBackgroundColor(0, 0, 0);

    // Set identity color matrix
    setColorMatrix(0x1000, 0, 0,
                   0, 0x1000, 0,
                   0, 0, 0x1000);

    // Set far color
    setFarColor(0x7FFF, 0x7FFF, 0x7FFF);

    // Set IR0 to 0 (no interpolation)
    setIR0(0);

    // Set RGBC
    setRGBC(0x80, 0x80, 0x80, 0xDE);

    // Execute CDP with sf=1
    gte.execute(0x00080014); // CDP with sf=1
}

TEST_F(GteCCCDPTest, CDP_FullInterpolationWhenIR0IsMax) {
    // Set up IR values
    setIR(0x100, 0x200, 0x300);

    // Set background color
    setBackgroundColor(0, 0, 0);

    // Set identity color matrix
    setColorMatrix(0x1000, 0, 0,
                   0, 0x1000, 0,
                   0, 0, 0x1000);

    // Set significant far color
    setFarColor(0x4000, 0x5000, 0x6000);

    // Set IR0 to max (0x7FFF) - full interpolation to far color
    setIR0(0x7FFF);

    // Set RGBC
    setRGBC(0x80, 0x80, 0x80, 0xEF);

    // Execute CDP with sf=1
    gte.execute(0x00080014); // CDP with sf=1
}

TEST_F(GteCCCDPTest, CDP_WithBackgroundAndColorMatrix) {
    // Set up IR values
    setIR(0x800, 0x800, 0x800);

    // Set non-zero background
    setBackgroundColor(0x400, 0x800, 0xC00);

    // Set scaling color matrix
    setColorMatrix(0x2000, 0, 0,
                   0, 0x3000, 0,
                   0, 0, 0x4000);

    // Set far color
    setFarColor(0x2000, 0x3000, 0x4000);

    // Set moderate IR0
    setIR0(0x1000); // 0.25 in fixed point

    // Set RGBC
    setRGBC(0x80, 0x80, 0x80, 0xF0);

    // Execute CDP with sf=1
    gte.execute(0x00080014);
}

TEST_F(GteCCCDPTest, CDP_ClampingBehavior) {
    // Set up maximum IR values
    setIR(0x7FFF, 0x7FFF, 0x7FFF);

    // Set maximum background
    setBackgroundColor(0x7FFF, 0x7FFF, 0x7FFF);

    // Set maximum color matrix
    setColorMatrix(0x7FFF, 0, 0,
                   0, 0x7FFF, 0,
                   0, 0, 0x7FFF);

    // Set maximum far color
    setFarColor(0x7FFFFFFF, 0x7FFFFFFF, 0x7FFFFFFF);

    // Set maximum IR0
    setIR0(0x7FFF);

    // Set maximum RGBC
    setRGBC(0xFF, 0xFF, 0xFF, 0xFF);

    // Execute CDP with sf=1
    gte.execute(0x00080014);
}

TEST_F(GteCCCDPTest, CDP_NegativeFarColor) {
    // Set up IR values
    setIR(0x2000, 0x2000, 0x2000);

    // Set zero background
    setBackgroundColor(0, 0, 0);

    // Set identity color matrix
    setColorMatrix(0x1000, 0, 0,
                   0, 0x1000, 0,
                   0, 0, 0x1000);

    // Set negative far color (darker regions)
    setFarColor(-0x1000, -0x2000, -0x3000);

    // Set IR0 for interpolation
    setIR0(0x2000); // 0.5

    // Set RGBC
    setRGBC(0x80, 0x80, 0x80, 0x12);

    // Execute CDP with sf=1
    gte.execute(0x00080014);
}

TEST_F(GteCCCDPTest, CDP_CodePreservation) {
    // Set up values
    setIR(0x1000, 0x1000, 0x1000);
    setBackgroundColor(0, 0, 0);
    setColorMatrix(0x1000, 0, 0, 0, 0x1000, 0, 0, 0, 0x1000);
    setFarColor(0x2000, 0x2000, 0x2000);
    setIR0(0x1000);

    // Set specific CODE value
    uint8_t expectedCode = 0x5C;
    setRGBC(0x80, 0x80, 0x80, expectedCode);

    // Execute CDP
    gte.execute(0x00080014);
}

// ==========================
// Edge Cases and Comparisons
// ==========================

TEST_F(GteCCCDPTest, CC_vs_CDP_WithZeroIR0) {
    // Setup identical conditions for both
    setIR(0x1000, 0x2000, 0x3000);
    setBackgroundColor(0x100, 0x200, 0x300);
    setColorMatrix(0x1000, 0, 0, 0, 0x1000, 0, 0, 0, 0x1000);
    setRGBC(0x80, 0x80, 0x80, 0xAA);
    setFarColor(0x4000, 0x5000, 0x6000);
    setIR0(0); // Zero interpolation

    // Execute CDP
    gte.execute(0x00080014);

    // Reset and execute CC
    gte.reset();
    setIR(0x1000, 0x2000, 0x3000);
    setBackgroundColor(0x100, 0x200, 0x300);
    setColorMatrix(0x1000, 0, 0, 0, 0x1000, 0, 0, 0, 0x1000);
    setRGBC(0x80, 0x80, 0x80, 0xAA);
    gte.execute(0x0008001C);
}

TEST_F(GteCCCDPTest, CC_ZeroRGBCComponents) {
    // Set up IR values
    setIR(0x4000, 0x4000, 0x4000);
    setBackgroundColor(0, 0, 0);
    setColorMatrix(0x1000, 0, 0, 0, 0x1000, 0, 0, 0, 0x1000);

    // Set zero RGBC components
    setRGBC(0, 0, 0, 0xBB);

    // Execute CC
    gte.execute(0x0008001C);
}

TEST_F(GteCCCDPTest, CDP_ZeroFarColor) {
    // Set up IR values
    setIR(0x4000, 0x4000, 0x4000);
    setBackgroundColor(0, 0, 0);
    setColorMatrix(0x1000, 0, 0, 0, 0x1000, 0, 0, 0, 0x1000);
    setRGBC(0x80, 0x80, 0x80, 0xCC);

    // Set zero far color
    setFarColor(0, 0, 0);

    // Set high IR0 to emphasize far color influence
    setIR0(0x7FFF);

    // Execute CDP
    gte.execute(0x00080014);
}
