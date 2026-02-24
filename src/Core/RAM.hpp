/*
** EPITECH PROJECT, 2024
** rogem
** File description:
** RAM
*/

#ifndef RAM_HPP_
#define RAM_HPP_

#include "Memory.hpp"

class StateBuffer;

class RAM : public Memory
{
    public:
        RAM(Bus *bus);

        void reset() override;

        void serialize(StateBuffer &buf) const override;
        void deserialize(StateBuffer &buf) override;
        void loadExecutable(uint32_t baseAddr, const std::vector<uint8_t> &code);
};

#endif /* !RAM_HPP_ */
