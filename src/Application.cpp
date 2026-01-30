#include "Application.hpp"

#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

#include <spdlog/spdlog.h>
#include <argparse/argparse.hpp>

#include "Core/GPU.hpp"
#include "Core/InterruptController.hpp"
#include "Core/DigitalPad.hpp"
#include "GUI/RegisterWindow.hpp"
#include "GUI/AssemblyWindow.hpp"
#include "GUI/BreakpointWindow.hpp"
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

static PadButton mapKeyToPadButton(int key)
{
    switch (key) {
        case GLFW_KEY_UP: return PadButton::PAD_JOYUP;
        case GLFW_KEY_DOWN: return PadButton::PAD_JOYDOWN;
        case GLFW_KEY_LEFT: return PadButton::PAD_JOYLEFT;
        case GLFW_KEY_RIGHT: return PadButton::PAD_JOYRIGHT;
        case GLFW_KEY_S: return PadButton::PAD_CROSS;
        case GLFW_KEY_D: return PadButton::PAD_CIRCLE;
        case GLFW_KEY_A: return PadButton::PAD_SQUARE;
        case GLFW_KEY_W: return PadButton::PAD_TRIANGLE;
        case GLFW_KEY_Q: return PadButton::PAD_L1;
        case GLFW_KEY_E: return PadButton::PAD_R1;
        case GLFW_KEY_1: return PadButton::PAD_L2;
        case GLFW_KEY_3: return PadButton::PAD_R2;
        case GLFW_KEY_ENTER: return PadButton::PAD_START;
        case GLFW_KEY_F: return PadButton::PAD_SELECT;
        default: return PadButton::PAD_UNKOWN;
    }
}

static void appKeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    (void)scancode;
    (void)mods;
    System* system = static_cast<System*>(glfwGetWindowUserPointer(window));
    if (!system) {
        return;
    }
    PadButton padButton = mapKeyToPadButton(key);
    if (padButton == PadButton::PAD_UNKOWN) {
        return;
    }
    static uint16_t buttonsPort = 0xFFFF;
    if (action == GLFW_PRESS) {
        buttonsPort &= ~static_cast<uint16_t>(padButton);
    } else if (action == GLFW_RELEASE) {
        buttonsPort |= static_cast<uint16_t>(padButton);
    }
    system->updatePadInputs(buttonsPort);
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
    glfwSetKeyCallback(m_window, appKeyCallback);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        spdlog::error("Failed to initialize GLAD");
        return -1;
    }
    glfwSetWindowUserPointer(m_window, &m_system);
    return 0;
}

int Application::initImgui()
{
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;           // Enable Docking
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;       // Enable Keyboard Controls
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
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 1024, 512, 0, GL_RGBA, GL_UNSIGNED_SHORT_1_5_5_5_REV, 0);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
}

void Application::initWindows()
{
    m_windows.emplace_back(std::make_unique<RegisterWindow>(&m_debugger));
    m_windows.emplace_back(std::make_unique<AssemblyWindow>(this, &m_debugger));
    m_windows.emplace_back(std::make_unique<BreakpointWindow>(&m_debugger));
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

void Application::setFullscreen(bool fullscreen)
{
    if (fullscreen == m_fullscreen)
        return;
    m_fullscreen = fullscreen;
    if (fullscreen) {
        glfwGetWindowPos(m_window, &m_windowedX, &m_windowedY);
        glfwGetWindowSize(m_window, &m_windowedWidth, &m_windowedHeight);
        GLFWmonitor* monitor = glfwGetPrimaryMonitor();
        const GLFWvidmode* mode = glfwGetVideoMode(monitor);
        glfwSetWindowMonitor(m_window, monitor, 0, 0, mode->width, mode->height, mode->refreshRate);
    } else {
        glfwSetWindowMonitor(m_window, nullptr, m_windowedX, m_windowedY, m_windowedWidth, m_windowedHeight, 0);
    }
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
    static bool f11WasPressed = false;
    bool f11Pressed = glfwGetKey(m_window, GLFW_KEY_F11) == GLFW_PRESS;
    if (f11Pressed && !f11WasPressed)
        setFullscreen(!m_fullscreen);
    f11WasPressed = f11Pressed;
    pollGamepad();
    m_system.update();
}

void Application::pollGamepad()
{
    uint16_t gamepadButtons = 0xFFFF;
    if (!glfwJoystickPresent(GLFW_JOYSTICK_1) || !glfwJoystickIsGamepad(GLFW_JOYSTICK_1))
        return;
    GLFWgamepadstate state;
    if (!glfwGetGamepadState(GLFW_JOYSTICK_1, &state))
        return;

    struct { int glfwButton; PadButton padButton; } buttonMap[] = {
        { GLFW_GAMEPAD_BUTTON_A,            PadButton::PAD_CROSS },
        { GLFW_GAMEPAD_BUTTON_B,            PadButton::PAD_CIRCLE },
        { GLFW_GAMEPAD_BUTTON_X,            PadButton::PAD_SQUARE },
        { GLFW_GAMEPAD_BUTTON_Y,            PadButton::PAD_TRIANGLE },
        { GLFW_GAMEPAD_BUTTON_LEFT_BUMPER,  PadButton::PAD_L1 },
        { GLFW_GAMEPAD_BUTTON_RIGHT_BUMPER, PadButton::PAD_R1 },
        { GLFW_GAMEPAD_BUTTON_BACK,         PadButton::PAD_SELECT },
        { GLFW_GAMEPAD_BUTTON_START,        PadButton::PAD_START },
        { GLFW_GAMEPAD_BUTTON_DPAD_UP,      PadButton::PAD_JOYUP },
        { GLFW_GAMEPAD_BUTTON_DPAD_DOWN,    PadButton::PAD_JOYDOWN },
        { GLFW_GAMEPAD_BUTTON_DPAD_LEFT,    PadButton::PAD_JOYLEFT },
        { GLFW_GAMEPAD_BUTTON_DPAD_RIGHT,   PadButton::PAD_JOYRIGHT },
    };

    for (const auto& mapping : buttonMap) {
        if (state.buttons[mapping.glfwButton] == GLFW_PRESS) {
            gamepadButtons &= ~static_cast<uint16_t>(mapping.padButton);
        }
    }

    if (state.axes[GLFW_GAMEPAD_AXIS_LEFT_TRIGGER] > -0.5f)
        gamepadButtons &= ~static_cast<uint16_t>(PadButton::PAD_L2);
    if (state.axes[GLFW_GAMEPAD_AXIS_RIGHT_TRIGGER] > -0.5f)
        gamepadButtons &= ~static_cast<uint16_t>(PadButton::PAD_R2);

    m_system.updatePadInputs(gamepadButtons);
}

void Application::render()
{
    imguiNewFrame();
    if (!m_fullscreen) {
        ImGui::DockSpaceOverViewport(0, ImGui::GetMainViewport());
        m_mainMenuBar->draw();
        for (auto &window : m_windows) {
            if (window->isVisible()) {
                window->update();
            }
        }
    }
    drawScreen();
    imguiRenderFrame();
    glfwSwapBuffers(glfwGetCurrentContext());
}

void Application::drawScreen()
{
    GPU *gpu = m_system.getBus()->getDevice<GPU>();
    uint8_t *vram = gpu->getVram();

    glBindTexture(GL_TEXTURE_2D, m_vramTexture);
    glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, 1024, 512, GL_RGBA, GL_UNSIGNED_SHORT_1_5_5_5_REV, vram);

    ImVec2 uv0(0.0f, 0.0f);
    ImVec2 uv1(1.0f, 1.0f);
    if (m_showDisplayArea) {
        auto displayArea = gpu->getDisplayArea();
        int width = 0;
        if (gpu->getGpuStat().hRes2) {
            width = 368;
        } else {
            switch (gpu->getHorizontalRes()) {
                case HorizontalRes::RES_256: width = 256; break;
                case HorizontalRes::RES_320: width = 320; break;
                case HorizontalRes::RES_512: width = 512; break;
                case HorizontalRes::RES_640: width = 640; break;
            }
        }
        int height = (gpu->getVerticalRes() == VerticalRes::RES_240) ? 240 : 480;
        uv0.x = displayArea.halfwordAddress / 1024.0f;
        uv0.y = displayArea.scanlineAddress / 512.0f;
        uv1.x = (displayArea.halfwordAddress + width) / 1024.0f;
        uv1.y = (displayArea.scanlineAddress + height) / 512.0f;
    }

    if (m_fullscreen) {
        const ImGuiViewport* viewport = ImGui::GetMainViewport();
        ImGui::SetNextWindowPos(viewport->Pos);
        ImGui::SetNextWindowSize(viewport->Size);
        ImGui::Begin("##FullscreenScreen", nullptr,
            ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoMove
            | ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoDocking);
        ImGui::Image((ImTextureID)(intptr_t)m_vramTexture, ImGui::GetContentRegionAvail(), uv0, uv1);
        ImGui::End();
    } else {
        if (ImGui::Begin("Screen")) {
            ImGui::Checkbox("Display Area", &m_showDisplayArea);
            ImGui::Image((ImTextureID)(intptr_t)m_vramTexture, ImGui::GetContentRegionAvail(), uv0, uv1);
        }
        ImGui::End();
    }

    if (!m_fullscreen) {
        m_vramEditor.DrawWindow("VRAM", vram, GPU_VRAM_1MB_SIZE);

        if (ImGui::Begin("GPU Debug")) {
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
            ImGui::Text("Texture Page Base: (%d, %d)",
                        gpuStat.texPageBase.x * 64, gpuStat.texPageBase.y * 256);
            ImGui::Text("Texture page colors: %s", gpuStat.texPageColors == TexturePageColors::COL_4Bit ? "4bit" :
                                                gpuStat.texPageColors == TexturePageColors::COL_8Bit ? "8bit" :
                                                gpuStat.texPageColors == TexturePageColors::COL_15Bit ? "15bit" : "Reserved");
        }
        ImGui::End();

        if (ImGui::Begin("IRQ Controller")) {
            auto irqc = m_system.getBus()->getDevice<InterruptController>();
            ImGui::Text("ISTAT: 0x%08X", irqc->read32(0x1F801070));
            ImGui::Text("IMASK: 0x%08X", irqc->read32(0x1F801074));
        }
        ImGui::End();
    }
}
