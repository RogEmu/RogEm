#ifndef ASSEMBLYWINDOW_HPP_
#define ASSEMBLYWINDOW_HPP_

#include "imgui.h"
#include "IWindow.hpp"
#include <memory>

struct CPU;
class Debugger;

class AssemblyWindow : public IWindow
{
    public:
        AssemblyWindow(Debugger *debugger);
        ~AssemblyWindow();

        void update() override;

    private:
        void drawTopBar();
        void drawAssembly();
        void drawAssemblyLine(uint32_t addr);

        void jumpToPC();
        void findPcCursor(uint32_t startAddr, const ImGuiListClipper &clipper);

    private:
        bool m_autoFollowPc;
        float m_pcCursorPosY;
        Debugger *m_debugger;
};

#endif /* !ASSEMBLYWINDOW_HPP_ */
