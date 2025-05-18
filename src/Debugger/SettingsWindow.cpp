#include "SettingsWindow.hpp"
#include "Debugger.hpp"
#include "CPU.h"

SettingsWindow::SettingsWindow(Debugger *debugger) : m_debugger(debugger)
{}

SettingsWindow::~SettingsWindow()
{}

void SettingsWindow::resetButton()
{
    if (ImGui::Button("Reset System")){
        m_debugger->CPUReset();
    }
}

void SettingsWindow::update()
{
    if (ImGui::Begin("Settings")) {
        resetButton();
    }
    ImGui::End();
}
