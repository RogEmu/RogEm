#ifndef SETTINGSWINDOW_HPP_
#define SETTINGSWINDOW_HPP_

#include "imgui.h"
#include "IWindow.hpp"

class Debugger;
class MemoryWindow;

class SettingsWindow : public IWindow 
{
    public:
        SettingsWindow(Debugger *debugger, MemoryWindow* memoryWindow);
        ~SettingsWindow();

        void update() override;

    private:
        void resetButton();
        void dumpBIOSButton();

        Debugger *m_debugger;
        MemoryWindow* m_memoryWindow;
};


#endif /* !SETTINGSWINDOW_HPP_ */
