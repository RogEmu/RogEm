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

    private:
        std::string m_title;
};

#endif /* !IWINDOW_HPP_ */
