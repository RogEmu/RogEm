#ifndef REGISTERWINDOW_HPP_
#define REGISTERWINDOW_HPP_

#include "IWindow.hpp"
#include <memory>

class CPU;

class RegisterWindow : public IWindow
{
    public:
        RegisterWindow(const std::shared_ptr<CPU> &cpu);
        ~RegisterWindow();

        void update() override;

    private:
        std::shared_ptr<CPU> m_cpu;
};

#endif /* !REGISTERWINDOW_HPP_ */
