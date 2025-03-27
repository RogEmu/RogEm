#ifndef REGISTERWINDOW_HPP_
#define REGISTERWINDOW_HPP_

#include "IWindow.hpp"

#include <memory>
#include <vector>

class CPU;

class RegisterWindow : public IWindow
{
    public:
        RegisterWindow(const std::shared_ptr<CPU> &cpu);
        ~RegisterWindow();

        void update() override;

    private:
        std::shared_ptr<CPU> m_cpu;
        std::vector<uint32_t> previousRegisters;
        std::vector<uint32_t> previousExtraRegisters;
        std::vector<uint32_t> previousCopRegisters;
};

#endif /* !REGISTERWINDOW_HPP_ */
