/*
** EPITECH PROJECT, 2024
** RogEm
** File description:
** main
*/

#include <iostream>
#include <thread>

#include <argparse/argparse.hpp>
#include <spdlog/spdlog.h>

#include "System.hpp"

int main(int ac, char **av)
{
    spdlog::set_level(spdlog::level::debug);
    argparse::ArgumentParser args("RogEm");

    args.add_description("A PSX emulator written in C++ with love");
    args.add_argument("bios").help("The BIOS file to boot the console with").required();
    args.add_argument("exe").help("a PSX-EXE executable file to run after the BIOS boots").default_value("");

    try
    {
        args.parse_args(ac, av);
    }
    catch(const std::exception& e)
    {
        spdlog::error("{}", e.what());
        std::cout << args;
        return 1;
    }

    EmulatorConfig config;
    config.biosFilePath = args.get("bios");
    config.exeFilePath = args.get("exe");

    System system;

    if (system.init(config)) {
        return 1;
    }
    system.run();
    return 0;
}
