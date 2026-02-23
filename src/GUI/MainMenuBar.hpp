#ifndef MAINMENUBAR_HPP_
#define MAINMENUBAR_HPP_

#include "imgui.h"
#include <string>
#include <vector>
#include <filesystem>

class Debugger;
class Application;

enum class FileDialogMode {
    LoadROM,
    LoadBIOS,
    SaveState,
    LoadState
};

class MainMenuBar
{
    public:
        MainMenuBar(Application *application);
        ~MainMenuBar();

        void draw();

    private:
        void drawFileMenu();
        void drawEmulationMenu();
        void drawDebugMenu();
        void drawWindowsMenu();

        // File dialog functions
        void drawFileDialog();
        void refreshFileList();
        void navigateToDirectory(const std::filesystem::path &path);

    private:
        Application *m_application;

        // File dialog state
        bool m_showFileDialog = false;
        FileDialogMode m_fileDialogMode = FileDialogMode::LoadROM;
        std::filesystem::path m_currentPath;
        std::vector<std::filesystem::directory_entry> m_directoryContents;
        char m_filenameBuffer[256] = {0};
};

#endif /* !MAINMENUBAR_HPP_ */
