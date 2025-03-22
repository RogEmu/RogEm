#define GLFW_INCLUDE_NONE

#include <iostream>

#include <GL/gl.h>
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

#include "Debugger.hpp"

Debugger::Debugger(const std::shared_ptr<CPU> &cpu)
{
    m_cpu = cpu;
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

    ImGui::CreateContext();
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 330");
}

Debugger::~Debugger()
{
    glfwTerminate();
}

void Debugger::update()
{

    // Make the OpenGL context current
    glfwMakeContextCurrent(window);

    if (!glfwWindowShouldClose(window)) {
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
        // Draw text
        ImGui::Begin("Hello, ImGui!");
        ImGui::Text("Hello, World!");
        ImGui::End();
        ImGui::Render();

        glClear(GL_COLOR_BUFFER_BIT);
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        // Clear the screen

        // Swap buffers
        glfwSwapBuffers(window);

        // Poll events
        glfwPollEvents();
    }
    else {
        // Cleanup
        ImGui_ImplOpenGL3_Shutdown();
        ImGui_ImplGlfw_Shutdown();
        ImGui::DestroyContext();
        glfwDestroyWindow(window);
    }

    return;
}