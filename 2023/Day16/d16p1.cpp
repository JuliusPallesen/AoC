#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <algorithm>
#include <optional>
#include <numeric>

using Cavern = std::vector<std::string>;
using Illumination = std::vector<std::vector<int>>;

enum class Directions
{
    Down = 0,
    Left = 1,
    Up = 2,
    Right = 3
};

std::vector<Directions> tryGetDir(const char tile, const Directions dir)
{
    switch (dir)
    {
    case Directions::Down:
        switch (tile)
        {
        case '-':
            return {Directions::Left, Directions::Right};
        case '/':
            return {Directions::Left};
        case '\\':
            return {Directions::Right};
        default:
            return {dir};
        }
        break;
    case Directions::Left:
        switch (tile)
        {
        case '/':
            return {Directions::Down};
        case '\\':
            return {Directions::Up};
        case '|':
            return {Directions::Up, Directions::Down};
        default:
            return {dir};
        }
        break;
    case Directions::Up:
        switch (tile)
        {
        case '-':
            return {Directions::Left, Directions::Right};
        case '/':
            return {Directions::Right};
        case '\\':
            return {Directions::Left};
        default:
            return {dir};
        }
        break;
    case Directions::Right:
        switch (tile)
        {
        case '/':
            return {Directions::Up};
        case '\\':
            return {Directions::Down};
        case '|':
            return {Directions::Up, Directions::Down};
        default:
            return {dir};
        }
        break;
    }
    return {};
}

std::pair<size_t, size_t> getNewPosition(const std::pair<size_t, size_t> &p, Directions dir)
{
    switch (dir)
    {
    case Directions::Up:
        return {p.first - 1, p.second};
    case Directions::Left:
        return {p.first, p.second - 1};
    case Directions::Right:
        return {p.first, p.second + 1};
    case Directions::Down:
        return {p.first + 1, p.second};
    }
    return p;
}

bool isInBounds(const Cavern &c, const std::pair<size_t, size_t> &p)
{
    return p.first < c.size() && p.second < c[0].size();
}

void illuminate(const Cavern &c, Illumination &i, std::pair<size_t, size_t> p = {0, 0}, Directions d = Directions::Right)
{
    if (!(i[p.first][p.second] & (1 << static_cast<int>(d))))
    {
        i[p.first][p.second] += (1 << static_cast<int>(d));
        if (c[p.first][p.second] == '.')
        {
            const auto pos = getNewPosition(p, d);
            if (isInBounds(c, pos))
            {
                illuminate(c, i, pos, d);
            }
        }
        else
        {
            const auto dirs = tryGetDir(c[p.first][p.second], d);
            for (auto &&d1 : dirs)
            {
                const auto pos = getNewPosition(p, d1);
                if (isInBounds(c, pos))
                {
                    illuminate(c, i, pos, d1);
                }
            }
        }
    }
}

int calcIllumination(const Illumination &illuminated)
{
    int ans = 0;
    for (size_t i = 0; i < illuminated.size(); i++)
    {
        for (size_t j = 0; j < illuminated[i].size(); j++)
        {
            if (illuminated[i][j])
                ans++;
        }
    }
    return ans;
}

int getIllumination(const Cavern &c, std::pair<size_t, size_t> p = {0, 0}, Directions d = Directions::Right)
{
    Illumination illuminated(c.size(), std::vector<int>(c[0].size(), 0));
    illuminate(c, illuminated, p, d);
    return calcIllumination(illuminated);
}

int main(int argc, char const *argv[])
{
    std::ifstream inputFile;
    inputFile.open(argv[1]);
    std::string line;
    int ans = 0;
    Cavern input;
    while (std::getline(inputFile, line))
        input.emplace_back(std::move(line));
    inputFile.close();

    ans = getIllumination(input);
    std::cout << ans;
    return 0;
}
