#ifndef MEMORYWINDOW_HPP_
#define MEMORYWINDOW_HPP_

#include "IWindow.hpp"
#include <memory>

struct CPU;
class Debugger;

class MemoryWindow : public IWindow
{
    public:
        MemoryWindow(Debugger *debugger);
        ~MemoryWindow();

        void update() override;

    private:
        Debugger *m_debugger;
};

#endif /* !MEMORYWINDOW_HPP_ */
