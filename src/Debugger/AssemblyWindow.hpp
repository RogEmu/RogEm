#ifndef ASSEMBLYWINDOW_HPP_
#define ASSEMBLYWINDOW_HPP_

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

    private:
        Debugger *m_debugger;
};

#endif /* !ASSEMBLYWINDOW_HPP_ */
