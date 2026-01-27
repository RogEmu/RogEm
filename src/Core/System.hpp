#ifndef SYSTEM_HPP_
#define SYSTEM_HPP_

#include <memory>
#include <string>
#include <functional>

#include "CPU.hpp"
#include "BIOS.hpp"
#include "Bus.hpp"

class Debugger;

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
        void tick();
        void update();
        void reset();
        void setState(SystemState state) { m_state = state; }
        SystemState getState() const { return m_state; }

        CPU *getCPU();
        Bus *getBus();

        void setExecutablePath(const std::string &path);

        void loadBios(const char *path);
        void loadExecutable(const char *path);
        void updatePadInputs(uint16_t buttonsPort);

        void setDebuggerCallback(const std::function<void()> &callback);
        void setTtyCallback(const std::function<void(const std::string &)> &callback);

    private:
        std::unique_ptr<Bus> m_bus;
        std::unique_ptr<CPU> m_cpu;
        std::function<void(const std::string &)> m_ttyCallback;
        std::function<void()> m_debuggerCallback;

        SystemState m_state;
        std::string m_executablePath;
};

#endif /* !SYSTEM_HPP_ */
