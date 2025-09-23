#ifndef APPLICATION_HPP_
#define APPLICATION_HPP_

#include <string>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "System.hpp"
#include "Debugger/Debugger.hpp"

struct EmulatorConfig
{
    std::string biosFilePath;
    std::string exeFilePath;
};

class Application
{
    public:
        Application();
        ~Application();

        int run();
        void quit();
        int loadConfig(int ac, char **av);

    private:
        int initGlfw();
        int initImgui();
        void initVramTexture();

        void update();
        void render();

        void drawScreen();

    private:
        bool m_isRunning;
        EmulatorConfig m_config;

        System m_system;
        Debugger m_debugger;

        GLFWwindow* m_window;
        GLuint m_vramTexture;
};

#endif /* !APPLICATION_HPP_ */
