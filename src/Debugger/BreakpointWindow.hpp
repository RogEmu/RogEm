#ifndef BREAKPOINTWINDOW_HPP_
#define BREAKPOINTWINDOW_HPP_

#include "IWindow.hpp"
#include <cstdint>
#include <vector>

class Debugger;

class BreakpointWindow : public IWindow
{
    public:
        BreakpointWindow(Debugger *debugger);
        ~BreakpointWindow();

        void update() override;
    private:
        void AddBreakpointButton();

    private:
        Debugger *m_debugger;
        std::vector<uint32_t> m_breakpoints;
};

#endif