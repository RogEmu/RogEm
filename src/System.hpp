#ifndef SYSTEM_HPP_
#define SYSTEM_HPP_

#include <memory>
#include <string>

#include "Debugger.hpp"

class BIOS;
class RAM;
class Bus;
struct CPU;
struct GLFWwindow;

class System
{
    public:
        System(const std::string &biosPath);
        ~System();

        void run();
        bool isRunning() const;

        CPU *getCPU();
        Bus *getBus();

    private:
        int initGFLW();
        int initImGUi();

        double deltaTime();

    private:
        std::unique_ptr<CPU> m_cpu;
        std::unique_ptr<RAM> m_ram;
        std::unique_ptr<BIOS> m_bios;
        std::unique_ptr<Bus> m_bus;

        std::unique_ptr<Debugger> m_debug;

        GLFWwindow* m_window;
        bool m_isRunning;

        double m_lastTime;
};

#endif /* !SYSTEM_HPP_ */
