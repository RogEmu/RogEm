#ifndef SYSTEM_HPP_
#define SYSTEM_HPP_

#include <memory>
#include <string>

#include "Debugger/Debugger.hpp"
#include "Core/CPU.hpp"
#include "Core/BIOS.hpp"
#include "Core/Bus.hpp"

enum class SystemState
{
    RUNNING,
    PAUSED
};

class System
{
    public:
        System();
        ~System();

        int init();
        void update();

        CPU *getCPU();
        Bus *getBus();

        void setExecutablePath(const std::string &path);

        void loadBios(const char *path);
        void loadExecutable(const char *path);

        void attachDebugger(Debugger *debugger);

    private:
        void updateDebugger();

    private:
        std::unique_ptr<Bus> m_bus;
        std::unique_ptr<CPU> m_cpu;

        Debugger *m_debugger;
        bool m_debuggerAttached;

        SystemState m_state;
        std::string m_executablePath;
};

#endif /* !SYSTEM_HPP_ */
