#ifndef REGISTERWINDOW_HPP_
#define REGISTERWINDOW_HPP_

#include "IWindow.hpp"

#include <memory>
#include <vector>
#include <string>

class CPU;

class RegisterWindow : public IWindow
{
    public:
        RegisterWindow(const std::shared_ptr<CPU> &cpu);
        ~RegisterWindow();

        void update() override;
        void AddEditButton(const char* regName, int regIndex);
        void DisplayPopup();

    private:
        std::shared_ptr<CPU> m_cpu;
        std::vector<uint32_t> previousRegisters;
        std::vector<uint32_t> previousExtraRegisters;
        std::vector<uint32_t> previousCopRegisters;

        bool m_editorOpen;
        std::string m_registerNameToChange;
        int m_registerIndex;
};

#endif /* !REGISTERWINDOW_HPP_ */
