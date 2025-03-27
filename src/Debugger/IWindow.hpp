#ifndef IWINDOW_HPP_
#define IWINDOW_HPP_

class IWindow
{
    public:
        virtual ~IWindow() = default;

        virtual void update() = 0;
};

#endif /* !IWINDOW_HPP_ */
