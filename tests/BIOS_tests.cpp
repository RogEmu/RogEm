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
