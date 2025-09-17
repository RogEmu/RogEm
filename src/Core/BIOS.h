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

#include "Bus.h"

class BIOS : public Memory
{
    public:
        BIOS(Bus *bus);
        BIOS(Bus *bus, const std::string &path);

        bool loadFromFile(const std::string &path);
};

#endif /* !BIOS_H_ */
