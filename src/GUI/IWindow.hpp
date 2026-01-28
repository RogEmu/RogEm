#ifndef IWINDOW_HPP_
#define IWINDOW_HPP_

#include <string>

class IWindow
{
    public:
        virtual ~IWindow() = default;

        virtual void update() = 0;
        void setTitle(const char *title) { m_title = title; }
        void setTitle(const std::string &title) { m_title = title; }
        std::string getTitleString() const { return m_title; }
        const char* getTitleChar() const { return m_title.c_str(); }

        void setVisible(bool visible) { m_isVisible = visible; }
        bool isVisible() const { return m_isVisible; }
        void toggleVisibility() { m_isVisible = !m_isVisible; }

    private:
        std::string m_title;
        bool m_isVisible = true;  // Windows are visible by default
};

#endif /* !IWINDOW_HPP_ */
