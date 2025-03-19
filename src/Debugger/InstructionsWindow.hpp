/*
** EPITECH PROJECT, 2025
** rogem
** File description:
** InstructionsWindow
*/

#ifndef INSTRUCTIONSWINDOW_HPP_
#define INSTRUCTIONSWINDOW_HPP_

#include "Window.hpp"
#include "Instruction.h"

#include <memory>
#include <map>

class Bus;

class InstructionsWindow : public Window
{
    public:
        InstructionsWindow(int x, int y, int w, int h);
        ~InstructionsWindow();

        void setPc(uint32_t *const pc);
        void setBus(const std::shared_ptr<Bus> &bus);

        void draw(WINDOW *window) override;

    private:
        uint32_t *m_pc;
        std::shared_ptr<Bus> m_bus;
};

#endif /* !INSTRUCTIONSWINDOW_HPP_ */
