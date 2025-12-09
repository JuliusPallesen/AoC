#include "../include/aocutils.h"

int main(int argc, char const *argv[])
{
    try
    {
        const std::string input_path{argc <= 1 ? "../../input.txt" : argv[1]};
        const auto input = aoc::parseFileAsStringVector(input_path);
        aoc::printVector(input);
    }
    catch (const std::runtime_error &e)
    {
        std::cerr << std::format("{}\n", e.what());
    }

    std::cout << std::format("{}\n", argv[0]);

    return EXIT_SUCCESS;
}

