#include "LogWindow.hpp"

#include <cstdint>
#include "imgui.h"

LogWindow::LogWindow(Debugger *debugger) :
    m_debugger(debugger)
{
    setTitle("Logs");
}

LogWindow::~LogWindow()
{
}

void LogWindow::addLog(const std::string &log)
{
    m_logs.push_back(log);
}

void LogWindow::update()
{
    if (ImGui::Begin("Logs"))
    {
        drawTopBar();
        ImGui::Separator();
        drawLogsWindow();
    }
    ImGui::End();
}

void LogWindow::drawTopBar()
{
    ImGui::BeginGroup();

    if (ImGui::Button("Clear"))
    {
        m_logs.clear();
    }
    ImGui::SameLine();
    if (ImGui::Button("Scroll to bottom"))
    {
        m_autoScroll = true;
    }
    ImGui::EndGroup();
}

void LogWindow::drawLogsWindow()
{
    if (ImGui::BeginChild("Logs text", ImVec2(0.0f, 0.0f), ImGuiChildFlags_None, ImGuiWindowFlags_HorizontalScrollbar))
    {
        ImGuiListClipper clipper;
        clipper.Begin(static_cast<int>(m_logs.size()));
        while (clipper.Step())
        {
            for (int lineNbr = clipper.DisplayStart; lineNbr < clipper.DisplayEnd; ++lineNbr)
            {
                 ImGui::TextUnformatted(m_logs.at(lineNbr).c_str());
            }
        }
        if ((ImGui::GetScrollY() >= ImGui::GetScrollMaxY()) || (m_autoScroll))
        {
            if (m_autoScroll)
                m_autoScroll = false;
            ImGui::SetScrollHereY(1.0f);
        }
    }
    ImGui::EndChild();
}
