#include <iostream>
#include <set>
#include <fstream>
#include <format>
#include <ranges>
#include <algorithm>

using Pos = std::pair<int, int>;

Pos getDelta(const char c)
{
    switch (c)
    {
    case 'U':
        return {0, -1};
    case 'D':
        return {0, 1};
    case 'L':
        return {-1, 0};
    case 'R':
        return {1, 0};
    default:
        throw(std::runtime_error(std::format("Unsupported direction {}\n", c)));
    }
}

int main(int argc, char const *argv[])
{
    std::fstream inputFile(argv[1]);
    std::string line;

    Pos p{0, 0};
    while (std::getline(inputFile, line))
    {
        const Pos d = getDelta(line[0]);
        const int count = std::stoi(line.substr(2));
        for (size_t i = 0; i < count; i++)
        {
            p = {p.first + d.first, p.second + d.second};
        }
        


        std::cout << count << "\n";
    }
    return 0;
}
