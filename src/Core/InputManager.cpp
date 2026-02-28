#include "InputManager.hpp"
#include "System.hpp"

InputManager::InputManager(System* system)
    : m_system(system)
{
    // Default keyboard mapping (ce que tu avais en hardcode)
    m_keyboardMapping[GLFW_KEY_UP]    = PAD_JOYUP;
    m_keyboardMapping[GLFW_KEY_DOWN]  = PAD_JOYDOWN;
    m_keyboardMapping[GLFW_KEY_LEFT]  = PAD_JOYLEFT;
    m_keyboardMapping[GLFW_KEY_RIGHT] = PAD_JOYRIGHT;

    m_keyboardMapping[GLFW_KEY_S] = PAD_CROSS;
    m_keyboardMapping[GLFW_KEY_D] = PAD_CIRCLE;
    m_keyboardMapping[GLFW_KEY_A] = PAD_SQUARE;
    m_keyboardMapping[GLFW_KEY_W] = PAD_TRIANGLE;

    m_keyboardMapping[GLFW_KEY_Q] = PAD_L1;
    m_keyboardMapping[GLFW_KEY_E] = PAD_R1;

    m_keyboardMapping[GLFW_KEY_1] = PAD_L2;
    m_keyboardMapping[GLFW_KEY_3] = PAD_R2;

    m_keyboardMapping[GLFW_KEY_ENTER] = PAD_START;
    m_keyboardMapping[GLFW_KEY_F]     = PAD_SELECT;

    // Default gamepad mapping
    m_gamepadMapping[GLFW_GAMEPAD_BUTTON_A] = PAD_CROSS;
    m_gamepadMapping[GLFW_GAMEPAD_BUTTON_B] = PAD_CIRCLE;
    m_gamepadMapping[GLFW_GAMEPAD_BUTTON_X] = PAD_SQUARE;
    m_gamepadMapping[GLFW_GAMEPAD_BUTTON_Y] = PAD_TRIANGLE;

    m_gamepadMapping[GLFW_GAMEPAD_BUTTON_LEFT_BUMPER]  = PAD_L1;
    m_gamepadMapping[GLFW_GAMEPAD_BUTTON_RIGHT_BUMPER] = PAD_R1;
    m_gamepadMapping[GLFW_GAMEPAD_BUTTON_BACK]         = PAD_SELECT;
    m_gamepadMapping[GLFW_GAMEPAD_BUTTON_START]        = PAD_START;

    m_gamepadMapping[GLFW_GAMEPAD_BUTTON_DPAD_UP]    = PAD_JOYUP;
    m_gamepadMapping[GLFW_GAMEPAD_BUTTON_DPAD_DOWN]  = PAD_JOYDOWN;
    m_gamepadMapping[GLFW_GAMEPAD_BUTTON_DPAD_LEFT]  = PAD_JOYLEFT;
    m_gamepadMapping[GLFW_GAMEPAD_BUTTON_DPAD_RIGHT] = PAD_JOYRIGHT;
}

void InputManager::handleKeyEvent(int key, int action)
{
    if (m_waitingForKey.has_value() && action == GLFW_PRESS)
    {
        processRebinding(key);
        return;
    }

    if (m_keyboardMapping.find(key) == m_keyboardMapping.end())
        return;

    PadButton pad = m_keyboardMapping[key];

    if (action == GLFW_PRESS)
        m_buttonsPort &= ~static_cast<uint16_t>(pad);
    else if (action == GLFW_RELEASE)
        m_buttonsPort |= static_cast<uint16_t>(pad);

    updateSystem();
}

void InputManager::handleGamepadState(const GLFWgamepadstate& state)
{
    m_buttonsPort = 0xFFFF;

    for (auto& [glfwBtn, padBtn] : m_gamepadMapping)
    {
        if (state.buttons[glfwBtn] == GLFW_PRESS)
            m_buttonsPort &= ~static_cast<uint16_t>(padBtn);
    }

    // Triggers
    if (state.axes[GLFW_GAMEPAD_AXIS_LEFT_TRIGGER] > -0.5f)
        m_buttonsPort &= ~static_cast<uint16_t>(PAD_L2);

    if (state.axes[GLFW_GAMEPAD_AXIS_RIGHT_TRIGGER] > -0.5f)
        m_buttonsPort &= ~static_cast<uint16_t>(PAD_R2);

    updateSystem();
}

void InputManager::updateSystem()
{
    if (m_system)
        m_system->updatePadInputs(m_buttonsPort);
}

void InputManager::reset()
{
    m_buttonsPort = 0xFFFF;
}

void InputManager::startRebinding(PadButton button)
{
    m_waitingForKey = button;
}

void InputManager::processRebinding(int key)
{
    if (!m_waitingForKey.has_value())
        return;

    PadButton buttonToRebind = m_waitingForKey.value();

    // Supprimer ancienne assignation
    for (auto it = m_keyboardMapping.begin(); it != m_keyboardMapping.end(); )
    {
        if (it->second == buttonToRebind)
            it = m_keyboardMapping.erase(it);
        else
            ++it;
    }

    // Ajouter nouvelle
    m_keyboardMapping[key] = buttonToRebind;

    m_waitingForKey.reset();
}

void InputManager::setKeyboardMapping(const std::unordered_map<int, PadButton>& newMapping)
{
    m_keyboardMapping = newMapping;
}

std::unordered_map<int, PadButton> InputManager::getKeyboardMappingCopy() const
{
    return m_keyboardMapping;
}
