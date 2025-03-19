/*
** EPITECH PROJECT, 2025
** rogem
** File description:
** ABoxLayout
*/

#include "ABoxLayout.hpp"

ABoxLayout::ABoxLayout() :
    Widget(0, 0, COLS, LINES)
{
}

ABoxLayout::~ABoxLayout()
{
}

void ABoxLayout::addWidget(const std::shared_ptr<Widget> &widget)
{
    m_widgets.push_back(widget);
    widget->setParent(this);
    resizeInternalWidgets();
}

void ABoxLayout::draw(WINDOW *window)
{
    for (auto &widget : m_widgets)
    {
        widget->draw(window);
    }
}

void ABoxLayout::resize(int w, int h)
{
    Widget::resize(w, h);
    resizeInternalWidgets();
}
