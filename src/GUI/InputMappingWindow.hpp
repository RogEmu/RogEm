#ifndef INPUTMAPPINGWINDOW_HPP_
#define INPUTMAPPINGWINDOW_HPP_

#include "GUI/IWindow.hpp"
#include "Core/InputManager.hpp"
#include "Core/DigitalPad.hpp"
#include <vector>
#include <string>
#include <unordered_map>
#include <optional>

class InputMappingWindow : public IWindow
{
public:
    InputMappingWindow(InputManager* manager);
    ~InputMappingWindow() override = default;

    void update() override;

    bool wantsKeyboardCapture() const { return m_waitingForKey.has_value(); }
    void onGlfwKeyPressed(int glfwKey);

private:
    InputManager* m_inputManager;
    std::vector<PadButton> m_buttonsList;

    const char* padButtonToString(PadButton button);
    std::string getKeyNameForButton(PadButton button);

    std::unordered_map<int, PadButton> m_editMapping;
    std::unordered_map<int, PadButton> m_originalMapping;

    std::optional<PadButton> m_waitingForKey;
};

#endif /* !INPUTMAPPINGWINDOW_HPP_ */
