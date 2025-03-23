/*
** EPITECH PROJECT, 2024
** RogEm
** File description:
** main
*/

#include <iostream>
#include <thread>
#include <chrono>

#include "BIOS.h"
#include "CPU.h"
#include "RAM.h"
#include "Debugger.hpp"

float deltaTime(std::chrono::steady_clock::time_point &lastTime)
{
    auto now = std::chrono::steady_clock::now();
    float deltaTime = std::chrono::duration<double>(now - lastTime).count();
    lastTime = now;

    return deltaTime;
}

int main(int ac, char **av)
{
    if (ac != 2)
    {
        std::cout << "Missing BIOS filepath" << std::endl;
        return -1;
    }
    auto bios = std::make_shared<BIOS>(av[1]);
    auto ram = std::make_shared<RAM>();
    auto bus = std::make_shared<Bus>(bios, ram);
    auto cpu = std::make_shared<CPU>(bus);
    auto dbg = std::make_shared<Debugger>(cpu);

    auto startTime = std::chrono::steady_clock::now();
    float uiFps = 1/60.0f;
    float uiTimer = 0.0f;
    float dt = 0;
    float simluationTimer = 0.0f;

    while (dbg->isRunning())
    {
        if (!dbg->isPaused())
        {
            if (simluationTimer > (dbg->getSimSpeed()))
            {
                cpu->step();
                simluationTimer = 0;
            }
        }

        if (uiTimer > uiFps)
        {
            dbg->update();
            uiTimer = 0;
        }
        dt = deltaTime(startTime);
        uiTimer += dt;
        simluationTimer += dt;
    }
    return 0;
}
