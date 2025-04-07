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

        uint32_t getAddressFromLine(int line);
        int getLineFromAddress(uint32_t addr);

        void jumpToPC();
        void updatePcCursor(float itemHeight);

    private:
        bool m_jumpToPc;
        bool m_autoFollowPc;
        float m_pcCursor;
        Debugger *m_debugger;
};

#endif /* !ASSEMBLYWINDOW_HPP_ */
