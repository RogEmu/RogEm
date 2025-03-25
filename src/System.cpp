#include <iostream>
#include <memory>
#include <string>
#include <chrono>
#include <GLFW/glfw3.h>
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

#include "System.hpp"

System::System(const std::shared_ptr<BIOS> &bios)
{
    auto ram = std::make_shared<RAM>();
    auto bus = std::make_shared<Bus>(bios, ram);
    auto cpu = std::make_shared<CPU>(bus);
    m_cpu = cpu;
    m_debug = std::make_shared<Debugger>(cpu);
    is_running = true;
    if (initGFLW() != 0)
        is_running = false;
    if (initImGUi() != 0)
        is_running = false;
}

System::~System()
{
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
    glfwDestroyWindow(window);
    glfwTerminate();
}

float deltaTime(std::chrono::steady_clock::time_point &lastTime)
{
    auto now = std::chrono::steady_clock::now();
    float deltaTime = std::chrono::duration<double>(now - lastTime).count();
    lastTime = now;

    return deltaTime;
}

bool System::isRunning() const
{
    return is_running;
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
    auto startTime = std::chrono::steady_clock::now();
    float uiFps = 1/60.0f;
    float uiTimer = 0.0f;
    float dt = 0;
    float simulationTimer = 0.0f;

    while (is_running)
    {
        if (!m_debug->isPaused())
        {
            if (simulationTimer > (m_debug->getSimSpeed()))
            {
                m_cpu->step();
                simulationTimer = 0;
            }
        }
        if (uiTimer > uiFps)
        {
            glfwPollEvents();

            if (glfwGetWindowAttrib(window, GLFW_ICONIFIED) != 0) {
                // ImGui_ImplGlfw_Sleep(10);
                return;
            }
            if (glfwWindowShouldClose(window)) {
                is_running = false;
                return;
            }
            newFrame();

            m_debug->update();
            uiTimer = 0;
            renderFrame();
        }
        dt = deltaTime(startTime);
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
    window = glfwCreateWindow(1280, 720, "RogEm v0.0.1", nullptr, nullptr);
    if (!window)
        return -1;
    glfwMakeContextCurrent(window);
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
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init(glsl_version);
    return 0;
}
