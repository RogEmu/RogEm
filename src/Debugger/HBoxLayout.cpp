/*
** EPITECH PROJECT, 2025
** rogem
** File description:
** HBoxLayout
*/

#include "HBoxLayout.hpp"

HBoxLayout::HBoxLayout() :
    ABoxLayout()
{
}

HBoxLayout::~HBoxLayout()
{
}

void HBoxLayout::resizeInternalWidgets()
{
    if (m_widgets.empty())
        return;

    int sizePerWidget = m_size.x / static_cast<float>(m_widgets.size());
    int currentWidgetPos = 0;
    int currentWidgetWidth = sizePerWidget;

    for (auto &widget : m_widgets)
    {
        widget->resize(currentWidgetWidth, m_size.y);
        widget->move(currentWidgetPos + m_pos.x, m_pos.y);
        currentWidgetPos += currentWidgetWidth + 1;

        if (currentWidgetPos + currentWidgetWidth > m_size.x)
        {
            currentWidgetWidth = m_size.x - currentWidgetPos;
        }
    }
}
