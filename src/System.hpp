#ifndef SYSTEM_HPP_
#define SYSTEM_HPP_

#include <memory>
#include <string>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "Debugger/Debugger.hpp"
#include "Core/CPU.hpp"
#include "Core/BIOS.hpp"
#include "Core/RAM.hpp"
#include "Core/Bus.hpp"

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
