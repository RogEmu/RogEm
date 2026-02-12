#include <gtest/gtest.h>

#include "Core/BIOS.hpp"

TEST(BIOS, loadFileNotExisting)
{
    BIOS bios(nullptr);

    auto result = bios.loadFromFile("this_files_does_not_exist.bin");
    EXPECT_FALSE(result);
}

TEST(BIOS, loadFileTooSmall)
{
    BIOS bios(nullptr);

    auto result = bios.loadFromFile(__FILE__);
    EXPECT_FALSE(result);
}

TEST(BIOS, loadFileContructor)
{
    BIOS bios(nullptr, __FILE__);
}

TEST(BIOS, writeReadOnly)
{
    BIOS bios(nullptr);

    bios.write8(0, 0);
    bios.write16(0, 0);
    bios.write32(0, 0);
}

TEST(BIOS, isReadOnly)
{
    BIOS bios(nullptr);

    EXPECT_TRUE(bios.isReadOnly());
}
