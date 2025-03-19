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
    if (m_widgets.empty())
        return;

    int heightPerWidget = m_size.y / static_cast<float>(m_widgets.size());
    int currentWidgetPos = 0;
    int currentWidgetHeight = heightPerWidget;

    for (auto &widget : m_widgets)
    {
        widget->resize(m_size.x, currentWidgetHeight);
        widget->move(m_pos.x, currentWidgetPos + m_pos.y);
        currentWidgetPos += currentWidgetHeight + 1;

        if (currentWidgetPos + currentWidgetHeight > m_size.y)
        {
            currentWidgetHeight = m_size.y - currentWidgetPos;
        }
    }
}
