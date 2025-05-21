/*
** EPITECH PROJECT, 2024
** RogEm
** File description:
** BIOS
*/

#ifndef BIOS_H_
#define BIOS_H_

#include "Memory.hpp"

#include <string>

class BIOS : public Memory
{
    public:
        BIOS();
        BIOS(const std::string &path);

        bool loadFromFile(const std::string &path);
};

#endif /* !BIOS_H_ */
