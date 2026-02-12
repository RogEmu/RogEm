#include <gtest/gtest.h>
#include "Core/PsxExecutable.hpp"

TEST(PsxExecutable, loadExe)
{
    PsxExecutable exe("tests/files/psx.exe");

    exe.load();
    EXPECT_EQ(exe.exeSize, 0x00051000);
    EXPECT_EQ(exe.initialGp, 0);
    EXPECT_EQ(exe.initialSpBase, 0x801FFFF0);
    EXPECT_EQ(exe.initialSpOffset, 0);
    EXPECT_EQ(exe.initialPc, 0x80010000);
    EXPECT_EQ(exe.ramDestination, 0x80010000);
}

TEST(PsxExecutable, noExe)
{
    PsxExecutable exe("");

    exe.load();
    EXPECT_EQ(exe.exeSize, 0);
    EXPECT_EQ(exe.initialGp, 0);
    EXPECT_EQ(exe.initialSpBase, 0);
    EXPECT_EQ(exe.initialSpOffset, 0);
    EXPECT_EQ(exe.initialPc, 0);
    EXPECT_EQ(exe.ramDestination, 0);
}

TEST(PsxExecutable, notAnExeFile)
{
    PsxExecutable exe(__FILE__);

    exe.load();
    EXPECT_EQ(exe.exeSize, 0);
    EXPECT_EQ(exe.initialGp, 0);
    EXPECT_EQ(exe.initialSpBase, 0);
    EXPECT_EQ(exe.initialSpOffset, 0);
    EXPECT_EQ(exe.initialPc, 0);
    EXPECT_EQ(exe.ramDestination, 0);
}
