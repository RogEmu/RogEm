/*
** EPITECH PROJECT, 2025
** rogem
** File description:
** ABoxLayout
*/

#ifndef ABOXLAYOUT_HPP_
#define ABOXLAYOUT_HPP_

#include "Widget.hpp"

class ABoxLayout : public Widget
{
    public:
        ABoxLayout();
        ~ABoxLayout();

        void addWidget(const std::shared_ptr<Widget> &widget);

        void draw(WINDOW *window) override;

        void resize(int w, int h) override;

    protected:
        virtual void resizeInternalWidgets() = 0;

    protected:
        std::list<std::shared_ptr<Widget>> m_widgets;
};

#endif /* !ABOXLAYOUT_HPP_ */
