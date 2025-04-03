#ifndef BREAKPOINTWINDOW_HPP_
#define BREAKPOINTWINDOW_HPP_

#include "IWindow.hpp"
#include <cstdint>
#include <vector>
#include <string>

class Debugger;

struct Breakpoint
{
    uint32_t addr;
    int type;
    std::string label;
    bool enabled;
};

class BreakpointWindow : public IWindow
{
    public:
        BreakpointWindow(Debugger *debugger);
        ~BreakpointWindow();

        void update() override;
        std::vector<Breakpoint> GetBreakpoints() const;

    private:
        void AddBreakpointButton();
        void DisplayBreakpoints();

    private:
        Debugger *m_debugger;
        std::vector<Breakpoint> m_breakpoints;
        int m_breakpointType;
        bool m_displayBreakpoints;
};

#endif