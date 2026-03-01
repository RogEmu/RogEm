/*
** EPITECH PROJECT, 2026
** rogem
** File description:
** KeybindWindow
*/

#include "KeybindWindow.hpp"
#include "InputManager.hpp"

#include <imgui.h>
#include <GLFW/glfw3.h>

#include <array>

static const std::array<PadButton, 14> buttonOrder = {
    PadButton::PAD_JOYUP,
    PadButton::PAD_JOYDOWN,
    PadButton::PAD_JOYLEFT,
    PadButton::PAD_JOYRIGHT,
    PadButton::PAD_CROSS,
    PadButton::PAD_CIRCLE,
    PadButton::PAD_SQUARE,
    PadButton::PAD_TRIANGLE,
    PadButton::PAD_L1,
    PadButton::PAD_R1,
    PadButton::PAD_L2,
    PadButton::PAD_R2,
    PadButton::PAD_START,
    PadButton::PAD_SELECT,
};

static std::string getKeyName(int keyCode)
{
    const char *name = glfwGetKeyName(keyCode, 0);
    if (name) {
        std::string result(name);
        for (auto &c : result) {
            c = static_cast<char>(std::toupper(static_cast<unsigned char>(c)));
        }
        return result;
    }
    switch (keyCode) {
        case GLFW_KEY_UP:            return "UP";
        case GLFW_KEY_DOWN:          return "DOWN";
        case GLFW_KEY_LEFT:          return "LEFT";
        case GLFW_KEY_RIGHT:         return "RIGHT";
        case GLFW_KEY_ENTER:         return "ENTER";
        case GLFW_KEY_SPACE:         return "SPACE";
        case GLFW_KEY_TAB:           return "TAB";
        case GLFW_KEY_BACKSPACE:     return "BACKSPACE";
        case GLFW_KEY_ESCAPE:        return "ESCAPE";
        case GLFW_KEY_LEFT_SHIFT:    return "LEFT SHIFT";
        case GLFW_KEY_RIGHT_SHIFT:   return "RIGHT SHIFT";
        case GLFW_KEY_LEFT_CONTROL:  return "LEFT CTRL";
        case GLFW_KEY_RIGHT_CONTROL: return "RIGHT CTRL";
        case GLFW_KEY_LEFT_ALT:      return "LEFT ALT";
        case GLFW_KEY_RIGHT_ALT:     return "RIGHT ALT";
        default:                     return "KEY " + std::to_string(keyCode);
    }
}

KeybindWindow::KeybindWindow(InputManager *inputManager) :
    m_inputManager(inputManager)
{
    setTitle("Keybinds");
    setVisible(false);
}

void KeybindWindow::update()
{
    if (ImGui::Begin(getTitleChar())) {
        if (ImGui::BeginTable("keybinds", 3, ImGuiTableFlags_RowBg | ImGuiTableFlags_Borders)) {
            ImGui::TableSetupColumn("Button", ImGuiTableColumnFlags_WidthFixed, 100.0f);
            ImGui::TableSetupColumn("Key", ImGuiTableColumnFlags_WidthStretch);
            ImGui::TableSetupColumn("##action", ImGuiTableColumnFlags_WidthFixed, 80.0f);
            ImGui::TableHeadersRow();

            PadButton listeningButton = m_inputManager->getListeningButton();
            const auto &buttonToKey = m_inputManager->getButtonToKey();

            for (PadButton button : buttonOrder) {
                ImGui::TableNextRow();
                ImGui::TableNextColumn();
                ImGui::TextUnformatted(InputManager::padButtonToString(button).c_str());

                ImGui::TableNextColumn();
                if (m_inputManager->isListening() && listeningButton == button) {
                    ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), "Press a key...");
                } else {
                    auto it = buttonToKey.find(button);
                    if (it != buttonToKey.end()) {
                        ImGui::TextUnformatted(getKeyName(it->second).c_str());
                    } else {
                        ImGui::TextDisabled("Unbound");
                    }
                }

                ImGui::TableNextColumn();
                ImGui::PushID(static_cast<int>(button));
                if (m_inputManager->isListening() && listeningButton == button) {
                    if (ImGui::SmallButton("Cancel")) {
                        m_inputManager->cancelListening();
                    }
                } else {
                    bool disabled = m_inputManager->isListening();
                    if (disabled) {
                        ImGui::BeginDisabled();
                    }
                    if (ImGui::SmallButton("Rebind")) {
                        m_inputManager->startListening(button);
                    }
                    if (disabled) {
                        ImGui::EndDisabled();
                    }
                }
                ImGui::PopID();
            }
            ImGui::EndTable();
        }

        ImGui::Spacing();
        if (ImGui::Button("Reset to Defaults")) {
            m_inputManager->cancelListening();
            m_inputManager->resetToDefaults();
        }
    }
    ImGui::End();
}
