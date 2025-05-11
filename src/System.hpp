#ifndef SYSTEM_HPP_
#define SYSTEM_HPP_

#include <memory>
#include <string>

#include <GLFW/glfw3.h>

#include "Debugger.hpp"
#include "core/CPU.h"
#include "core/BIOS.h"
#include "core/RAM.h"
#include "core/Bus.h"

struct EmulatorConfig
{
    std::string biosFilePath;
    std::string exeFilePath;
};

class System
{
    public:
        System();
        ~System();

        int init(const EmulatorConfig &config);
        void run();
        bool isRunning() const;

        CPU *getCPU();
        Bus *getBus();

    private:
        int initGFLW();
        int initImGUi();

        double deltaTime();

        void loadPsxExe(const char *path);

    private:
        std::unique_ptr<CPU> m_cpu;
        std::unique_ptr<RAM> m_ram;
        std::unique_ptr<BIOS> m_bios;
        std::unique_ptr<Bus> m_bus;

        std::unique_ptr<Debugger> m_debug;

        EmulatorConfig m_emuConfig;

        GLFWwindow* m_window;
        bool m_isRunning;

        double m_lastTime;
};

#endif /* !SYSTEM_HPP_ */
