#include "MainMenuBar.hpp"
#include "Debugger/Debugger.hpp"
#include "Application.hpp"
#include "GUI/IWindow.hpp"
#include <fmt/format.h>

#include <iostream>
#include <memory>
#include <algorithm>

MainMenuBar::MainMenuBar(Application *application) :
    m_application(application)
{
    try {
        m_currentPath = std::filesystem::current_path();
        refreshFileList();
    } catch (const std::exception &e) {
        m_currentPath = std::filesystem::path(".");
        std::cerr << "Error getting current path: " << e.what() << std::endl;
    }
}

MainMenuBar::~MainMenuBar()
{
}

void MainMenuBar::draw()
{
    if (ImGui::BeginMainMenuBar()) {
        drawFileMenu();
        drawEmulationMenu();
        drawDebugMenu();
        drawWindowsMenu();
        ImGui::EndMainMenuBar();
    }
    if (m_showFileDialog) {
        drawFileDialog();
    }
}

void MainMenuBar::drawFileMenu()
{
    if (ImGui::BeginMenu("File")) {
        if (ImGui::MenuItem("Load ROM...", "Ctrl+O")) {
            m_isLoadingBios = false;
            m_showFileDialog = true;
            m_filenameBuffer[0] = '\0';
        }
        if (ImGui::MenuItem("Load BIOS...", "Ctrl+B")) {
            m_isLoadingBios = true;
            m_showFileDialog = true;
            m_filenameBuffer[0] = '\0';
        }
        if (ImGui::MenuItem("Remove Executable", "Ctrl+Shift+R")) {
            m_application->getSystem().setExecutablePath("");
        }
        ImGui::EndMenu();
    }
}

void MainMenuBar::drawEmulationMenu()
{
    if (ImGui::BeginMenu("Emulation")) {
        bool paused = m_application->getSystem().getState() == SystemState::PAUSED;
        if (ImGui::MenuItem(paused ? "Resume" : "Pause", "F5", &paused)) {
            m_application->getSystem().setState(paused ? SystemState::PAUSED : SystemState::RUNNING);
        }
        if (ImGui::MenuItem("Reset", "Ctrl+R")) {
            m_application->getSystem().reset();
        }
        ImGui::EndMenu();
    }
}

void MainMenuBar::drawDebugMenu()
{
    if (ImGui::BeginMenu("Debug")) {
        ImGui::Separator();

        if (ImGui::BeginMenu("Breakpoints")) {
            auto &breakpoints = m_application->getDebugger().getBreakpoints();
            if (breakpoints.empty()) {
                ImGui::MenuItem("No breakpoints", nullptr, false, false);
            } else {
                for (auto &bp : breakpoints) {
                    bool enabled = bp.enabled;
                    char label[128];
                    snprintf(label, sizeof(label), "0x%08X - %s###bp%08X", bp.addr, bp.label.c_str(), bp.addr);
                    if (ImGui::MenuItem(label, nullptr, &enabled)) {
                        long index = m_application->getDebugger().getBreakpointIndex(bp.addr);
                        if (index != -1) {
                            m_application->getDebugger().toggleBreakpoint(index, enabled);
                        }
                    }
                }
                ImGui::Separator();
                if (ImGui::MenuItem("Clear All")) {
                    int breakpointCount = static_cast<int>(breakpoints.size());
                    for (int i = breakpointCount - 1; i >= 0; i--) {
                        m_application->getDebugger().removeBreakpoint(i);
                    }
                }
            }
            ImGui::EndMenu();
        }
        ImGui::EndMenu();
    }
}

void MainMenuBar::drawWindowsMenu()
{
    if (ImGui::BeginMenu("Windows")) {
        auto &windows = m_application->getWindows();
        for (auto &window : windows) {
            const char* title = window->getTitleChar();
            bool isVisible = window->isVisible();
            if (ImGui::MenuItem(title, nullptr, &isVisible)) {
                window->setVisible(isVisible);
            }
        }
        ImGui::Separator();
        if (ImGui::MenuItem("Show All")) {
            for (auto &window : windows) {
                window->setVisible(true);
            }
        }
        if (ImGui::MenuItem("Hide All")) {
            for (auto &window : windows) {
                window->setVisible(false);
            }
        }
        ImGui::EndMenu();
    }
}

void MainMenuBar::drawFileDialog()
{
    bool shouldNavigate = false;
    std::filesystem::path pathToNavigate;
    const char* title = m_isLoadingBios ? "Load BIOS File" : "Load ROM File";

    ImGui::SetNextWindowSize(ImVec2(700, 500), ImGuiCond_FirstUseEver);
    if (ImGui::Begin(title, &m_showFileDialog, ImGuiWindowFlags_NoCollapse)) {
        ImGui::Text("Current Path: %s", m_currentPath.string().c_str());
        ImGui::Separator();
        if (ImGui::Button("..") && m_currentPath.has_parent_path()) {
            navigateToDirectory(m_currentPath.parent_path());
        }
        ImGui::SameLine();
        ImGui::Text("(Parent Directory)");

        ImGui::Separator();
        ImGui::BeginChild("FileList", ImVec2(0, -70), true);
        for (const auto &entry : m_directoryContents) {
            bool isDirectory = entry.is_directory();
            std::string filename = entry.path().filename().string();

            if (isDirectory) {
                ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 1.0f, 0.0f, 1.0f)); // Directories color. Set as yellow but can be changed
                if (ImGui::Selectable(("[DIR] " + filename).c_str(), false, ImGuiSelectableFlags_AllowDoubleClick)) {
                    if (ImGui::IsMouseDoubleClicked(0)) {
                        pathToNavigate = entry.path();
                        shouldNavigate = true;
                    }
                }
                ImGui::PopStyleColor();
            } else {
                std::string ext = entry.path().extension().string();
                std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);
                bool validFile = false;
                if (m_isLoadingBios) {
                    validFile = (ext == ".bin" || ext == ".bios" || ext == ".rom");
                } else {
                    validFile = (ext == ".bin" || ext == ".exe" || ext == ".ps-exe" || ext == ".psx" || ext == ".rom");
                }
                if (validFile) {
                    if (ImGui::Selectable(("[FILE] " + filename).c_str(), false, ImGuiSelectableFlags_AllowDoubleClick)) {
                        std::string pathStr = fmt::format("{}", entry.path().string());
                        pathStr.copy(m_filenameBuffer, sizeof(m_filenameBuffer) - 1);
                        m_filenameBuffer[std::min(pathStr.size(), sizeof(m_filenameBuffer) - 1)] = '\0';
                        if (ImGui::IsMouseDoubleClicked(0)) {
                            m_application->getSystem().reset();
                            if (m_isLoadingBios) {
                                m_application->getSystem().loadBios(m_filenameBuffer);
                            } else {
                                m_application->getSystem().setExecutablePath(m_filenameBuffer);
                            }
                            ImGui::CloseCurrentPopup();
                            m_showFileDialog = false;
                        }
                    }
                } else {
                    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.5f, 0.5f, 0.5f, 1.0f)); // Non-matching files color. Set as gray but can be changed
                    ImGui::Selectable(("[FILE] " + filename).c_str(), false, ImGuiSelectableFlags_Disabled);
                    ImGui::PopStyleColor();
                }
            }
        }
        ImGui::EndChild();
        if (shouldNavigate) {
            navigateToDirectory(pathToNavigate);
        }
        ImGui::Separator();
        ImGui::Text("Selected File:");
        ImGui::InputText("##filepath", m_filenameBuffer, sizeof(m_filenameBuffer));
        if (ImGui::Button("Load", ImVec2(100, 0))) {
            if (m_filenameBuffer[0] != '\0') {
                std::string filePath(m_filenameBuffer);
                m_application->getSystem().reset();
                if (m_isLoadingBios) {
                    m_application->getSystem().loadBios(m_filenameBuffer);
                } else {
                    m_application->getSystem().setExecutablePath(m_filenameBuffer);
                }
                m_showFileDialog = false;
            }
        }
        ImGui::SameLine();
        if (ImGui::Button("Cancel", ImVec2(100, 0))) {
            m_showFileDialog = false;
        }
    }
    ImGui::End();
}

void MainMenuBar::refreshFileList()
{
    m_directoryContents.clear();
    try {
        for (const auto &entry : std::filesystem::directory_iterator(m_currentPath)) {
            m_directoryContents.push_back(entry);
        }
        // Sort: directories first, then files, alphabetically
        std::sort(m_directoryContents.begin(), m_directoryContents.end(),
            [](const std::filesystem::directory_entry &a, const std::filesystem::directory_entry &b) {
                if (a.is_directory() != b.is_directory())
                    return a.is_directory();
                return a.path().filename().string() < b.path().filename().string();
            });
    } catch (const std::exception &e) {
        std::cerr << "Error getting current path: " << e.what() << std::endl;
    }
}

void MainMenuBar::navigateToDirectory(const std::filesystem::path &path)
{
    try {
        m_currentPath = std::filesystem::canonical(path);
        refreshFileList();
    } catch (const std::exception &e) {
        std::cerr << "Error navigating to directory: " << e.what() << std::endl;
    }
}
