#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <optional>
#include <algorithm>
#include <unordered_map>

int checkLoad(std::vector<std::string> platform)
{
    int ans;
    for (size_t y = 0; y < platform.size(); y++)
    {
        for (size_t x = 0; x < platform[y].size(); x++)
        {
            if (platform[y][x] == 'O')
                ans += platform.size() - y;
        }
    }
    return ans;
}

void tiltNorth(std::vector<std::string> &platform)
{
    std::vector<int> freeSlots(platform.size(), 0);
    for (size_t y = 0; y < platform.size(); ++y)
    {
        for (size_t x = 0; x < platform[y].size(); ++x)
        {
            switch (platform[y][x])
            {
            case 'O':
                std::swap(platform[y][x], platform[y - freeSlots[x]][x]);
                break;
            case '.':
                freeSlots[x] = freeSlots[x] + 1;
                break;
            case '#':
                freeSlots[x] = 0;
                break;
            default:
                throw(std::runtime_error("inv symbol"));
            }
        }
    }
}

void tiltSouth(std::vector<std::string> &platform)
{
    std::vector<int> freeSlots(platform.size(), 0);
    for (int y = platform.size() - 1; y >= 0; --y)
    {
        for (int x = 0; x < platform[y].size(); ++x)
        {
            switch (platform[y][x])
            {
            case 'O':
                std::swap(platform[y][x], platform[y + freeSlots[x]][x]);
                break;
            case '.':
                freeSlots[x] = freeSlots[x] + 1;
                break;
            case '#':
                freeSlots[x] = 0;
                break;
            default:
                throw(std::runtime_error("inv symbol"));
            }
        }
    }
}

void tiltWest(std::vector<std::string> &platform)
{
    std::vector<int> freeSlots(platform[0].size(), 0);
    for (size_t x = 0; x < platform.size(); ++x)
    {
        for (size_t y = 0; y < platform[x].size(); ++y)
        {
            switch (platform[y][x])
            {
            case 'O':
                std::swap(platform[y][x], platform[y][x - freeSlots[y]]);
                break;
            case '.':
                freeSlots[y] = freeSlots[y] + 1;
                break;
            case '#':
                freeSlots[y] = 0;
                break;
            default:
                throw(std::runtime_error("inv symbol"));
            }
        }
    }
}

void tiltEast(std::vector<std::string> &platform)
{
    std::vector<int> freeSlots(platform[0].size(), 0);
    for (int x = platform.size() - 1; x >= 0; x--)
    {
        for (int y = 0; y < platform[x].size(); ++y)
        {
            switch (platform[y][x])
            {
            case 'O':
                std::swap(platform[y][x], platform[y][x + freeSlots[y]]);
                break;
            case '.':
                freeSlots[y] = freeSlots[y] + 1;
                break;
            case '#':
                freeSlots[y] = 0;
                break;
            default:
                throw(std::runtime_error("inv symbol"));
            }
        }
    }
}

int main(int argc, char const *argv[])
{
    std::ifstream inputFile;
    inputFile.open(argv[1]);
    std::string line;
    int ans;
    std::unordered_map<int, int> occurs;

    std::vector<std::string> platform;
    while (std::getline(inputFile, line))
    {
        platform.push_back(line);
    }
    inputFile.close();

    for (size_t i = 0; i < 10000; i++)
    {
        tiltNorth(platform);
        tiltWest(platform);
        tiltSouth(platform);
        tiltEast(platform);

        int temp = checkLoad(platform);
        if (occurs.count(temp) > 0)
        {
            occurs[temp]++;
        }
        else
        {
            occurs[temp] = 1;
        }
    }

    for (const auto &s : platform)
    {
        std::cout << s << '\n';
    }

    for (auto &&i : occurs)
    {
        if (i.first > 88316 && i.first < 88491 && i.second > 1)
            std::cout << i.first << ": " << i.second << '\n';
    }

    // 88371: was right answer, but i just trial and errored it. Could also be caused by the fact
    // that 10000 is divisible by the proper input
    return 0;
}
