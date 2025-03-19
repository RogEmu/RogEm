/*
** EPITECH PROJECT, 2025
** rogem
** File description:
** HBoxLayout
*/

#ifndef HBoxLayout_HPP_
#define HBoxLayout_HPP_

#include "Widget.hpp"

class HBoxLayout : public Widget
{
    public:
        HBoxLayout();
        ~HBoxLayout();

        void addWidget(const std::shared_ptr<Widget> &widget);

        void draw(WINDOW *window) override;

        void resize(int w, int h) override;

    private:
        void resizeInternalWidgets();

    private:
        std::list<std::shared_ptr<Widget>> m_widgets;
};

#endif /* !HBoxLayout_HPP_ */
