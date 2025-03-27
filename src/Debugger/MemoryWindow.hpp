#ifndef MEMORYWINDOW_HPP_
#define MEMORYWINDOW_HPP_

#include "IWindow.hpp"
#include <memory>

struct CPU;

class MemoryWindow : public IWindow
{
    public:
        MemoryWindow(const std::shared_ptr<CPU> &cpu);
        ~MemoryWindow();

        void update() override;

    private:
        std::shared_ptr<CPU> m_cpu;
};

#endif /* !MEMORYWINDOW_HPP_ */
