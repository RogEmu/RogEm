#include "Application.hpp"

#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

#include <spdlog/spdlog.h>
#include <argparse/argparse.hpp>

#include "Core/GPU.hpp"
#include "GUI/RegisterWindow.hpp"
#include "GUI/AssemblyWindow.hpp"
#include "GUI/BreakpointWindow.hpp"
#include "GUI/SettingsWindow.hpp"
#include "GUI/LogWindow.hpp"
#include "GUI/MemoryWindow.hpp"
#include "GUI/MainMenuBar.hpp"

const char *glsl_version = "#version 330";

Application::Application() :
    m_debugger(&m_system)
{
    m_system.init();
    m_system.setDebuggerCallback([this]() { m_debugger.update(); });

    initWindows();
}

Application::~Application()
{
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
    glfwDestroyWindow(m_window);
    glfwTerminate();
}

static void glfw_error_callback(int error_code, const char* description)
{
    spdlog::error("GLFW Error {}: {}", error_code, description);
}

int Application::initGlfw()
{
    glfwSetErrorCallback(glfw_error_callback);
    if (!glfwInit())
        return -1;

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // Create a window
    m_window = glfwCreateWindow(1280, 720, "RogEm", nullptr, nullptr);
    if (!m_window) {
        return -1;
    }
    glfwMakeContextCurrent(m_window);
    glfwSwapInterval(0); // VSync Off

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        spdlog::error("Failed to initialize GLAD");
        return -1;
    }
    return 0;
}

int Application::initImgui()
{
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

void Application::initVramTexture()
{
    glGenTextures(1, &m_vramTexture);
    glBindTexture(GL_TEXTURE_2D, m_vramTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 1024, 512, 0, GL_RGBA, GL_UNSIGNED_SHORT_1_5_5_5_REV, 0);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
}

void Application::initWindows()
{
    m_windows.emplace_back(std::make_unique<RegisterWindow>(&m_debugger));
    m_windows.emplace_back(std::make_unique<AssemblyWindow>(this, &m_debugger));
    m_windows.emplace_back(std::make_unique<BreakpointWindow>(&m_debugger));
    m_windows.emplace_back(std::make_unique<SettingsWindow>(&m_debugger));
    auto logWindow = std::make_unique<LogWindow>(&m_debugger);
    m_system.setTtyCallback([window = logWindow.get()](const std::string &log) {
        window->addLog(log);
    });
    m_windows.emplace_back(std::move(logWindow));
    auto biosMemoryWindow = std::make_unique<MemoryWindow>(&m_debugger);
    biosMemoryWindow->setBaseAddr(0xBFC00000);
    biosMemoryWindow->setTitle("BIOS");
    biosMemoryWindow->setReadOnly(true);
    m_windows.push_back(std::move(biosMemoryWindow));

    auto ramMemoryWindow = std::make_unique<MemoryWindow>(&m_debugger);
    ramMemoryWindow->setBaseAddr(0);
    ramMemoryWindow->setTitle("RAM");
    m_windows.push_back(std::move(ramMemoryWindow));

    m_mainMenuBar = std::make_unique<MainMenuBar>(this);
}

int Application::run()
{
    if (initGlfw() || initImgui()) {
        return -1;
    }
    initVramTexture();

    m_isRunning = true;
    m_system.loadBios(m_config.biosFilePath.c_str());
    m_system.setExecutablePath(m_config.exeFilePath);

    m_debugger.pause(false);
    while (m_isRunning) {
        glfwPollEvents();
        update();
        render();
    }
    return 0;
}

void Application::quit()
{
    m_isRunning = false;
}

int Application::loadConfig(int ac, char **av)
{
    argparse::ArgumentParser args("RogEm");

    args.add_description("A PSX emulator written in C++ with love");
    args.add_argument("bios").help("The BIOS file to boot the console with").required();
    args.add_argument("exe").help("a PSX-EXE executable file to run after the BIOS boots").default_value("");

    try {
        args.parse_args(ac, av);
    } catch(const std::exception& e) {
        spdlog::error("{}", e.what());
        std::cout << args;
        return 1;
    }
    m_config.biosFilePath = args.get("bios");
    m_config.exeFilePath = args.get("exe");
    return 0;
}

static void imguiNewFrame()
{
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
}

static void imguiRenderFrame()
{
    ImGui::Render();
    int display_w, display_h;
    glfwGetFramebufferSize(glfwGetCurrentContext(), &display_w, &display_h);
    glViewport(0, 0, display_w, display_h);
    glClear(GL_COLOR_BUFFER_BIT);
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void Application::update()
{
    if (glfwGetWindowAttrib(m_window, GLFW_ICONIFIED) != 0) {
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
    if (glfwWindowShouldClose(m_window)) {
        m_isRunning = false;
    }
    m_system.update();
}

void Application::render()
{
    imguiNewFrame();
    ImGui::DockSpaceOverViewport(0, ImGui::GetMainViewport());
    m_mainMenuBar->draw();
    for (auto &window : m_windows) {
        if (window->isVisible()) {
            window->update();
        }
    }
    drawScreen();
    imguiRenderFrame();
    glfwSwapBuffers(glfwGetCurrentContext());
}

void Application::drawScreen()
{
    if (ImGui::Begin("Screen")) {
        GPU *gpu = m_system.getBus()->getDevice<GPU>();
        const uint8_t *vram = gpu->getVram();
        glBindTexture(GL_TEXTURE_2D, m_vramTexture);
        glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, 1024, 512, GL_RGBA, GL_UNSIGNED_SHORT_1_5_5_5_REV, vram);
        ImGui::Image((ImTextureID)(intptr_t)m_vramTexture, ImGui::GetContentRegionAvail());
    }
    ImGui::End();

    if (ImGui::Begin("GPU Debug")) {
        auto gpu = m_system.getBus()->getDevice<GPU>();
        auto gpuStat = gpu->getGpuStat();
        auto drawArea = gpu->getDrawArea();
        auto drawOffset = gpu->getDrawOffset();
        auto hres = gpu->getHorizontalRes();
        auto vres = gpu->getVerticalRes();
        ImGui::Text("GPUStat: 0x%08X", gpu->getGpuStatRaw());
        ImGui::Text("Vertical Interlace: %s", gpuStat.vInterlace ? "Yes" : "No");
        ImGui::Text("Video Mode: %s",  gpuStat.videoMode == VideoMode::NTSC ? "NTSC" : "PAL");
        ImGui::Text("Resolution: %s x %s",
                    hres == HorizontalRes::RES_256 ? "256" :
                    hres == HorizontalRes::RES_320 ? "320" :
                    hres == HorizontalRes::RES_512 ? "512" : "640",
                    vres == VerticalRes::RES_240 ? "240" : "480");
        ImGui::Text("Draw Area: (%d, %d) - (%d, %d)",
                    drawArea.topLeft.x, drawArea.topLeft.y,
                    drawArea.botRight.x, drawArea.botRight.y);
        ImGui::Text("Draw Offset: (%d, %d)",
                    drawOffset.x, drawOffset.y);
    }
    ImGui::End();
}
