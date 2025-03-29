#ifndef REGISTERWINDOW_HPP_
#define REGISTERWINDOW_HPP_

#include "IWindow.hpp"

#include <string>
#include <cstdint>

class Debugger;

class RegisterWindow : public IWindow
{
    public:
        RegisterWindow(Debugger *debugger);
        ~RegisterWindow();

        void update() override;

    private:
        void addEditButton(const char* regName, int regIndex);
        void displayPopup();
        void drawGpr();
        void drawSpecialRegs();
        void drawCop0Regs();

        void drawRegister(uint8_t index);

    private:
        Debugger *m_debugger;
        uint32_t m_prevGPR[32];
        uint32_t m_prevSpecialRegs[3];
        uint32_t m_prevCop0Regs[16];

        bool m_editorOpen;
        std::string m_registerNameToChange;
        int m_registerIndex;
};

#endif /* !REGISTERWINDOW_HPP_ */
