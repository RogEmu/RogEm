/*
** EPITECH PROJECT, 2025
** rogem
** File description:
** PsxExecutable
*/

#ifndef PSXEXECUTABLE_HPP_
#define PSXEXECUTABLE_HPP_

#include <string>
#include <cstdint>
#include <vector>

struct PsxExecutable
{
    PsxExecutable(const std::string &exeFilePath);

    bool load();

    std::string path;
    uint32_t initialPc; // Initial Program Counter address
    uint32_t initialGp; // Initial Global Pointer address
    uint32_t ramDestination; // Ram address to which the program will be copied
    uint32_t exeSize; // Executable file size in multiples of 2kB excluding header size (2kB)
    uint32_t initialSpBase; // Initial Stack Pointer and Frame Pointer base address
    uint32_t initialSpOffset; // Initial Stack Pointer and Frame Pointer address
    std::vector<uint8_t> exeData; // Executable code

    private:
        int readExeHeader(std::ifstream &exeFile);
};

#endif /* !PSXEXECUTABLE_HPP_ */
