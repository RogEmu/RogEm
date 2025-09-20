
#include "System.hpp"

#ifdef _WIN32
    #include <Windows.h>
#endif //_WIN32

#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

#include <iostream>
#include <memory>
#include <string>
#include <fstream>
#include <spdlog/spdlog.h>
#include <chrono>
#include <thread>

#include "core/PsxExecutable.hpp"
#include "core/GPU.hpp"
#include "core/InterruptController.hpp"
#include "core/SIO.hpp"
#include "core/PadDigital.hpp"

static uint16_t buildPadMaskFromKeyboard(GLFWwindow* win);
static GamepadState readGamepadAsAnalog();

System::System()
{
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

int System::init(const EmulatorConfig &config)
{
    if (initGFLW() || initImGUi())
        return 1;

    m_emuConfig = config;
    m_bus = std::make_unique<Bus>();
    m_cpu = std::make_unique<CPU>(m_bus.get());
    m_debug = std::make_unique<Debugger>(this);

    if (m_bus) {
        BIOS *bios = static_cast<BIOS *>(m_bus->getDevice(PsxDeviceType::BIOS));
        if (!bios->loadFromFile(config.biosFilePath)) {
            return 1;
        }
    }
    m_bus->connectCpu(m_cpu.get());
    initVramTexture();
    return 0;
}

void System::run()
{
    int cpuFreq = 33868800;
    float cyclesPerFrame = cpuFreq / 60.0f;
    float currentCycles = 0;

    m_isRunning = true;
    m_debug->pause(false);
    while (m_isRunning)
    {
        update();

        // >>> Input → Port 1
        if (auto* sio = static_cast<SIO*>(m_bus->getDevice(PsxDeviceType::SIO0))) {
            GamepadState gp = readGamepadAsAnalog();
            if (gp.present) {
                // Prefer gamepad → Analog controller
                sio->ensurePadType(0, SioPadType::Analog);
                sio->setControllerAnalog(0, gp.buttonsMask, gp.lx, gp.ly, gp.rx, gp.ry);
            } else {
                // No gamepad → Digital controller driven by keyboard
                sio->ensurePadType(0, SioPadType::Digital);
                uint16_t kbMask = buildPadMaskFromKeyboard(m_window);
                sio->setControllerButtons(0, kbMask);
            }
        }

        if (currentCycles >= cyclesPerFrame) {
            render();
            currentCycles = 0;
            auto irqc = static_cast<InterruptController*>(m_bus->getDevice(PsxDeviceType::IRQController));
            irqc->triggerIRQ(DeviceIRQ::VBLANK);
        }
        currentCycles += 2;
    }
}

void glfw_error_callback(int error_code, const char* description)
{
    spdlog::error("GLFW Error {}: {}", error_code, description);
}

int System::initGFLW()
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
    // glfwSwapInterval(1); // VSync

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        spdlog::error("Failed to initialize GLAD");
        return -1;
    }
    return 0;
}

int System::initImGUi()
{
    const char *glsl_version = "#version 330";

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

void System::update()
{
    if (!m_debug->isPaused())
    {
        if (m_cpu->getReg(CpuReg::PC) == 0x80030000 && !m_emuConfig.exeFilePath.empty()) {
            loadPsxExe(m_emuConfig.exeFilePath.c_str());
        }
        m_cpu->step();
    }
    m_debug->update();
}

static void newImGuiFrame()
{
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
}

static void renderImGuiFrame()
{
    ImGui::Render();
    int display_w, display_h;
    glfwGetFramebufferSize(glfwGetCurrentContext(), &display_w, &display_h);
    glViewport(0, 0, display_w, display_h);
    glClear(GL_COLOR_BUFFER_BIT);
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    glfwSwapBuffers(glfwGetCurrentContext());
}

static inline bool key_down(GLFWwindow* win, int key) {
    return glfwGetKey(win, key) == GLFW_PRESS;
}

// PSX buttons are active-LOW: 0 = pressed.
static uint16_t buildPadMaskFromKeyboard(GLFWwindow* win) {
    uint16_t m = 0xFFFF;

    // D-Pad: Z,Q,S,D => UP,LEFT,DOWN,RIGHT  (AZERTY)
    if (key_down(win, GLFW_KEY_Z)) m &= ~PadDigital::Up;
    if (key_down(win, GLFW_KEY_Q)) m &= ~PadDigital::Left;
    if (key_down(win, GLFW_KEY_S)) m &= ~PadDigital::Down;
    if (key_down(win, GLFW_KEY_D)) m &= ~PadDigital::Right;

    // Face: O,K,L,M => TRIANGLE,SQUARE,CROSS,CIRCLE
    if (key_down(win, GLFW_KEY_O)) m &= ~PadDigital::Triangle;
    if (key_down(win, GLFW_KEY_K)) m &= ~PadDigital::Square;
    if (key_down(win, GLFW_KEY_L)) m &= ~PadDigital::Cross;
    if (key_down(win, GLFW_KEY_M)) m &= ~PadDigital::Circle;

    // Shoulders: A,E,I,P => L2,L1,R1,R2
    if (key_down(win, GLFW_KEY_A)) m &= ~PadDigital::L2;
    if (key_down(win, GLFW_KEY_E)) m &= ~PadDigital::L1;
    if (key_down(win, GLFW_KEY_I)) m &= ~PadDigital::R1;
    if (key_down(win, GLFW_KEY_P)) m &= ~PadDigital::R2;

    // System: F,J => SELECT,START
    if (key_down(win, GLFW_KEY_F)) m &= ~PadDigital::Select;
    if (key_down(win, GLFW_KEY_J)) m &= ~PadDigital::Start;

    return m;
}

static inline uint8_t to_u8_axis(float a, float dead = 0.12f) {
    // GLFW axes in [-1..1], center 0. Apply deadzone, map to [0..255]
    if (a > -dead && a < dead) a = 0.0f;
    float v = (a + 1.0f) * 0.5f * 255.0f; // -1 -> 0, 0 -> 127.5, 1 -> 255
    if (v < 0.0f) v = 0.0f; else if (v > 255.0f) v = 255.0f;
    return static_cast<uint8_t>(v + 0.5f);
}

static GamepadState readGamepadAsAnalog() {
    GamepadState g;
    if (!glfwJoystickIsGamepad(GLFW_JOYSTICK_1)) return g;

    GLFWgamepadstate st{};
    if (!glfwGetGamepadState(GLFW_JOYSTICK_1, &st)) return g;
    g.present = true;

    auto downB = [&](int b){ return st.buttons[b] == GLFW_PRESS; };
    auto axis  = [&](int a){ return st.axes[a]; };

    // Face: Y,X,A,B => TRIANGLE,SQUARE,CROSS,CIRCLE
    if (downB(GLFW_GAMEPAD_BUTTON_Y)) g.buttonsMask &= ~PadDigital::Triangle;
    if (downB(GLFW_GAMEPAD_BUTTON_X)) g.buttonsMask &= ~PadDigital::Square;
    if (downB(GLFW_GAMEPAD_BUTTON_A)) g.buttonsMask &= ~PadDigital::Cross;
    if (downB(GLFW_GAMEPAD_BUTTON_B)) g.buttonsMask &= ~PadDigital::Circle;

    // D-pad
    if (downB(GLFW_GAMEPAD_BUTTON_DPAD_UP))    g.buttonsMask &= ~PadDigital::Up;
    if (downB(GLFW_GAMEPAD_BUTTON_DPAD_LEFT))  g.buttonsMask &= ~PadDigital::Left;
    if (downB(GLFW_GAMEPAD_BUTTON_DPAD_DOWN))  g.buttonsMask &= ~PadDigital::Down;
    if (downB(GLFW_GAMEPAD_BUTTON_DPAD_RIGHT)) g.buttonsMask &= ~PadDigital::Right;

    // Shoulders / triggers
    if (downB(GLFW_GAMEPAD_BUTTON_LEFT_BUMPER))  g.buttonsMask &= ~PadDigital::L1;
    if (downB(GLFW_GAMEPAD_BUTTON_RIGHT_BUMPER)) g.buttonsMask &= ~PadDigital::R1;
    if (axis(GLFW_GAMEPAD_AXIS_LEFT_TRIGGER)  > 0.5f) g.buttonsMask &= ~PadDigital::L2;
    if (axis(GLFW_GAMEPAD_AXIS_RIGHT_TRIGGER) > 0.5f) g.buttonsMask &= ~PadDigital::R2;

    // Start / Back → Start / Select
    if (downB(GLFW_GAMEPAD_BUTTON_START)) g.buttonsMask &= ~PadDigital::Start;
    if (downB(GLFW_GAMEPAD_BUTTON_BACK))  g.buttonsMask &= ~PadDigital::Select;

    // Sticks
    g.lx = to_u8_axis(axis(GLFW_GAMEPAD_AXIS_LEFT_X));
    g.ly = to_u8_axis(-axis(GLFW_GAMEPAD_AXIS_LEFT_Y));  // Y up is negative
    g.rx = to_u8_axis(axis(GLFW_GAMEPAD_AXIS_RIGHT_X));
    g.ry = to_u8_axis(-axis(GLFW_GAMEPAD_AXIS_RIGHT_Y));

    return g;
}

void System::render()
{
    glfwPollEvents();

    if (glfwGetWindowAttrib(m_window, GLFW_ICONIFIED) != 0) {
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
    if (glfwWindowShouldClose(m_window))
    {
        m_isRunning = false;
    }
    newImGuiFrame();
    ImGui::DockSpaceOverViewport(0, ImGui::GetMainViewport());
    m_debug->draw();
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, {0, 0});
    if (ImGui::Begin("Screen")) {
        GPU *gpu = static_cast<GPU *>(m_bus->getDevice(PsxDeviceType::GPU));
        const uint8_t *vram = gpu->getVram();
        glBindTexture(GL_TEXTURE_2D, m_vramTexture);
        glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, 1024, 512, GL_RGBA, GL_UNSIGNED_SHORT_1_5_5_5_REV, vram);
        ImGui::Image((ImTextureID)(intptr_t)m_vramTexture, ImGui::GetContentRegionAvail());
    }
    ImGui::PopStyleVar();
    ImGui::End();

    renderImGuiFrame();
}

void System::loadPsxExe(const char *path)
{
    PsxExecutable exe(path);

    auto ram = static_cast<RAM *>(m_bus->getDevice(PsxDeviceType::RAM));

    if (exe.load()) {
        m_cpu->setReg(CpuReg::PC, exe.initialPc);
        m_cpu->setReg(CpuReg::GP, exe.initialGp);
        m_cpu->setReg(CpuReg::SP, exe.initialSpBase);
        m_cpu->setReg(CpuReg::FP, exe.initialSpBase);
        ram->loadExecutable(exe.ramDestination, exe.exeData);
        spdlog::info("Loaded PSX-EXE file successfuly");
    } else {
        spdlog::error("Error while loading PSX-EXE file");
    }
}

void System::initVramTexture()
{
    glGenTextures(1, &m_vramTexture);
    glBindTexture(GL_TEXTURE_2D, m_vramTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 1024, 512, 0, GL_RGBA, GL_UNSIGNED_SHORT_5_5_5_1, 0);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
}
