/*
** EPITECH PROJECT, 2025
** rogem
** File description:
** MemoryWindow
*/

#ifndef MEMORYWINDOW_HPP_
#define MEMORYWINDOW_HPP_

#include "Window.hpp"

class Bus;

class MemoryWindow : public Window
{
    public:
        MemoryWindow();
        ~MemoryWindow();

        void setBus(const std::shared_ptr<Bus> &bus);

        void draw(WINDOW *window) override;

        void addressUp();
        void addressDown();

    private:
        void drawMemoryLine(WINDOW *window, int x, int y, uint32_t addr);

    private:
        uint32_t m_startAddress;
        std::shared_ptr<Bus> m_bus;
};

#endif /* !MEMORYWINDOW_HPP_ */
