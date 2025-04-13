/*
** EPITECH PROJECT, 2024
** rogem
** File description:
** RAM
*/

#ifndef RAM_H_
#define RAM_H_

#include "Memory.hpp"

constexpr uint32_t RAM_SIZE = (2048 * 1024);

class RAM : public Memory
{
    public:
        RAM();
};

#endif /* !RAM_H_ */
