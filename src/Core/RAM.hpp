/*
** EPITECH PROJECT, 2024
** rogem
** File description:
** RAM
*/

#ifndef RAM_H_
#define RAM_H_

#include "Memory.hpp"

class RAM : public Memory
{
    public:
        RAM(Bus *bus);

        void loadExecutable(uint32_t baseAddr, const std::vector<uint8_t> &code);
};

#endif /* !RAM_H_ */
