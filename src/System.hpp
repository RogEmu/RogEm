#ifndef SYSTEM_HPP_
#define SYSTEM_HPP_

#include <memory>

#include "Debugger.hpp"

class BIOS;
struct CPU;
struct GLFWwindow;

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
        GLFWwindow* m_window;
        bool m_isRunning;
};

#endif /* !SYSTEM_HPP_ */
