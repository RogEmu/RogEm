/*
** EPITECH PROJECT, 2025
** rogem
** File description:
** HBoxLayout
*/

#ifndef HBOXLAYOUT_HPP_
#define HBOXLAYOUT_HPP_

#include "ABoxLayout.hpp"

class HBoxLayout : public ABoxLayout
{
    public:
        HBoxLayout();
        ~HBoxLayout();

    protected:
        virtual void resizeInternalWidgets() override;
};

#endif /* !HBOXLAYOUT_HPP_ */
