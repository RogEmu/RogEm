#include "Application.hpp"

#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

#include <spdlog/spdlog.h>
#include <argparse/argparse.hpp>

#include "Core/GPU.hpp"

const char *glsl_version = "#version 330";

Application::Application() :
    m_debugger(&m_system)
{
    m_system.init();
    m_system.attachDebugger(&m_debugger);
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

int Application::run()
{
    if (initGlfw()) {
        return -1;
    }
    if (initImgui()) {
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
    m_debugger.update();
}

void Application::render()
{
    imguiNewFrame();
    ImGui::DockSpaceOverViewport(0, ImGui::GetMainViewport());
    m_debugger.draw();
    drawScreen();
    imguiRenderFrame();
    glfwSwapBuffers(glfwGetCurrentContext());
}

void Application::drawScreen()
{
    if (ImGui::Begin("Screen")) {
        GPU *gpu = static_cast<GPU *>(m_system.getBus()->getDevice(PsxDeviceType::GPU));
        const uint8_t *vram = gpu->getVram();
        glBindTexture(GL_TEXTURE_2D, m_vramTexture);
        glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, 1024, 512, GL_RGBA, GL_UNSIGNED_SHORT_1_5_5_5_REV, vram);
        ImGui::Image((ImTextureID)(intptr_t)m_vramTexture, ImGui::GetContentRegionAvail());
        ImGui::End();
    }
}
