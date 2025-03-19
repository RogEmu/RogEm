/*
** EPITECH PROJECT, 2025
** rogem
** File description:
** VBoxLayout
*/

#ifndef VBOXLAYOUT_HPP_
#define VBOXLAYOUT_HPP_

#include "ABoxLayout.hpp"

class VBoxLayout : public ABoxLayout
{
    public:
        VBoxLayout();
        ~VBoxLayout();

    protected:
        void resizeInternalWidgets() override;
};

#endif /* !VBOXLAYOUT_HPP_ */
