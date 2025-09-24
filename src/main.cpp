#include <spdlog/spdlog.h>

#include "Application.hpp"

int main(int ac, char **av)
{
    spdlog::set_level(spdlog::level::debug);
    Application app;

    if (app.loadConfig(ac, av)) {
        return 1;
    }
    return app.run();
}
