/*
** EPITECH PROJECT, 2024
** RogEm
** File description:
** BIOS
*/

#ifndef BIOS_HPP_
#define BIOS_HPP_

#include "Memory.hpp"

#include <string>

#include "Bus.hpp"

class BIOS : public Memory
{
    public:
        BIOS(Bus *bus);
        BIOS(Bus *bus, const std::string &path);

        bool loadFromFile(const std::string &path);
};

#endif /* !BIOS_HPP_ */
