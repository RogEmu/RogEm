/*
** EPITECH PROJECT, 2026
** rogem
** File description:
** InputManager
*/

#include "InputManager.hpp"

#include <fstream>
#include <nlohmann/json.hpp>
#include <spdlog/spdlog.h>
#include <GLFW/glfw3.h>

static const std::unordered_map<int, PadButton> defaultKeys = {
    { GLFW_KEY_UP,    PadButton::PAD_JOYUP },
    { GLFW_KEY_DOWN,  PadButton::PAD_JOYDOWN },
    { GLFW_KEY_LEFT,  PadButton::PAD_JOYLEFT },
    { GLFW_KEY_RIGHT, PadButton::PAD_JOYRIGHT },
    { GLFW_KEY_Z,     PadButton::PAD_CROSS },
    { GLFW_KEY_Q,     PadButton::PAD_CIRCLE },
    { GLFW_KEY_S,     PadButton::PAD_SQUARE },
    { GLFW_KEY_D,     PadButton::PAD_TRIANGLE },
    { GLFW_KEY_A,     PadButton::PAD_L1 },
    { GLFW_KEY_E,     PadButton::PAD_R1 },
    { GLFW_KEY_1,     PadButton::PAD_L2 },
    { GLFW_KEY_2,     PadButton::PAD_R2 },
    { GLFW_KEY_ENTER, PadButton::PAD_START },
    { GLFW_KEY_F,     PadButton::PAD_SELECT },
};

static std::string padButtonToString(PadButton button)
{
    switch (button) {
        case PadButton::PAD_SELECT:   return "SELECT";
        case PadButton::PAD_L3:       return "L3";
        case PadButton::PAD_R3:       return "R3";
        case PadButton::PAD_START:    return "START";
        case PadButton::PAD_JOYUP:    return "UP";
        case PadButton::PAD_JOYRIGHT: return "RIGHT";
        case PadButton::PAD_JOYDOWN:  return "DOWN";
        case PadButton::PAD_JOYLEFT:  return "LEFT";
        case PadButton::PAD_L2:       return "L2";
        case PadButton::PAD_R2:       return "R2";
        case PadButton::PAD_L1:       return "L1";
        case PadButton::PAD_R1:       return "R1";
        case PadButton::PAD_TRIANGLE: return "TRIANGLE";
        case PadButton::PAD_CIRCLE:   return "CIRCLE";
        case PadButton::PAD_CROSS:    return "CROSS";
        case PadButton::PAD_SQUARE:   return "SQUARE";
        default:                      return "UNKNOWN";
    }
}

static PadButton stringToPadButton(const std::string& name)
{
    static const std::unordered_map<std::string, PadButton> map = {
        {"SELECT",   PadButton::PAD_SELECT},
        {"L3",       PadButton::PAD_L3},
        {"R3",       PadButton::PAD_R3},
        {"START",    PadButton::PAD_START},
        {"UP",       PadButton::PAD_JOYUP},
        {"RIGHT",    PadButton::PAD_JOYRIGHT},
        {"DOWN",     PadButton::PAD_JOYDOWN},
        {"LEFT",     PadButton::PAD_JOYLEFT},
        {"L2",       PadButton::PAD_L2},
        {"R2",       PadButton::PAD_R2},
        {"L1",       PadButton::PAD_L1},
        {"R1",       PadButton::PAD_R1},
        {"TRIANGLE", PadButton::PAD_TRIANGLE},
        {"CIRCLE",   PadButton::PAD_CIRCLE},
        {"CROSS",    PadButton::PAD_CROSS},
        {"SQUARE",   PadButton::PAD_SQUARE},
    };
    auto it = map.find(name);
    if (it != map.end()) {
        return it->second;
    }
    return PadButton::PAD_UNKOWN;
}

InputManager::InputManager()
    : m_buttonState(0xFFFF)
{
    initDefaultKeybindings();
}

void InputManager::initDefaultKeybindings()
{
    m_keybindings = defaultKeys;
    m_buttonToKey.clear();
    for (const auto& [key, button] : m_keybindings) {
        m_buttonToKey[button] = key;
    }
}

void InputManager::onKeyEvent(int key, int action)
{
    auto it = m_keybindings.find(key);
    if (it == m_keybindings.end()) {
        return;
    }
    PadButton button = it->second;
    if (button == PadButton::PAD_UNKOWN) {
        return;
    }
    if (action == GLFW_PRESS) {
        m_buttonState &= ~static_cast<uint16_t>(button);
    } else if (action == GLFW_RELEASE) {
        m_buttonState |= static_cast<uint16_t>(button);
    }
}

uint16_t InputManager::getButtonState() const
{
    return m_buttonState;
}

void InputManager::setKeybinding(int keyCode, PadButton button)
{
    auto it = m_buttonToKey.find(button);
    if (it != m_buttonToKey.end()) {
        m_keybindings.erase(it->second);
    }
    m_keybindings[keyCode] = button;
    m_buttonToKey[button] = keyCode;
}

void InputManager::removeKeybinding(int keyCode)
{
    auto it = m_keybindings.find(keyCode);
    if (it != m_keybindings.end()) {
        m_buttonToKey.erase(it->second);
        m_keybindings.erase(it);
    }
}

void InputManager::clearKeybindings()
{
    m_keybindings.clear();
    m_buttonToKey.clear();
}

void InputManager::resetToDefaults()
{
    m_buttonState = 0xFFFF;
    initDefaultKeybindings();
}

const std::unordered_map<int, PadButton>& InputManager::getKeybindings() const
{
    return m_keybindings;
}

bool InputManager::saveToFile(const std::string& filePath) const
{
    nlohmann::json bindings = nlohmann::json::object();
    for (const auto& [keyCode, button] : m_keybindings) {
        std::string buttonName = padButtonToString(button);
        if (buttonName != "UNKNOWN") {
            bindings[buttonName] = keyCode;
        }
    }
    nlohmann::json j;
    j["keybindings"] = bindings;

    std::ofstream file(filePath);
    if (!file.is_open()) {
        spdlog::error("InputManager: Failed to open '{}' for writing", filePath);
        return false;
    }
    file << j.dump(4);
    return true;
}

bool InputManager::loadFromFile(const std::string& filePath)
{
    std::ifstream file(filePath);
    if (!file.is_open()) {
        spdlog::warn("InputManager: File '{}' not found, using defaults", filePath);
        return false;
    }
    try {
        nlohmann::json j;
        file >> j;
        if (!j.contains("keybindings") || !j["keybindings"].is_object()) {
            spdlog::error("InputManager: Invalid format in '{}'", filePath);
            return false;
        }
        m_keybindings.clear();
        m_buttonToKey.clear();
        for (auto& [buttonName, keyCode] : j["keybindings"].items()) {
            PadButton button = stringToPadButton(buttonName);
            if (button != PadButton::PAD_UNKOWN && keyCode.is_number_integer()) {
                int key = keyCode.get<int>();
                m_keybindings[key] = button;
                m_buttonToKey[button] = key;
            } else {
                spdlog::warn("InputManager: Skipping unknown binding '{}' -> {}",
                             buttonName, keyCode.dump());
            }
        }
    } catch (const nlohmann::json::exception& e) {
        spdlog::error("InputManager: JSON parse error in '{}': {}", filePath, e.what());
        return false;
    }
    return true;
}
