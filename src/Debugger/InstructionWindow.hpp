#ifndef INSTRUCTIONWINDOW_HPP_
#define INSTRUCTIONWINDOW_HPP_

#include "IWindow.hpp"
#include <memory>

struct CPU;

class InstructionWindow : public IWindow
{
    public:
        InstructionWindow(const std::shared_ptr<CPU> &cpu, bool &systemPaused, float &simSpeed);
        ~InstructionWindow();

        void update() override;

    private:
        std::shared_ptr<CPU> m_cpu;

        bool &m_systemPaused;
        float &m_simSpeed;
};

#endif /* !INSTRUCTIONWINDOW_HPP_ */
