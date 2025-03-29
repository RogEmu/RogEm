#ifdef _WIN32
    #include "Windows.h"
#endif //_WIN32

#include <GLFW/glfw3.h>
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

#include <iostream>
#include <memory>
#include <string>

#include "System.hpp"
#include "RAM.h"
#include "CPU.h"
#include "BIOS.h"
#include "Bus.h"

System::System(const std::string &biosPath) :
    m_lastTime(0.0)
{
    m_bios = std::make_unique<BIOS>(biosPath);
    m_ram = std::make_unique<RAM>();
    m_bus = std::make_unique<Bus>(m_bios.get(), m_ram.get());
    m_cpu = std::make_unique<CPU>(m_bus.get());

    m_debug = std::make_unique<Debugger>(this);

    m_isRunning = true;
    if (initGFLW() != 0)
        m_isRunning = false;
    if (initImGUi() != 0)
        m_isRunning = false;
}

System::~System()
{
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
    glfwDestroyWindow(m_window);
    glfwTerminate();
}

bool System::isRunning() const
{
    return m_isRunning;
}

CPU *System::getCPU()
{
    return m_cpu.get();
}

Bus *System::getBus()
{
    return m_bus.get();
}

void newFrame()
{
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
}

void renderFrame()
{
    ImGui::Render();
    int display_w, display_h;
    glfwGetFramebufferSize(glfwGetCurrentContext(), &display_w, &display_h);
    glViewport(0, 0, display_w, display_h);
    glClear(GL_COLOR_BUFFER_BIT);
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    glfwSwapBuffers(glfwGetCurrentContext());
}

void System::run()
{
    double uiFps = 1/60.0;
    double uiTimer = 0.0;
    double simulationTimer = 0.0;
    double dt = 0.0;

    while (m_isRunning)
    {
        if (!m_debug->isPaused())
        {
            if (simulationTimer > (m_debug->getSimulationSpeed()))
            {
                m_cpu->step();
                simulationTimer = 0;
            }
        }
        if (uiTimer > uiFps)
        {
            glfwPollEvents();

            if (glfwGetWindowAttrib(m_window, GLFW_ICONIFIED) != 0) {
                // ImGui_ImplGlfw_Sleep(10);
                return;
            }
            if (glfwWindowShouldClose(m_window)) {
                m_isRunning = false;
                return;
            }
            newFrame();
            ImGui::DockSpaceOverViewport(0, ImGui::GetMainViewport());
            m_debug->update();
            uiTimer = 0;
            renderFrame();
        }
        dt = deltaTime();
        uiTimer += dt;
        simulationTimer += dt;
    }
}

void glfw_error_callback(int error_code, const char* description)
{
    fprintf(stderr, "GLFW Error %d: %s\n", error_code, description);
}

int System::initGFLW()
{
    glfwSetErrorCallback(glfw_error_callback);
    if (!glfwInit())
        return -1;

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);

    // Create a window
    m_window = glfwCreateWindow(1280, 720, "RogEm v0.0.1", nullptr, nullptr);
    if (!m_window)
        return -1;
    glfwMakeContextCurrent(m_window);
    glfwSwapInterval(1); // VSync
    return 0;
}

int System::initImGUi()
{
    const char *glsl_version = "#version 130";

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;           // Enable Docking
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;       // Enable Keyboard Controls
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;        // Enable Gamepad Controls
    io.Fonts->AddFontFromFileTTF("assets/fonts/JetBrainsMono/JetBrainsMono-Regular.ttf", 16);
    ImGui::StyleColorsDark();
    ImGui_ImplGlfw_InitForOpenGL(m_window, true);
    ImGui_ImplOpenGL3_Init(glsl_version);
    return 0;
}

double System::deltaTime()
{
    double currentTime = glfwGetTime();
    double dt = currentTime - m_lastTime;

    m_lastTime = currentTime;
    return dt;
}
