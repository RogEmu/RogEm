#ifndef INPUTMANAGER_HPP_
#define INPUTMANAGER_HPP_

#include <unordered_map>
#include <cstdint>
#include <GLFW/glfw3.h>
#include "DigitalPad.hpp"
#include <optional>

class System;

class InputManager
{
public:
    InputManager(System* system);

    void handleKeyEvent(int key, int action);
    void handleGamepadState(const GLFWgamepadstate& state);

    void reset();

    void startRebinding(PadButton button);
    bool isRebinding() const { return m_waitingForKey.has_value(); }
    void processRebinding(int key);

    const std::unordered_map<int, PadButton>& getKeyboardMapping() const { return m_keyboardMapping; }

    void setKeyboardMapping(const std::unordered_map<int, PadButton>& newMapping);
    std::unordered_map<int, PadButton> getKeyboardMappingCopy() const;

private:
    void updateSystem();

private:
    System* m_system;

    uint16_t m_buttonsPort = 0xFFFF;

    std::unordered_map<int, PadButton> m_keyboardMapping;
    std::unordered_map<int, PadButton> m_gamepadMapping;
    std::optional<PadButton> m_waitingForKey;
};

#endif
