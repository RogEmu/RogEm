#ifndef BREAKPOINTWINDOW_HPP_
#define BREAKPOINTWINDOW_HPP_

#include "IWindow.hpp"
#include <cstdint>
#include <vector>
#include <string>

class Debugger;

class BreakpointWindow : public IWindow
{
    public:
        BreakpointWindow(Debugger *debugger);
        ~BreakpointWindow();

        void update() override;

    private:
        void AddBreakpointButton();
        void DisplayBreakpoints();

    private:
        Debugger *m_debugger;
        int m_breakpointType;
        bool m_displayBreakpoints;
};

#endif