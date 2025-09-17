#ifndef ASSEMBLYWINDOW_HPP_
#define ASSEMBLYWINDOW_HPP_

#include "IWindow.hpp"

#include "imgui.h"
#include <memory>

#include "Core/CPU.hpp"
#include "Debugger/Debugger.hpp"

#define GREY ImColor(171, 183, 183, 255)

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
        void drawContextMenu(uint32_t addr, bool isSelected, bool hasBreakpoint);

        uint32_t getAddressFromLine(int line);
        int getLineFromAddress(uint32_t addr);

        void jumpToPC();
        void updatePcCursor(float itemHeight);

    private:
        bool m_jumpToPc;
        bool m_autoFollowPc;
        float m_pcCursor;
        Debugger *m_debugger;
        uint32_t m_selectedAddr;
};

#endif /* !ASSEMBLYWINDOW_HPP_ */
