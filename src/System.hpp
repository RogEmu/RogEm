#ifndef SYSTEM_HPP_
#define SYSTEM_HPP_

#include <memory>
#include <string>

#include <glad/glad.h>
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

struct GamepadState {
    uint16_t buttonsMask = 0xFFFF; // active-LOW
    uint8_t  lx = 0x80, ly = 0x80, rx = 0x80, ry = 0x80;
    bool     present = false;
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

        void update();
        void render();

        void loadPsxExe(const char *path);

        void initVramTexture();

    private:
        std::unique_ptr<CPU> m_cpu;
        std::unique_ptr<Bus> m_bus;

        std::unique_ptr<Debugger> m_debug;

        EmulatorConfig m_emuConfig;

        GLFWwindow* m_window;
        bool m_isRunning;

        GLuint m_vramTexture;
};

#endif /* !SYSTEM_HPP_ */
