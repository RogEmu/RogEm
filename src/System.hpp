#ifndef SYSTEM_HPP_
#define SYSTEM_HPP_

#include "Debugger.hpp"

#include "CPU.h"
#include "BIOS.h"
#include "RAM.h"

class System
{
    public:
        System(const std::shared_ptr<BIOS> &cpu);
        ~System();

        void run();
        bool isRunning() const;

    private:
        int initGFLW();
        int initImGUi();

    private:
        std::shared_ptr<Debugger> m_debug;
        std::shared_ptr<CPU> m_cpu;
        GLFWwindow* window;
        bool is_running;
};

#endif /* !SYSTEM_HPP_ */