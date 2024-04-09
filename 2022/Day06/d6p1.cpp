#include <string>
#include <fstream>
#include <algorithm>
#include <array>
#include <iostream>
#include <ranges>

using Arr4 = std::array<char, 4>;

bool hasOnlyUniqueElements(const Arr4 &arr)
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
    int ans = 0;
    while (std::getline(inputFile, line))
    {
        Arr4 last;
        const auto pos = std::find_if(line.begin(), line.end(), [i = 0, &last](const char c) mutable
                                      { 
                                        last[i++ % last.size()] = c;
                                        return hasOnlyUniqueElements(last) && i >= 4; });
        ans = (pos != line.end()) ? pos - line.begin() + 1 : 0;
    }
    std::cout << ans;
    return EXIT_SUCCESS;
}
