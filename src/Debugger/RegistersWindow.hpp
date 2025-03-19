/*
** EPITECH PROJECT, 2025
** rogem
** File description:
** RegistersWindow
*/

#ifndef REGISTERSWINDOW_HPP_
#define REGISTERSWINDOW_HPP_

#include "Window.hpp"

class RegistersWindow : public Window
{
    public:
        RegistersWindow(int x, int y, int w, int h);
        ~RegistersWindow();

        void setGPR(uint32_t *const gpr);
        void setSpecialRegisters(uint32_t *const pc, uint32_t *const hi, uint32_t *const lo);

        void draw(WINDOW *window) override;

    private:
        uint32_t *m_gpr;
        uint32_t *m_pc;
        uint32_t *m_hi;
        uint32_t *m_lo;
};

#endif /* !REGISTERSWINDOW_HPP_ */
