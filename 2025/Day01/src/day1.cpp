#include "../include/aocutils.h"

int main(int argc, char const *argv[])
{
    try
    {
        const int32_t MAX_DIAL = 100;
        const std::string input_path{argc <= 1 ? "../input.txt" : argv[1]};
        const auto input = aoc::parseFileAsStringVector(input_path);
        int32_t zeros{};
        int32_t rotations{};

        const auto result = std::accumulate(input.begin(), input.end(), 50, [&](const int32_t dial, std::string_view instruction)
                                            {
                                                const auto direction = instruction[0];
                                                int32_t rotation{};
                                                std::from_chars(instruction.begin() + 1, instruction.end(), rotation);

                                                const bool left_rotation = direction == 'L';
                                                const int32_t position = (left_rotation ? dial - rotation: dial + rotation);
                                                const int32_t remainder = position % MAX_DIAL;
                                                const int32_t state = (remainder < 0 ? (MAX_DIAL + remainder) : remainder);

                                                if (state == 0) ++zeros;

                                                rotations += std::abs(position / MAX_DIAL);
                                                if (position <= 0 && dial != 0) ++rotations;

                                                return state; });

        std::cout << std::format("part 1: {}\n", zeros);
        std::cout << std::format("part 2: {}\n", rotations);
    }
    catch (const std::runtime_error &e)
    {
        std::cerr << std::format("{}\n", e.what());
    }

    return EXIT_SUCCESS;
}
