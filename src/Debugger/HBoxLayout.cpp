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
    int sizePerWidget = ((COLS - m_pos.x) / static_cast<float>(m_widgets.size()));
    int currentWidgetPos = 0;
    int currentWidgetWidth = sizePerWidget;

    for (auto &widget : m_widgets)
    {
        widget->resize(currentWidgetWidth, m_size.y);
        widget->move(currentWidgetPos, widget->position().y);
        currentWidgetPos += currentWidgetWidth + 2;

        if (currentWidgetPos + currentWidgetWidth > COLS)
        {
            currentWidgetWidth = COLS - currentWidgetPos;
        }
    }
}
