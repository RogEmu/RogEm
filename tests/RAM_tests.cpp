#include <gtest/gtest.h>
#include "Core/RAM.hpp"

TEST(RAM, loadExecutable)
{
    RAM ram(nullptr);
    uint32_t baseAddr = 0x80000000;
    std::vector<uint8_t> data;

    data.resize(100, 0xF);
    ram.loadExecutable(baseAddr, data);

    auto result = ram.data();
    for (size_t i = 0; i < data.size(); i++) {
        EXPECT_EQ(data.at(i), result->at(i));
    }
}
