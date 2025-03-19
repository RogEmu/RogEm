/*
** EPITECH PROJECT, 2025
** rogem
** File description:
** VBoxLayout
*/

#include "VBoxLayout.hpp"

VBoxLayout::VBoxLayout() :
    ABoxLayout()
{
}

VBoxLayout::~VBoxLayout()
{
}

void VBoxLayout::resizeInternalWidgets()
{
    int heightPerWidget = ((LINES - m_pos.y) / static_cast<float>(m_widgets.size()));
    int currentWidgetPos = 0;
    int currentWidgetHeight = heightPerWidget;

    for (auto &widget : m_widgets)
    {
        widget->resize(m_size.x, currentWidgetHeight);
        widget->move(widget->position().x, currentWidgetPos);
        currentWidgetPos += currentWidgetHeight + 1;

        if (currentWidgetPos + currentWidgetHeight > LINES)
        {
            currentWidgetHeight = LINES - currentWidgetPos;
        }
    }
}
