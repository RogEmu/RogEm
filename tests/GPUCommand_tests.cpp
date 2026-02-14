#include <gtest/gtest.h>
#include <memory>

#include "Core/GPUCommand.hpp"

// Test fixture for GPUCommand tests
class GPUCommandTest : public ::testing::Test {
protected:
    void SetUp() override {
        gpuCommand = std::make_unique<GPUCommand>();
    }

    std::unique_ptr<GPUCommand> gpuCommand;
};

// Test NOP command (0x00)

TEST_F(GPUCommandTest, NOPCommand)
{
    gpuCommand->set(0x00000000);

    EXPECT_EQ(GPUCommandType::NOP, gpuCommand->type());
    EXPECT_EQ(0, gpuCommand->expectedParams());
}

// Test ClearCache command (0x01)

TEST_F(GPUCommandTest, ClearCacheCommand)
{
    gpuCommand->set(0x01000000);

    EXPECT_EQ(GPUCommandType::ClearCache, gpuCommand->type());
    EXPECT_EQ(0, gpuCommand->expectedParams());
}

// Test QuickRectFill command (0x02)

TEST_F(GPUCommandTest, QuickRectFillCommand)
{
    gpuCommand->set(0x02AABBCC);

    EXPECT_EQ(GPUCommandType::QuickRectFill, gpuCommand->type());
    EXPECT_EQ(3, gpuCommand->expectedParams());

    // First param should be the color (lower 24 bits)
    EXPECT_EQ(1, gpuCommand->params().size());
    EXPECT_EQ(0x00AABBCC, gpuCommand->params().data()[0]);
}

// Test DrawPolygon commands - 3 vertices, flat

TEST_F(GPUCommandTest, DrawPolygon3VerticesFlatOpaque)
{
    // bits: 001 (polygon) | 0 (flat) | 0 (3 vertices) | 0 (no texture) | 0 (opaque) | 0 (normal)
    uint32_t cmd = 0x20112233;
    gpuCommand->set(cmd);

    EXPECT_EQ(GPUCommandType::DrawPolygon, gpuCommand->type());

    const GPUCommandFlags &flags = gpuCommand->flags();
    EXPECT_FALSE(flags.shaded);
    EXPECT_EQ(3, flags.nbVertices);
    EXPECT_FALSE(flags.textured);
    EXPECT_FALSE(flags.semiTransparent);
    EXPECT_FALSE(flags.rawTexture);

    // Expected params: 3 vertices * 1 (position) + 1 (already added) = 4
    EXPECT_EQ(4, gpuCommand->expectedParams());
}

TEST_F(GPUCommandTest, DrawPolygon3VerticesShadedOpaque)
{
    // bits: 001 (polygon) | 1 (shaded) | 0 (3 vertices) | 0 (no texture) | 0 (opaque) | 0 (normal)
    uint32_t cmd = 0x30AABBCC;
    gpuCommand->set(cmd);

    EXPECT_EQ(GPUCommandType::DrawPolygon, gpuCommand->type());

    const GPUCommandFlags &flags = gpuCommand->flags();
    EXPECT_TRUE(flags.shaded);
    EXPECT_EQ(3, flags.nbVertices);
    EXPECT_FALSE(flags.textured);
    EXPECT_FALSE(flags.semiTransparent);

    // Expected params: 3 * (1 + 1) = 6
    EXPECT_EQ(6, gpuCommand->expectedParams());
}

TEST_F(GPUCommandTest, DrawPolygon3VerticesFlatTextured)
{
    // bits: 001 (polygon) | 0 (flat) | 0 (3 vertices) | 1 (textured) | 0 (opaque) | 0 (normal)
    uint32_t cmd = 0x24FFFFFF;
    gpuCommand->set(cmd);

    EXPECT_EQ(GPUCommandType::DrawPolygon, gpuCommand->type());

    const GPUCommandFlags &flags = gpuCommand->flags();
    EXPECT_FALSE(flags.shaded);
    EXPECT_EQ(3, flags.nbVertices);
    EXPECT_TRUE(flags.textured);
    EXPECT_FALSE(flags.semiTransparent);

    // Expected params: 3 * (1 + 1) + 1 = 7
    EXPECT_EQ(7, gpuCommand->expectedParams());
}

TEST_F(GPUCommandTest, DrawPolygon3VerticesSemiTransparent)
{
    // bits: 001 (polygon) | 0 (flat) | 0 (3 vertices) | 0 (no texture) | 1 (semi-transparent) | 0 (normal)
    uint32_t cmd = 0x22000000;
    gpuCommand->set(cmd);

    EXPECT_EQ(GPUCommandType::DrawPolygon, gpuCommand->type());

    const GPUCommandFlags &flags = gpuCommand->flags();
    EXPECT_FALSE(flags.shaded);
    EXPECT_EQ(3, flags.nbVertices);
    EXPECT_FALSE(flags.textured);
    EXPECT_TRUE(flags.semiTransparent);
}

TEST_F(GPUCommandTest, DrawPolygon3VerticesRawTexture)
{
    // bits: 001 (polygon) | 0 (flat) | 0 (3 vertices) | 1 (textured) | 0 (opaque) | 1 (raw)
    uint32_t cmd = 0x25000000;
    gpuCommand->set(cmd);

    EXPECT_EQ(GPUCommandType::DrawPolygon, gpuCommand->type());

    const GPUCommandFlags &flags = gpuCommand->flags();
    EXPECT_TRUE(flags.textured);
    EXPECT_TRUE(flags.rawTexture);
}

// Test DrawPolygon commands - 4 vertices

TEST_F(GPUCommandTest, DrawPolygon4VerticesFlatOpaque)
{
    // bits: 001 (polygon) | 0 (flat) | 1 (4 vertices) | 0 (no texture) | 0 (opaque) | 0 (normal)
    uint32_t cmd = 0x28000000;
    gpuCommand->set(cmd);

    EXPECT_EQ(GPUCommandType::DrawPolygon, gpuCommand->type());

    const GPUCommandFlags &flags = gpuCommand->flags();
    EXPECT_FALSE(flags.shaded);
    EXPECT_EQ(4, flags.nbVertices);
    EXPECT_FALSE(flags.textured);

    // Expected params: 4 vertices * 1 (position) + 1 (already added) = 5
    EXPECT_EQ(5, gpuCommand->expectedParams());
}

TEST_F(GPUCommandTest, DrawPolygon4VerticesShadedTextured)
{
    // bits: 001 (polygon) | 1 (shaded) | 1 (4 vertices) | 1 (textured) | 0 (opaque) | 0 (normal)
    uint32_t cmd = 0x3C000000;
    gpuCommand->set(cmd);

    EXPECT_EQ(GPUCommandType::DrawPolygon, gpuCommand->type());

    const GPUCommandFlags &flags = gpuCommand->flags();
    EXPECT_TRUE(flags.shaded);
    EXPECT_EQ(4, flags.nbVertices);
    EXPECT_TRUE(flags.textured);

    // Expected params: 4 * (1 + 1 + 1) = 12
    EXPECT_EQ(12, gpuCommand->expectedParams());
}

// Test DrawLine commands

TEST_F(GPUCommandTest, DrawLineFlatOpaque)
{
    // bits: 010 (line) | 0 (flat) | x | x | x | 0 (opaque) | 0 (single)
    uint32_t cmd = 0x40AABBCC;
    gpuCommand->set(cmd);

    EXPECT_EQ(GPUCommandType::DrawLine, gpuCommand->type());

    const GPUCommandFlags &flags = gpuCommand->flags();
    EXPECT_FALSE(flags.shaded);
    EXPECT_FALSE(flags.semiTransparent);
    EXPECT_FALSE(flags.polyline);

    // Expected params: 3 (two vertices, one color)
    EXPECT_EQ(3, gpuCommand->expectedParams());
}

TEST_F(GPUCommandTest, DrawLineShadedOpaque)
{
    // bits: 010 (line) | 1 (shaded) | x | x | x | 0 (opaque) | 0 (single)
    uint32_t cmd = 0x50000000;
    gpuCommand->set(cmd);

    EXPECT_EQ(GPUCommandType::DrawLine, gpuCommand->type());

    const GPUCommandFlags &flags = gpuCommand->flags();
    EXPECT_TRUE(flags.shaded);
    EXPECT_FALSE(flags.semiTransparent);
    EXPECT_FALSE(flags.polyline);

    // Expected params: 4 (two vertices with colors)
    EXPECT_EQ(4, gpuCommand->expectedParams());
}

TEST_F(GPUCommandTest, DrawLineSemiTransparent)
{
    // bits: 010 (line) | 0 (flat) | x | x | x | 1 (semi-transparent) | 0 (single)
    uint32_t cmd = 0x42000000;
    gpuCommand->set(cmd);

    EXPECT_EQ(GPUCommandType::DrawLine, gpuCommand->type());

    const GPUCommandFlags &flags = gpuCommand->flags();
    EXPECT_FALSE(flags.shaded);
    EXPECT_TRUE(flags.semiTransparent);
    EXPECT_FALSE(flags.polyline);
}

TEST_F(GPUCommandTest, DrawLinePolyline)
{
    // bits: 010 (line) | 0 (flat) | x | x | x | 0 (opaque) | 1 (polyline)
    uint32_t cmd = 0x41000000;
    gpuCommand->set(cmd);

    EXPECT_EQ(GPUCommandType::DrawLine, gpuCommand->type());

    const GPUCommandFlags &flags = gpuCommand->flags();
    EXPECT_FALSE(flags.shaded);
    EXPECT_FALSE(flags.semiTransparent);
    EXPECT_TRUE(flags.polyline);

    // Expected params: -1 (variable length for polyline)
    EXPECT_EQ(-1, gpuCommand->expectedParams());
}

TEST_F(GPUCommandTest, DrawLineShadedPolyline)
{
    // bits: 010 (line) | 1 (shaded) | x | x | x | 1 (semi-transparent) | 1 (polyline)
    uint32_t cmd = 0x53000000;
    gpuCommand->set(cmd);

    EXPECT_EQ(GPUCommandType::DrawLine, gpuCommand->type());

    const GPUCommandFlags &flags = gpuCommand->flags();
    EXPECT_TRUE(flags.shaded);
    EXPECT_TRUE(flags.semiTransparent);
    EXPECT_TRUE(flags.polyline);
}

// Test DrawRectangle commands

TEST_F(GPUCommandTest, DrawRectangleVariableSize)
{
    // bits: 011 (rect) | 00 (variable size) | x | 0 (no texture) | 0 (opaque) | 0 (normal)
    uint32_t cmd = 0x60112233;
    gpuCommand->set(cmd);

    EXPECT_EQ(GPUCommandType::DrawRectangle, gpuCommand->type());

    const GPUCommandFlags &flags = gpuCommand->flags();
    EXPECT_EQ(GPURectSize::Variable, flags.rectFlag);
    EXPECT_FALSE(flags.textured);
    EXPECT_FALSE(flags.semiTransparent);
    EXPECT_EQ(0, flags.rectSize.width);
    EXPECT_EQ(0, flags.rectSize.height);

    // Expected params: 2 (position) + 1 (size for variable) = 3
    EXPECT_EQ(3, gpuCommand->expectedParams());
}

TEST_F(GPUCommandTest, DrawRectangle1x1)
{
    // bits: 011 (rect) | 01 (1x1) | x | 0 (no texture) | 0 (opaque) | 0 (normal)
    uint32_t cmd = 0x68000000;
    gpuCommand->set(cmd);

    EXPECT_EQ(GPUCommandType::DrawRectangle, gpuCommand->type());

    const GPUCommandFlags &flags = gpuCommand->flags();
    EXPECT_EQ(GPURectSize::Size_1x1, flags.rectFlag);
    EXPECT_EQ(1, flags.rectSize.width);
    EXPECT_EQ(1, flags.rectSize.height);

    // Expected params: 2 (position only, size is fixed)
    EXPECT_EQ(2, gpuCommand->expectedParams());
}

TEST_F(GPUCommandTest, DrawRectangle8x8)
{
    // bits: 011 (rect) | 10 (8x8) | x | 0 (no texture) | 0 (opaque) | 0 (normal)
    uint32_t cmd = 0x70000000;
    gpuCommand->set(cmd);

    EXPECT_EQ(GPUCommandType::DrawRectangle, gpuCommand->type());

    const GPUCommandFlags &flags = gpuCommand->flags();
    EXPECT_EQ(GPURectSize::Size_8x8, flags.rectFlag);
    EXPECT_EQ(8, flags.rectSize.width);
    EXPECT_EQ(8, flags.rectSize.height);

    EXPECT_EQ(2, gpuCommand->expectedParams());
}

TEST_F(GPUCommandTest, DrawRectangle16x16)
{
    // bits: 011 (rect) | 11 (16x16) | x | 0 (no texture) | 0 (opaque) | 0 (normal)
    uint32_t cmd = 0x78000000;
    gpuCommand->set(cmd);

    EXPECT_EQ(GPUCommandType::DrawRectangle, gpuCommand->type());

    const GPUCommandFlags &flags = gpuCommand->flags();
    EXPECT_EQ(GPURectSize::Size_16x16, flags.rectFlag);
    EXPECT_EQ(16, flags.rectSize.width);
    EXPECT_EQ(16, flags.rectSize.height);

    EXPECT_EQ(2, gpuCommand->expectedParams());
}

TEST_F(GPUCommandTest, DrawRectangleTextured)
{
    // bits: 011 (rect) | 00 (variable) | x | 1 (textured) | 0 (opaque) | 0 (normal)
    uint32_t cmd = 0x64000000;
    gpuCommand->set(cmd);

    EXPECT_EQ(GPUCommandType::DrawRectangle, gpuCommand->type());

    const GPUCommandFlags &flags = gpuCommand->flags();
    EXPECT_TRUE(flags.textured);

    // Expected params: 2 + 1 (size for variable) + 1 (texture) = 4
    EXPECT_EQ(4, gpuCommand->expectedParams());
}

TEST_F(GPUCommandTest, DrawRectangleSemiTransparent)
{
    // bits: 011 (rect) | 00 (variable) | x | 0 (no texture) | 1 (semi-transparent) | 0 (normal)
    uint32_t cmd = 0x62000000;
    gpuCommand->set(cmd);

    EXPECT_EQ(GPUCommandType::DrawRectangle, gpuCommand->type());

    const GPUCommandFlags &flags = gpuCommand->flags();
    EXPECT_FALSE(flags.textured);
    EXPECT_TRUE(flags.semiTransparent);
}

TEST_F(GPUCommandTest, DrawRectangleRawTexture)
{
    // bits: 011 (rect) | 00 (variable) | x | 1 (textured) | 0 (opaque) | 1 (raw)
    uint32_t cmd = 0x65000000;
    gpuCommand->set(cmd);

    EXPECT_EQ(GPUCommandType::DrawRectangle, gpuCommand->type());

    const GPUCommandFlags &flags = gpuCommand->flags();
    EXPECT_TRUE(flags.textured);
    EXPECT_TRUE(flags.rawTexture);
}

TEST_F(GPUCommandTest, DrawRectangle8x8Textured)
{
    // bits: 011 (rect) | 10 (8x8) | x | 1 (textured) | 0 (opaque) | 0 (normal)
    uint32_t cmd = 0x74000000;
    gpuCommand->set(cmd);

    EXPECT_EQ(GPUCommandType::DrawRectangle, gpuCommand->type());

    const GPUCommandFlags &flags = gpuCommand->flags();
    EXPECT_EQ(GPURectSize::Size_8x8, flags.rectFlag);
    EXPECT_TRUE(flags.textured);

    // Expected params: 2 (position) + 1 (texture) = 3 (size is fixed)
    EXPECT_EQ(3, gpuCommand->expectedParams());
}

// Test VRAM copy commands

TEST_F(GPUCommandTest, VramVramCopyCommand)
{
    // bits: 100 (VramVram)
    uint32_t cmd = 0x80000000;
    gpuCommand->set(cmd);

    EXPECT_EQ(GPUCommandType::VramVramCopy, gpuCommand->type());
    EXPECT_EQ(3, gpuCommand->expectedParams());
}

TEST_F(GPUCommandTest, CpuVramCopyCommand)
{
    // bits: 101 (CpuVram)
    uint32_t cmd = 0xA0000000;
    gpuCommand->set(cmd);

    EXPECT_EQ(GPUCommandType::CpuVramCopy, gpuCommand->type());
    EXPECT_EQ(2, gpuCommand->expectedParams());
}

TEST_F(GPUCommandTest, VramCpuCopyCommand)
{
    // bits: 110 (VramCpu)
    uint32_t cmd = 0xC0000000;
    gpuCommand->set(cmd);

    EXPECT_EQ(GPUCommandType::VramCpuCopy, gpuCommand->type());
    EXPECT_EQ(gpuCommand->expectedParams(), 2);
}

// Test Env command

TEST_F(GPUCommandTest, EnvCommand)
{
    // bits: 111 (Env)
    uint32_t cmd = 0xE1000000;
    gpuCommand->set(cmd);

    EXPECT_EQ(GPUCommandType::Env, gpuCommand->type());
    EXPECT_EQ(0, gpuCommand->expectedParams());
}

// Test parameter handling

TEST_F(GPUCommandTest, AddParameters)
{
    gpuCommand->set(0x20000000);  // Simple polygon command

    EXPECT_EQ(1, gpuCommand->params().size());

    gpuCommand->addParam(0x11111111);
    EXPECT_EQ(2, gpuCommand->params().size());

    gpuCommand->addParam(0x22222222);
    EXPECT_EQ(3, gpuCommand->params().size());

    // Verify parameter values
    EXPECT_EQ(0x00000000, gpuCommand->params().data()[0]);
    EXPECT_EQ(0x11111111, gpuCommand->params().data()[1]);
    EXPECT_EQ(0x22222222, gpuCommand->params().data()[2]);
}

TEST_F(GPUCommandTest, ParameterArrayMaxSize)
{
    gpuCommand->set(0x20000000);

    // Add 31 more parameters (already has 1)
    for (int i = 0; i < 31; i++) {
        gpuCommand->addParam(i);
    }

    EXPECT_EQ(32, gpuCommand->params().size());

    // Try to add one more (should be ignored)
    gpuCommand->addParam(0xFFFFFFFF);
    EXPECT_EQ(32, gpuCommand->params().size());
}

TEST_F(GPUCommandTest, ResetCommand)
{
    gpuCommand->set(0x30AABBCC);  // Shaded polygon
    gpuCommand->addParam(0x11111111);
    gpuCommand->addParam(0x22222222);

    EXPECT_EQ(GPUCommandType::DrawPolygon, gpuCommand->type());
    EXPECT_EQ(3, gpuCommand->params().size());

    // Reset
    gpuCommand->reset();

    EXPECT_EQ(GPUCommandType::None, gpuCommand->type());
    EXPECT_EQ(0, gpuCommand->params().size());
    EXPECT_EQ(0, gpuCommand->expectedParams());
}

TEST_F(GPUCommandTest, ColorMasking)
{
    // Test that the color is properly masked (lower 24 bits)
    uint32_t cmd = 0x20AABBCC;
    gpuCommand->set(cmd);

    EXPECT_EQ(1, gpuCommand->params().size());
    EXPECT_EQ(0x00AABBCC, gpuCommand->params().data()[0]);
}

// Test complex flag combinations

TEST_F(GPUCommandTest, Polygon4VerticesShadedTexturedSemiTransparentRaw)
{
    // bits: 001 | 1 (shaded) | 1 (4 vertices) | 1 (textured) | 1 (semi-transparent) | 1 (raw)
    uint32_t cmd = 0x3F000000;
    gpuCommand->set(cmd);

    EXPECT_EQ(GPUCommandType::DrawPolygon, gpuCommand->type());

    const GPUCommandFlags &flags = gpuCommand->flags();
    EXPECT_TRUE(flags.shaded);
    EXPECT_EQ(4, flags.nbVertices);
    EXPECT_TRUE(flags.textured);
    EXPECT_TRUE(flags.semiTransparent);
    EXPECT_TRUE(flags.rawTexture);
}

TEST_F(GPUCommandTest, RectangleVariableSizeTexturedSemiTransparentRaw)
{
    // bits: 011 | 00 (variable) | 1 (textured) | 1 (semi-transparent) | 1 (raw)
    uint32_t cmd = 0x67000000;
    gpuCommand->set(cmd);

    EXPECT_EQ(GPUCommandType::DrawRectangle, gpuCommand->type());

    const GPUCommandFlags &flags = gpuCommand->flags();
    EXPECT_EQ(GPURectSize::Variable, flags.rectFlag);
    EXPECT_TRUE(flags.textured);
    EXPECT_TRUE(flags.semiTransparent);
    EXPECT_TRUE(flags.rawTexture);

    // Expected params: 2 + 1 (variable size) + 1 (texture) = 4
    EXPECT_EQ(4, gpuCommand->expectedParams());
}

TEST_F(GPUCommandTest, SequentialCommands)
{
    // Test setting multiple commands in sequence
    gpuCommand->set(0x00000000);  // NOP
    EXPECT_EQ(GPUCommandType::NOP, gpuCommand->type());

    gpuCommand->reset();
    gpuCommand->set(0x01000000);  // ClearCache
    EXPECT_EQ(GPUCommandType::ClearCache, gpuCommand->type());

    gpuCommand->reset();
    gpuCommand->set(0x20000000);  // DrawPolygon
    EXPECT_EQ(GPUCommandType::DrawPolygon, gpuCommand->type());
}
