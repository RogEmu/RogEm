#ifndef APPLICATION_HPP_
#define APPLICATION_HPP_

#include <string>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "Core/System.hpp"
#include "Debugger/Debugger.hpp"
#include "GUI/MainMenuBar.hpp"
#include "imgui/imgui_memory_editor.h"

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

        std::list<std::shared_ptr<IWindow>> &getWindows() { return m_windows; }
        System &getSystem() { return m_system; }
        Debugger &getDebugger() { return m_debugger; }

    private:
        int initGlfw();
        int initImgui();
        void initVramTexture();
        void initWindows();

        void update();
        void render();

        void drawScreen();
        void pollGamepad();

    private:
        bool m_isRunning;
        EmulatorConfig m_config;

        System m_system;
        Debugger m_debugger;

        std::unique_ptr<MainMenuBar> m_mainMenuBar;
        std::list<std::shared_ptr<IWindow>> m_windows;
        MemoryEditor m_vramEditor;
        GLFWwindow* m_window;
        GLuint m_vramTexture;
        bool m_showDisplayArea = true;
};

#endif /* !APPLICATION_HPP_ */
