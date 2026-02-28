#ifndef INPUTMANAGER_HPP_
#define INPUTMANAGER_HPP_

#include <unordered_map>
#include <cstdint>
#include <GLFW/glfw3.h>
#include "DigitalPad.hpp"
#include <optional>
#include <string>

class System;

class InputManager
{
public:
    InputManager(System* system);

    void handleKeyEvent(int key, int action);
    void handleGamepadState(const GLFWgamepadstate& state);

    void reset();

    void startRebinding(PadButton button);
    void cancelRebinding();
    bool isRebinding() const { return m_waitingForKey.has_value(); }
    void processRebinding(int key);

    void startGamepadRebinding(PadButton button);
    void cancelGamepadRebinding();
    bool isGamepadRebinding() const;

    void updateGamepadRebinding(const GLFWgamepadstate& state);

    const std::unordered_map<int, PadButton>& getKeyboardMapping() const { return m_keyboardMapping; }
    const std::unordered_map<int, PadButton>& getGamepadMapping() const { return m_gamepadMapping; }

    void setKeyboardMapping(const std::unordered_map<int, PadButton>& newMapping);
    std::unordered_map<int, PadButton> getKeyboardMappingCopy() const;

    std::unordered_map<int, PadButton> getGamepadMappingCopy() const;

    void saveKeyboardMappingToFile(const std::string& path);
    void loadKeyboardMappingFromFile(const std::string& path);

    void saveGamepadMappingToFile(const std::string& path);
    void loadGamepadMappingFromFile(const std::string& path);

    void resetKeyboardMappingToDefault();
    std::unordered_map<int, PadButton> getDefaultKeyboardMapping() const;

    std::optional<std::pair<int, PadButton>> consumePendingGamepadAssignment();

    void setGamepadMapping(const std::unordered_map<int, PadButton>& newMapping);
    void resetGamepadMappingToDefault();

private:
    void updateSystem();

private:
    System* m_system;

    uint16_t m_buttonsPort = 0xFFFF;

    std::unordered_map<int, PadButton> m_keyboardMapping;
    std::unordered_map<int, PadButton> m_gamepadMapping;
    std::unordered_map<int, PadButton> m_defaultGamepadMapping;
    
    std::optional<PadButton> m_waitingForKey;
    std::optional<PadButton> m_waitingGamepadButton;
    GLFWgamepadstate m_previousGamepadState{};

    std::optional<std::pair<int, PadButton>> m_pendingGamepadAssignment;
};

#endif
