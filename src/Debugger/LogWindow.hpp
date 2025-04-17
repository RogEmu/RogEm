#ifndef LOGRWINDOW_HPP_
#define LOGRWINDOW_HPP_

#include "imgui.h"
#include "IWindow.hpp"
#include <vector>
#include <string>

class Debugger;

class LogWindow : public IWindow
{
    public:
        LogWindow(Debugger *debugger);
        ~LogWindow();

        void addLog(const std::string &log);
        void update() override;
    private:
        void drawTopBar();
        void drawLogsWindow();

        bool m_autoScroll;
        Debugger *m_debugger;
        std::vector<std::string> m_logs;
};

#endif /* !LOGWINDOW_HPP_ */
