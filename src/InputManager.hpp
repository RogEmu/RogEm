/*
** EPITECH PROJECT, 2026
** rogem
** File description:
** InputManager
*/

#ifndef INPUTMANAGER_HPP_
#define INPUTMANAGER_HPP_

#include <cstdint>
#include <string>
#include <unordered_map>

#include "Core/DigitalPad.hpp"

class InputManager
{
    public:
        InputManager();
        ~InputManager() = default;

        void onKeyEvent(int key, int action);
        uint16_t getButtonState() const;

        void setKeybinding(int keyCode, PadButton button);
        void removeKeybinding(int keyCode);
        void clearKeybindings();
        void resetToDefaults();
        const std::unordered_map<int, PadButton>& getKeybindings() const;

        bool saveToFile(const std::string& filePath) const;
        bool loadFromFile(const std::string& filePath);

    private:
        void initDefaultKeybindings();

        std::unordered_map<int, PadButton> m_keybindings;
        std::unordered_map<PadButton, int> m_buttonToKey;
        uint16_t m_buttonState;
};

#endif /* !INPUTMANAGER_HPP_ */
