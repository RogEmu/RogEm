/*
** EPITECH PROJECT, 2025
** rogem
** File description:
** ScratchPad
*/

#ifndef SCRATCHPAD_HPP_
#define SCRATCHPAD_HPP_

#include "Memory.hpp"

class StateBuffer;

class ScratchPad : public Memory
{
    public:
        ScratchPad(Bus *bus);
        ~ScratchPad();

        void serialize(StateBuffer &buf) const override;
        void deserialize(StateBuffer &buf) override;
};

#endif /* !SCRATCHPAD_HPP_ */
