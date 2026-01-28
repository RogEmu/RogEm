#ifndef REGISTERWINDOW_HPP_
#define REGISTERWINDOW_HPP_

#include "IWindow.hpp"

#include <string>
#include <cstdint>
#include <vector>

class Debugger;

class RegisterWindow : public IWindow
{
    public:
        RegisterWindow(Debugger *debugger);
        ~RegisterWindow();

        void update() override;

    private:
        void editRegisterPopup(const std::string &regName, int regIndex);
        void drawGpr();
        void drawSpecialRegs();
        void drawCop0Regs();
        void updateRegisterChanges();

        void drawRegister(uint8_t index);

    private:
        Debugger *m_debugger;
        uint32_t m_prevGPR[32];
        uint32_t m_currentGPR[32];
        std::vector<bool> isHighlighted;
        uint32_t m_prevSpecialRegs[3];
        uint32_t m_prevCop0Regs[16];
};

#endif /* !REGISTERWINDOW_HPP_ */
