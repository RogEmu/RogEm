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

#include <map>

class Bus;

class InstructionsWindow : public Window
{
    public:
        InstructionsWindow(int x, int y, int w, int h);
        ~InstructionsWindow();

        void setPc(uint32_t *const pc);
        void addInstruction(uint32_t pc, const Instruction &i);
        void setBus(Bus *bus);

    protected:
        void drawElements() const override;

    private:
        uint32_t *m_pc;
        Bus *m_bus;
        std::map<uint32_t, Instruction> m_instructionMap;
};

#endif /* !INSTRUCTIONSWINDOW_HPP_ */
