#include <string>
#include <fstream>
#include <algorithm>
#include <array>
#include <iostream>
#include <ranges>

using Arr14 = std::array<char, 14>;

int main(int argc, char const *argv[])
{
    std::fstream inputFile(argv[1]);
    std::string input;
    std::getline(inputFile, input);
    Arr14 last;

    const auto onlyUnique = [&last]() -> bool
    {
        for (auto &&c : last)
            if (std::ranges::count(last, c) >= 2)
                return false;
        return true;
    };

    const auto pos = std::find_if(input.begin(), input.end(), [i = 0, &last, &onlyUnique](const char c) mutable
                                  { last[i++ % last.size()] = c;
                                    return onlyUnique() && i >= 4; });
    std::cout << pos - input.begin() + 1;
    return EXIT_SUCCESS;
}
