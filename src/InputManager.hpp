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
        const std::unordered_map<PadButton, int>& getButtonToKey() const;

        void startListening(PadButton button);
        void cancelListening();
        bool isListening() const;
        PadButton getListeningButton() const;

        static std::string padButtonToString(PadButton button);

        bool saveToFile(const std::string& filePath) const;
        bool loadFromFile(const std::string& filePath);

    private:
        void initDefaultKeybindings();

        std::unordered_map<int, PadButton> m_keybindings;
        std::unordered_map<PadButton, int> m_buttonToKey;
        uint16_t m_buttonState;
        bool m_listening = false;
        PadButton m_listeningButton = PadButton::PAD_UNKOWN;
};

#endif /* !INPUTMANAGER_HPP_ */
