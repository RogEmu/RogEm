#ifndef SETTINGSWINDOW_HPP_
#define SETTINGSWINDOW_HPP_

#include "imgui.h"
#include "IWindow.hpp"

class Debugger;

class SettingsWindow : public IWindow 
{
    public:
        SettingsWindow(Debugger *debugger);
        ~SettingsWindow();

        void update() override;

    private:
        void resetButton();

        Debugger *m_debugger;
};


#endif /* !SETTINGSWINDOW_HPP_ */
