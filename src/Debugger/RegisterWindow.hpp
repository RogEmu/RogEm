#ifndef REGISTERWINDOW_HPP_
#define REGISTERWINDOW_HPP_

#include "IWindow.hpp"

#include <vector>
#include <string>
#include <cstdint>

class Debugger;

class RegisterWindow : public IWindow
{
    public:
        RegisterWindow(Debugger *debugger);
        ~RegisterWindow();

        void update() override;
        void AddEditButton(const char* regName, int regIndex);
        void DisplayPopup();

    private:
        Debugger *m_debugger;
        std::vector<uint32_t> previousRegisters;
        std::vector<uint32_t> previousExtraRegisters;
        std::vector<uint32_t> previousCopRegisters;

        bool m_editorOpen;
        std::string m_registerNameToChange;
        int m_registerIndex;
};

#endif /* !REGISTERWINDOW_HPP_ */
