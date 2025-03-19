/*
** EPITECH PROJECT, 2025
** rogem
** File description:
** HBoxLayout
*/

#include "HBoxLayout.hpp"

HBoxLayout::HBoxLayout() :
    Widget(0, 0, 0, 0)
{
}

HBoxLayout::~HBoxLayout()
{
}

void HBoxLayout::addWidget(const std::shared_ptr<Widget> &widget)
{
    m_widgets.push_back(widget);
    widget->setParent(std::make_shared<Widget>(*this));
    resizeInternalWidgets();
}

void HBoxLayout::draw(WINDOW *window)
{
    for (auto &widget : m_widgets)
    {
        widget->draw(window);
    }
    // drawBorder(window);
}

void HBoxLayout::resize(int w, int h)
{
    Widget::resize(w, h);
    resizeInternalWidgets();
}

void HBoxLayout::resizeInternalWidgets()
{
    int sizePerWidget = ((COLS - m_pos.x) / static_cast<float>(m_widgets.size()));
    int currentWidgetPos = 0;
    int currentWidgetWidth = sizePerWidget;

    for (auto &widget : m_widgets)
    {
        widget->resize(currentWidgetWidth, m_size.y);
        widget->move(currentWidgetPos, widget->position().y);
        currentWidgetPos += currentWidgetWidth + 1;

        if (currentWidgetPos + currentWidgetWidth + 1 > COLS)
        {
            currentWidgetWidth = COLS - currentWidgetPos - 1;
        }
    }
}
