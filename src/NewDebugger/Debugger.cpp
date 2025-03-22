#define GLFW_INCLUDE_NONE

#include <iostream>

#include <GL/gl.h>
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

#include "Debugger.hpp"
#include "CPU.h"
#include "Disassembler.h"

Debugger::Debugger(const std::shared_ptr<CPU> &cpu) :
    m_cpu(cpu)
{
    if (!glfwInit()) {
        std::cerr << "Failed to initialize GLFW" << std::endl;
        return;
    }
    // Create a window
    window = glfwCreateWindow(800, 600, "Hello, GLFW!", nullptr, nullptr);
    if (!window) {
        std::cerr << "Failed to create window" << std::endl;
        glfwTerminate();
        return;
    }
    glfwMakeContextCurrent(window);
    registerNames = Disassembler::getRegisterNames();

    ImGui::CreateContext();
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 330");
}

Debugger::~Debugger()
{
    glfwTerminate();
}

void Debugger::registerTable()
{
    ImGui::Begin("Registers");
    ImGui::BeginTable("Registers", 2);
    ImGui::TableSetupColumn("Register");
    ImGui::TableSetupColumn("Value");
    ImGui::TableHeadersRow();
    for (int i = 0; i < NB_GPR; i++) {
        ImGui::TableNextRow();
        ImGui::TableNextColumn();
        ImGui::Text("%s", registerNames[i].c_str());
        ImGui::TableNextColumn();
        ImGui::Text("%08x", m_cpu->m_registers[i]);
    }
    ImGui::TableNextRow();
    ImGui::TableNextColumn();
    ImGui::Text("PC");
    ImGui::TableNextColumn();
    ImGui::Text("%08x", m_cpu->m_pc);
    ImGui::TableNextRow();
    ImGui::TableNextColumn();
    ImGui::Text("HI");
    ImGui::TableNextColumn();
    ImGui::Text("%08x", m_cpu->m_hi);
    ImGui::TableNextRow();
    ImGui::TableNextColumn();
    ImGui::Text("LO");
    ImGui::TableNextColumn();
    ImGui::Text("%08x", m_cpu->m_lo);

    ImGui::EndTable();
    ImGui::End();
}

void Debugger::MemoryTable()
{
    ImGui::Begin("Memory");
    if (ImGui::BeginTable("Memory", 18, ImGuiTableFlags_SizingFixedFit)) {
        ImGui::TableSetupColumn("Address");
        ImGui::TableSetupColumn("00");
        ImGui::TableSetupColumn("01");
        ImGui::TableSetupColumn("02");
        ImGui::TableSetupColumn("03");
        ImGui::TableSetupColumn("04");
        ImGui::TableSetupColumn("05");
        ImGui::TableSetupColumn("06");
        ImGui::TableSetupColumn("07");
        ImGui::TableSetupColumn("08");
        ImGui::TableSetupColumn("09");
        ImGui::TableSetupColumn("0A");
        ImGui::TableSetupColumn("0B");
        ImGui::TableSetupColumn("0C");
        ImGui::TableSetupColumn("0D");
        ImGui::TableSetupColumn("0E");
        ImGui::TableSetupColumn("0F");
        ImGui::TableSetupColumn("ASCII");
        ImGui::TableHeadersRow();

        for (int i = 0; i < 100; i++) {
            ImGui::TableNextRow();
            ImGui::TableNextColumn();
            ImGui::Text("%08x", i * 16);
            for (int j = 0; j < 16; j++) {
                ImGui::TableNextColumn();
                ImGui::Text("%02x", m_cpu->m_bus->loadByte(i * 16 + j));
            }
            ImGui::TableNextColumn();
            // for (int j = 0; j < 16; j++) {
            //     char c = m_cpu->m_bus->loadByte(i * 16 + j);
            //     if (c < 32 || c > 126) c = '.';

            // }
            // ImGui::Text("%c", c);
        }

        ImGui::EndTable();
    }
    ImGui::End();
}

void Debugger::InstructionTable()
{
    ImGui::Begin("Instructions");

    for (int i = 0; i < 100; i++) {
        ImGui::Text("%s", Disassembler::disassemble(m_cpu->m_pc + i * 4, static_cast<Instruction>(m_cpu->m_bus->loadWord(m_cpu->m_pc + i * 4))).c_str());
    }

    ImGui::End();
}

void Debugger::update()
{
    // Make the OpenGL context current
    glfwMakeContextCurrent(window);

    if (!glfwWindowShouldClose(window)) {
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        registerTable();
        MemoryTable();
        InstructionTable();

        ImGui::ShowDemoWindow();
        ImGui::Render();

        glClear(GL_COLOR_BUFFER_BIT);
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        glfwSwapBuffers(window);

        glfwPollEvents();
    }
    else {
        ImGui_ImplOpenGL3_Shutdown();
        ImGui_ImplGlfw_Shutdown();
        ImGui::DestroyContext();
        glfwDestroyWindow(window);
    }

    return;
}