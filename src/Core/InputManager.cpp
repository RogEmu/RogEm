#include "InputManager.hpp"
#include "System.hpp"
#include <fstream>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

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
    m_defaultGamepadMapping = {
        { GLFW_GAMEPAD_BUTTON_A, PAD_CROSS },
        { GLFW_GAMEPAD_BUTTON_B, PAD_CIRCLE },
        { GLFW_GAMEPAD_BUTTON_X, PAD_SQUARE },
        { GLFW_GAMEPAD_BUTTON_Y, PAD_TRIANGLE },
        { GLFW_GAMEPAD_BUTTON_LEFT_BUMPER,  PAD_L1 },
        { GLFW_GAMEPAD_BUTTON_RIGHT_BUMPER, PAD_R1 },
        { GLFW_GAMEPAD_BUTTON_BACK,  PAD_SELECT },
        { GLFW_GAMEPAD_BUTTON_START, PAD_START },
        { GLFW_GAMEPAD_BUTTON_DPAD_UP,    PAD_JOYUP },
        { GLFW_GAMEPAD_BUTTON_DPAD_DOWN,  PAD_JOYDOWN },
        { GLFW_GAMEPAD_BUTTON_DPAD_LEFT,  PAD_JOYLEFT },
        { GLFW_GAMEPAD_BUTTON_DPAD_RIGHT, PAD_JOYRIGHT }
    };

    m_gamepadMapping = m_defaultGamepadMapping;
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

    if (isGamepadRebinding())
    {
        updateGamepadRebinding(state);
        return; // bloque gameplay
    }

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

void InputManager::cancelRebinding()
{
    m_waitingForKey.reset();
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

void InputManager::saveKeyboardMappingToFile(const std::string& path)
{
    json j;

    // Charger fichier existant si présent
    std::ifstream inFile(path);
    if (inFile.is_open())
    {
        inFile >> j;
        inFile.close();
    }

    // Mettre à jour uniquement la section keyboard
    j["keyboard"].clear();

    for (auto& [key, pad] : m_keyboardMapping)
    {
        j["keyboard"][std::to_string(key)] = static_cast<uint16_t>(pad);
    }

    std::ofstream outFile(path);
    if (outFile.is_open())
    {
        outFile << j.dump(4);
        outFile.close();
    }
}

void InputManager::loadKeyboardMappingFromFile(const std::string& path)
{
    std::ifstream file(path);
    if (!file.is_open())
        return;

    json j;
    file >> j;

    if (!j.contains("keyboard"))
        return;

    m_keyboardMapping.clear();

    for (auto& [keyStr, value] : j["keyboard"].items())
    {
        int key = std::stoi(keyStr);
        PadButton pad = static_cast<PadButton>(value.get<uint16_t>());
        m_keyboardMapping[key] = pad;
    }
}

void InputManager::saveGamepadMappingToFile(const std::string& path)
{
    json j;

    // Charger fichier existant si présent
    std::ifstream inFile(path);
    if (inFile.is_open())
    {
        inFile >> j;
        inFile.close();
    }

    // Mettre à jour uniquement la section gamepad
    j["gamepad"].clear();

    for (auto& [key, pad] : m_gamepadMapping)
    {
        j["gamepad"][std::to_string(key)] = static_cast<uint16_t>(pad);
    }

    std::ofstream outFile(path);
    if (outFile.is_open())
    {
        outFile << j.dump(4);
        outFile.close();
    }
}

void InputManager::loadGamepadMappingFromFile(const std::string& path)
{
    std::ifstream file(path);
    if (!file.is_open())
        return;

    json j;
    file >> j;

    if (!j.contains("gamepad"))
        return;

    m_gamepadMapping.clear();

    for (auto& [keyStr, value] : j["gamepad"].items())
    {
        int key = std::stoi(keyStr);
        PadButton pad = static_cast<PadButton>(value.get<uint16_t>());
        m_gamepadMapping[key] = pad;
    }
}

void InputManager::resetKeyboardMappingToDefault()
{
    m_keyboardMapping = getDefaultKeyboardMapping();
}

std::unordered_map<int, PadButton> InputManager::getDefaultKeyboardMapping() const
{
    return {
        { GLFW_KEY_UP,    PAD_JOYUP },
        { GLFW_KEY_DOWN,  PAD_JOYDOWN },
        { GLFW_KEY_LEFT,  PAD_JOYLEFT },
        { GLFW_KEY_RIGHT, PAD_JOYRIGHT },

        { GLFW_KEY_S,     PAD_CROSS },
        { GLFW_KEY_D,     PAD_CIRCLE },
        { GLFW_KEY_A,     PAD_SQUARE },
        { GLFW_KEY_W,     PAD_TRIANGLE },

        { GLFW_KEY_Q,     PAD_L1 },
        { GLFW_KEY_E,     PAD_R1 },
        { GLFW_KEY_1,     PAD_L2 },
        { GLFW_KEY_3,     PAD_R2 },

        { GLFW_KEY_ENTER, PAD_START },
        { GLFW_KEY_F,     PAD_SELECT }
    };
}

void InputManager::startGamepadRebinding(PadButton button)
{
    m_waitingGamepadButton = button;
}

void InputManager::cancelGamepadRebinding()
{
    m_waitingGamepadButton.reset();
}

bool InputManager::isGamepadRebinding() const
{
    return m_waitingGamepadButton.has_value();
}

void InputManager::updateGamepadRebinding(const GLFWgamepadstate& state)
{
    if (!m_waitingGamepadButton.has_value())
        return;

    PadButton target = m_waitingGamepadButton.value();

    // Boutons digitaux
    for (int i = 0; i <= GLFW_GAMEPAD_BUTTON_LAST; i++)
    {
        if (state.buttons[i] == GLFW_PRESS &&
            m_previousGamepadState.buttons[i] == GLFW_RELEASE)
        {
            // supprimer ancienne assignation
            for (auto it = m_gamepadMapping.begin(); it != m_gamepadMapping.end(); )
            {
                if (it->second == target)
                    it = m_gamepadMapping.erase(it);
                else
                    ++it;
            }

            m_pendingGamepadAssignment = {i, target};
            m_waitingGamepadButton.reset();
            break;
        }
    }

    m_previousGamepadState = state;
}

std::unordered_map<int, PadButton> InputManager::getGamepadMappingCopy() const
{
    return m_gamepadMapping;
}

std::optional<std::pair<int, PadButton>> InputManager::consumePendingGamepadAssignment()
{
    auto tmp = m_pendingGamepadAssignment;
    m_pendingGamepadAssignment.reset();
    return tmp;
}

void InputManager::setGamepadMapping(const std::unordered_map<int, PadButton>& newMapping)
{
    m_gamepadMapping = newMapping;
}

void InputManager::resetGamepadMappingToDefault()
{
    m_gamepadMapping = m_defaultGamepadMapping;
}
