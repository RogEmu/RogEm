/*
** EPITECH PROJECT, 2026
** rogem
** File description:
** KeybindWindow
*/

#ifndef KEYBINDWINDOW_HPP_
#define KEYBINDWINDOW_HPP_

#include "IWindow.hpp"

class InputManager;

class KeybindWindow : public IWindow
{
    public:
        KeybindWindow(InputManager *inputManager);
        void update() override;

    private:
        InputManager *m_inputManager;
};

#endif /* !KEYBINDWINDOW_HPP_ */
