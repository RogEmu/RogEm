#ifndef INSTRUCTIONWINDOW_HPP_
#define INSTRUCTIONWINDOW_HPP_

#include "IWindow.hpp"
#include <memory>

struct CPU;
class Debugger;

class InstructionWindow : public IWindow
{
    public:
        InstructionWindow(Debugger *debugger);
        ~InstructionWindow();

        void update() override;

    private:
        void drawTopBar();
        void drawAssembly();

    private:
        Debugger *m_debugger;
};

#endif /* !INSTRUCTIONWINDOW_HPP_ */
