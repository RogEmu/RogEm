#ifndef MEMORYWINDOW_HPP_
#define MEMORYWINDOW_HPP_

#include "IWindow.hpp"
#include <cstdint>

#include "imgui.h"
#include "imgui/imgui_memory_editor.h"

struct CPU;
class Debugger;

class MemoryWindow : public IWindow
{
    public:
        MemoryWindow(Debugger *debugger);
        ~MemoryWindow();

        void update() override;

        void setTitle(const char *title);
        void setBaseAddr(uint32_t baseAddr);
        void setReadOnly(bool readOnly);

    private:
        void drawEditor(const char *title, uint32_t baseAddr);

    private:
        Debugger *m_debugger;
        MemoryEditor m_memEdit;

        const char *m_title;
        uint32_t m_baseAddr;
};

#endif /* !MEMORYWINDOW_HPP_ */
