#include "InputMappingWindow.hpp"
#include <imgui.h>

InputMappingWindow::InputMappingWindow(InputManager* manager)
    : m_inputManager(manager)
{
    setTitle("Input Mapping");

    m_buttonsList = {
        PAD_CROSS, PAD_CIRCLE, PAD_SQUARE, PAD_TRIANGLE,
        PAD_L1, PAD_R1, PAD_L2, PAD_R2,
        PAD_START, PAD_SELECT,
        PAD_JOYUP, PAD_JOYDOWN, PAD_JOYLEFT, PAD_JOYRIGHT
    };

    m_originalMapping = m_inputManager->getKeyboardMappingCopy();
    m_editMapping = m_originalMapping;

    m_originalGamepadMapping = m_inputManager->getGamepadMappingCopy();
    m_editGamepadMapping = m_originalGamepadMapping;
}

void InputMappingWindow::update()
{
    if (auto pending = m_inputManager->consumePendingGamepadAssignment())
    {
        int key = pending->first;
        PadButton btn = pending->second;

        // supprimer ancienne assignation de ce bouton
        for (auto it = m_editGamepadMapping.begin(); it != m_editGamepadMapping.end(); )
        {
            if (it->second == btn)
                it = m_editGamepadMapping.erase(it);
            else
                ++it;
        }

        m_editGamepadMapping[key] = btn;
    }

    if (ImGui::Begin("Input Mapping"))
    {
        ImGui::Text("Keyboard Mapping");
        ImGui::Separator();

        auto findKeyFor = [](const std::unordered_map<int, PadButton>& map, PadButton btn)
        {
            for (auto& [k,v] : map)
                if (v == btn)
                    return k;
            return -1;
        };

        for (auto button : m_buttonsList)
        {
            int originalKey = findKeyFor(m_originalMapping, button);
            int editedKey   = findKeyFor(m_editMapping, button);
            bool modified   = (originalKey != editedKey);

            ImGui::Text("%s", padButtonToString(button));
            ImGui::SameLine(150);

            std::string keyName = getKeyNameForButton(button);

            if (modified)
                ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(255, 80, 80, 255));

            if (ImGui::Button(keyName.c_str(), ImVec2(160, 0)))
            {
                m_waitingForKey = button;
            }

            if (modified)
                ImGui::PopStyleColor();
        }

        ImGui::Separator();

        if (m_waitingForKey.has_value())
            ImGui::TextColored(ImVec4(1,0,0,1), "Press a key...(Esc to cancel)");

        ImGui::Separator();

        ImGui::Spacing();
        ImGui::Spacing();

        ImGui::Text("Gamepad Mapping");
        ImGui::Separator();

        auto findGamepadKeyFor = [](const std::unordered_map<int, PadButton>& map, PadButton btn)
        {
            for (auto& [k,v] : map)
                if (v == btn)
                    return k;
            return -9999; // valeur impossible
        };

        for (auto button : m_buttonsList)
        {
            int originalKey = findGamepadKeyFor(m_originalGamepadMapping, button);
            int editedKey   = findGamepadKeyFor(m_editGamepadMapping, button);
            bool modified   = (originalKey != editedKey);

            ImGui::Text("%s", padButtonToString(button));
            ImGui::SameLine(150);

            std::string gpName = getGamepadNameForButton(button);

            if (modified)
                ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(255, 80, 80, 255));

            if (ImGui::Button(gpName.c_str(), ImVec2(160, 0)))
            {
                m_inputManager->startGamepadRebinding(button);
            }

            if (modified)
                ImGui::PopStyleColor();
        }

        ImGui::Separator();

        if (m_inputManager->isGamepadRebinding())
        {
            ImGui::TextColored(ImVec4(1,0,0,1),
                "Press a gamepad button... (Esc to cancel)");
        }

        ImGui::Separator();

        if (ImGui::Button("Save Gamepad"))
        {
            m_inputManager->setGamepadMapping(m_editGamepadMapping);
            m_inputManager->saveGamepadMappingToFile("input_mapping.json");
            m_originalGamepadMapping = m_editGamepadMapping;
        }

        ImGui::SameLine();

        if (ImGui::Button("Reset Gamepad"))
        {
            m_inputManager->resetGamepadMappingToDefault();

            m_originalGamepadMapping = m_inputManager->getGamepadMappingCopy();
            m_editGamepadMapping = m_originalGamepadMapping;
        }

        ImGui::SameLine();

        if (ImGui::Button("Cancel Gamepad"))
        {
            m_editGamepadMapping = m_originalGamepadMapping;
        }

        if (ImGui::Button("Save"))
        {
            m_inputManager->setKeyboardMapping(m_editMapping);
            m_inputManager->saveKeyboardMappingToFile("input_mapping.json");
            m_originalMapping = m_editMapping;
        }

        ImGui::SameLine();

        if (ImGui::Button("Reset to Default"))
        {
            m_inputManager->resetKeyboardMappingToDefault();

            m_originalMapping = m_inputManager->getKeyboardMappingCopy();
            m_editMapping = m_originalMapping;

            m_waitingForKey.reset();
        }

        ImGui::SameLine();

        if (ImGui::Button("Cancel"))
        {
            m_editMapping = m_originalMapping;
            m_waitingForKey.reset();
        }
    }
    ImGui::End();
}

const char* InputMappingWindow::padButtonToString(PadButton button)
{
    switch (button)
    {
        case PAD_CROSS: return "Cross";
        case PAD_CIRCLE: return "Circle";
        case PAD_SQUARE: return "Square";
        case PAD_TRIANGLE: return "Triangle";
        case PAD_L1: return "L1";
        case PAD_R1: return "R1";
        case PAD_L2: return "L2";
        case PAD_R2: return "R2";
        case PAD_START: return "Start";
        case PAD_SELECT: return "Select";
        case PAD_JOYUP: return "Up";
        case PAD_JOYDOWN: return "Down";
        case PAD_JOYLEFT: return "Left";
        case PAD_JOYRIGHT: return "Right";
        default: return "Unknown";
    }
}

std::string InputMappingWindow::getKeyNameForButton(PadButton button)
{
    for (auto& [key, pad] : m_editMapping)
    {
        if (pad == button)
            return glfwGetKeyName(key, 0) ? glfwGetKeyName(key, 0) : std::to_string(key);
    }
    return "Unassigned";
}

void InputMappingWindow::onGlfwKeyPressed(int glfwKey)
{
    if (!m_waitingForKey.has_value())
        return;

    PadButton btn = m_waitingForKey.value();

    // enlever ancienne assignation de ce bouton
    for (auto it = m_editMapping.begin(); it != m_editMapping.end(); )
    {
        if (it->second == btn)
            it = m_editMapping.erase(it);
        else
            ++it;
    }

    // Assignation nouvelle touche
    m_editMapping[glfwKey] = btn;

    m_waitingForKey.reset();
}

std::string InputMappingWindow::getGamepadNameForButton(PadButton button)
{
    const auto& mapping = m_editGamepadMapping;

    for (auto& [id, pad] : mapping)
    {
        if (pad == button)
        {
            switch (id)
            {
                case GLFW_GAMEPAD_BUTTON_A: return "A";
                case GLFW_GAMEPAD_BUTTON_B: return "B";
                case GLFW_GAMEPAD_BUTTON_X: return "X";
                case GLFW_GAMEPAD_BUTTON_Y: return "Y";
                case GLFW_GAMEPAD_BUTTON_LEFT_BUMPER:  return "LB";
                case GLFW_GAMEPAD_BUTTON_RIGHT_BUMPER: return "RB";
                case GLFW_GAMEPAD_BUTTON_BACK:  return "Back";
                case GLFW_GAMEPAD_BUTTON_START: return "Start";
                case GLFW_GAMEPAD_BUTTON_DPAD_UP:    return "DPad Up";
                case GLFW_GAMEPAD_BUTTON_DPAD_DOWN:  return "DPad Down";
                case GLFW_GAMEPAD_BUTTON_DPAD_LEFT:  return "DPad Left";
                case GLFW_GAMEPAD_BUTTON_DPAD_RIGHT: return "DPad Right";
                case -1: return "L2";
                case -2: return "R2";
                default: return "Unknown";
            }
        }
    }

    return "Unassigned";
}
