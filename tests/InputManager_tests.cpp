#include <gtest/gtest.h>
#include <fstream>
#include <filesystem>
#include <GLFW/glfw3.h>
#include "InputManager.hpp"

class InputManagerTest : public ::testing::Test {
protected:
    void SetUp() override {
        mgr = std::make_unique<InputManager>();
    }

    void TearDown() override {
        if (std::filesystem::exists(m_testFile)) {
            std::filesystem::remove(m_testFile);
        }
    }

    std::unique_ptr<InputManager> mgr;
    std::string m_testFile = "test_keybindings.json";
};

TEST_F(InputManagerTest, DefaultStateAllReleased) {
    EXPECT_EQ(mgr->getButtonState(), 0xFFFF);
}

TEST_F(InputManagerTest, DefaultKeybindingsLoaded) {
    const auto& bindings = mgr->getKeybindings();
    EXPECT_FALSE(bindings.empty());
    EXPECT_EQ(bindings.size(), 14);
}

TEST_F(InputManagerTest, KeyPressClearsBit) {
    // GLFW_KEY_S is mapped to PAD_CROSS (0x4000) by default
    mgr->onKeyEvent(GLFW_KEY_S, GLFW_PRESS);
    EXPECT_EQ(mgr->getButtonState() & 0x4000, 0);
}

TEST_F(InputManagerTest, KeyReleaseSetsBit) {
    mgr->onKeyEvent(GLFW_KEY_S, GLFW_PRESS);
    EXPECT_EQ(mgr->getButtonState() & 0x4000, 0);

    mgr->onKeyEvent(GLFW_KEY_S, GLFW_RELEASE);
    EXPECT_EQ(mgr->getButtonState() & 0x4000, 0x4000);
}

TEST_F(InputManagerTest, UnmappedKeyDoesNothing) {
    mgr->onKeyEvent(9999, GLFW_PRESS);
    EXPECT_EQ(mgr->getButtonState(), 0xFFFF);
}

TEST_F(InputManagerTest, MultipleSimultaneousPresses) {
    // Press S (CROSS=0x4000) and W (TRIANGLE=0x1000)
    mgr->onKeyEvent(GLFW_KEY_S, GLFW_PRESS);
    mgr->onKeyEvent(GLFW_KEY_W, GLFW_PRESS);

    uint16_t state = mgr->getButtonState();
    EXPECT_EQ(state & 0x4000, 0);  // CROSS pressed
    EXPECT_EQ(state & 0x1000, 0);  // TRIANGLE pressed
    EXPECT_EQ(state & 0x0008, 0x0008);  // START still released
}

TEST_F(InputManagerTest, SetKeybinding) {
    // Bind key 999 to PAD_START
    mgr->setKeybinding(999, PadButton::PAD_START);

    mgr->onKeyEvent(999, GLFW_PRESS);
    EXPECT_EQ(mgr->getButtonState() & 0x0008, 0);
}

TEST_F(InputManagerTest, RemoveKeybinding) {
    // Remove default Z key binding
    mgr->removeKeybinding(GLFW_KEY_S);

    mgr->onKeyEvent(GLFW_KEY_S, GLFW_PRESS);
    EXPECT_EQ(mgr->getButtonState(), 0xFFFF);
}

TEST_F(InputManagerTest, ClearKeybindings) {
    mgr->clearKeybindings();
    EXPECT_TRUE(mgr->getKeybindings().empty());

    mgr->onKeyEvent(GLFW_KEY_S, GLFW_PRESS);
    EXPECT_EQ(mgr->getButtonState(), 0xFFFF);
}

TEST_F(InputManagerTest, ResetToDefaults) {
    mgr->clearKeybindings();
    EXPECT_TRUE(mgr->getKeybindings().empty());

    mgr->resetToDefaults();
    EXPECT_EQ(mgr->getKeybindings().size(), 14);
    EXPECT_EQ(mgr->getButtonState(), 0xFFFF);
}

TEST_F(InputManagerTest, ResetToDefaultsClearsButtonState) {
    mgr->onKeyEvent(GLFW_KEY_S, GLFW_PRESS);
    EXPECT_NE(mgr->getButtonState(), 0xFFFF);

    mgr->resetToDefaults();
    EXPECT_EQ(mgr->getButtonState(), 0xFFFF);
}

TEST_F(InputManagerTest, SaveAndLoadRoundTrip) {
    ASSERT_TRUE(mgr->saveToFile(m_testFile));

    InputManager loaded;
    ASSERT_TRUE(loaded.loadFromFile(m_testFile));

    // Verify loaded bindings match originals
    const auto& original = mgr->getKeybindings();
    const auto& restored = loaded.getKeybindings();
    EXPECT_EQ(original.size(), restored.size());

    for (const auto& [key, button] : original) {
        auto it = restored.find(key);
        ASSERT_NE(it, restored.end()) << "Key " << key << " missing after load";
        EXPECT_EQ(it->second, button);
    }
}

TEST_F(InputManagerTest, LoadFromNonexistentFile) {
    EXPECT_FALSE(mgr->loadFromFile("nonexistent_file.json"));
    // Bindings should remain unchanged
    EXPECT_EQ(mgr->getKeybindings().size(), 14);
}

TEST_F(InputManagerTest, LoadFromMalformedJson) {
    std::ofstream file(m_testFile);
    file << "{ not valid json";
    file.close();

    EXPECT_FALSE(mgr->loadFromFile(m_testFile));
}

TEST_F(InputManagerTest, LoadFromInvalidFormat) {
    std::ofstream file(m_testFile);
    file << R"({"wrong_key": 42})";
    file.close();

    EXPECT_FALSE(mgr->loadFromFile(m_testFile));
}

TEST_F(InputManagerTest, SavedFileContainsAllBindings) {
    ASSERT_TRUE(mgr->saveToFile(m_testFile));

    std::ifstream file(m_testFile);
    std::string content((std::istreambuf_iterator<char>(file)),
                        std::istreambuf_iterator<char>());

    EXPECT_NE(content.find("CROSS"), std::string::npos);
    EXPECT_NE(content.find("CIRCLE"), std::string::npos);
    EXPECT_NE(content.find("SQUARE"), std::string::npos);
    EXPECT_NE(content.find("TRIANGLE"), std::string::npos);
    EXPECT_NE(content.find("START"), std::string::npos);
    EXPECT_NE(content.find("SELECT"), std::string::npos);
    EXPECT_NE(content.find("UP"), std::string::npos);
    EXPECT_NE(content.find("DOWN"), std::string::npos);
    EXPECT_NE(content.find("LEFT"), std::string::npos);
    EXPECT_NE(content.find("RIGHT"), std::string::npos);
    EXPECT_NE(content.find("L1"), std::string::npos);
    EXPECT_NE(content.find("R1"), std::string::npos);
    EXPECT_NE(content.find("L2"), std::string::npos);
    EXPECT_NE(content.find("R2"), std::string::npos);
}

TEST_F(InputManagerTest, OverwriteExistingKeybinding) {
    // GLFW_KEY_S is mapped to CROSS by default; remap it to START
    mgr->setKeybinding(GLFW_KEY_S, PadButton::PAD_START);

    mgr->onKeyEvent(GLFW_KEY_S, GLFW_PRESS);
    uint16_t state = mgr->getButtonState();
    EXPECT_EQ(state & 0x0008, 0);      // START pressed
    EXPECT_EQ(state & 0x4000, 0x4000); // CROSS still released
}

TEST_F(InputManagerTest, SetKeybindingRemovesOldKeyForSameButton) {
    // GLFW_KEY_S (90) is mapped to PAD_CROSS by default
    // Rebind PAD_CROSS to key 999
    mgr->setKeybinding(999, PadButton::PAD_CROSS);

    // Old key should no longer be bound
    mgr->onKeyEvent(GLFW_KEY_S, GLFW_PRESS);
    EXPECT_EQ(mgr->getButtonState(), 0xFFFF);

    // New key should work
    mgr->onKeyEvent(999, GLFW_PRESS);
    EXPECT_EQ(mgr->getButtonState() & 0x4000, 0);

    // Binding count stays at 14
    EXPECT_EQ(mgr->getKeybindings().size(), 14);
}

TEST_F(InputManagerTest, AllDefaultButtonsMapped) {
    const auto& bindings = mgr->getKeybindings();

    // Collect all mapped PadButton values
    std::set<PadButton> mapped;
    for (const auto& [key, button] : bindings) {
        mapped.insert(button);
    }

    EXPECT_TRUE(mapped.count(PadButton::PAD_JOYUP));
    EXPECT_TRUE(mapped.count(PadButton::PAD_JOYDOWN));
    EXPECT_TRUE(mapped.count(PadButton::PAD_JOYLEFT));
    EXPECT_TRUE(mapped.count(PadButton::PAD_JOYRIGHT));
    EXPECT_TRUE(mapped.count(PadButton::PAD_CROSS));
    EXPECT_TRUE(mapped.count(PadButton::PAD_CIRCLE));
    EXPECT_TRUE(mapped.count(PadButton::PAD_SQUARE));
    EXPECT_TRUE(mapped.count(PadButton::PAD_TRIANGLE));
    EXPECT_TRUE(mapped.count(PadButton::PAD_L1));
    EXPECT_TRUE(mapped.count(PadButton::PAD_R1));
    EXPECT_TRUE(mapped.count(PadButton::PAD_L2));
    EXPECT_TRUE(mapped.count(PadButton::PAD_R2));
    EXPECT_TRUE(mapped.count(PadButton::PAD_START));
    EXPECT_TRUE(mapped.count(PadButton::PAD_SELECT));
}
