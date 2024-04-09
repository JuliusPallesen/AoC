#include <string>
#include <fstream>
#include <algorithm>
#include <array>
#include <iostream>
#include <ranges>

using Arr14 = std::array<char, 14>;

bool hasOnlyUniqueElements(const Arr14 &arr)
{
    for (auto &&c : arr)
        if (std::ranges::count(arr, c) >= 2)
            return false;
    return true;
}

int main(int argc, char const *argv[])
{
    std::fstream inputFile(argv[1]);
    std::string line;
    std::getline(inputFile, line);
    Arr14 last;

    const auto pos = std::find_if(line.begin(), line.end(), [i = 0, &last](const char c) mutable
                                  { last[i++ % last.size()] = c;
                                    return hasOnlyUniqueElements(last) && i >= 4; });
    std::cout << pos - line.begin() + 1;
    return EXIT_SUCCESS;
}
