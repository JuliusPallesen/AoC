#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <algorithm>
#include <queue>

/*
Solution stolen from: https://github.com/villuna/aoc23/wiki/A-Geometric-solution-to-advent-of-code-2023,-day-21
*/

using Garden = std::vector<std::string>;

class ElfGarden
{
public:
    ElfGarden(const std::string &inputFilePath)
    {
        getGardenFromFilePath(inputFilePath);
        m_size = m_garden.size();
    }

    int64_t calcPossibilities(int N)
    {
        int64_t b = (N - m_size / 2) / m_size;
        const auto reached = calcParity();

        auto ans = std::count_if(reached.begin(), reached.end(), [&N](const auto r)
                                 { return r <= N && (r % 2) == (N % 2) && r != -1; });

        if (b > 0)
        {
            const auto half = m_size / 2;
            const auto evenCorner = std::count_if(reached.begin(), reached.end(), [&half](const auto r)
                                                  { return r > half && (r % 2) == 0 && r != -1; });

            const auto oddCorner = std::count_if(reached.begin(), reached.end(), [&half](const auto r)
                                                 { return r > half && (r % 2) == 1 && r != -1; });

            const auto evenFull = std::count_if(reached.begin(), reached.end(), [](const auto r)
                                                { return (r % 2) == 0 && r != -1; });

            const auto oddFull = std::count_if(reached.begin(), reached.end(), [](const auto r)
                                               { return (r % 2) == 1 && r != -1; });

            ans = ((b + 1) * (b + 1) * oddFull) + ((b * b) * evenFull) - ((b + 1) * oddCorner) + (b * evenCorner);
        }

        return ans;
    }

private:
    Garden m_garden;
    int64_t m_size;

    void getGardenFromFilePath(const std::string &inputFilePath)
    {
        std::ifstream inputFile;
        inputFile.open(inputFilePath);
        std::string line;
        while (std::getline(inputFile, line))
        {
            m_garden.emplace_back(std::move(line));
        }
        inputFile.close();
    }

    bool isValidPos(int y, int x)
    {
        return y >= 0 && x >= 0 && y < m_size && x < m_size && m_garden[y][x] != '#';
    }

    std::vector<int> calcParity()
    {
        int64_t ans = 0;
        std::vector<int> reached(m_size * m_size, -1);
        std::queue<std::pair<std::pair<int, int>, int>> nextSteps;
        nextSteps.push({{m_size / 2, m_size / 2}, 0});
        while (!nextSteps.empty())
        {
            const auto [pos, steps] = nextSteps.front();
            const auto [y, x] = pos;
            const auto i = (y * m_size) + x;
            nextSteps.pop();
            if (reached[i] != -1)
                continue;

            reached[i] = steps;

            const std::vector<std::pair<int, int>> directions = {{0, 1}, {1, 0}, {-1, 0}, {0, -1}};
            for (const auto &delta : directions)
            {
                const int new_y = y + delta.first;
                const int new_x = x + delta.second;
                const int new_i = (new_y * m_size) + new_x;
                if (isValidPos(new_y, new_x) && reached[new_i] == -1)
                {
                    nextSteps.push({{new_y, new_x}, steps + 1});
                }
            }
        }
        return reached;
    }
};

int main(int argc, char const *argv[])
{
    constexpr int64_t STEPS = 26501365;
    int64_t ans = 0;

    ElfGarden garden(argv[1]);

    ans = garden.calcPossibilities(STEPS);

    std::cout << ans << "\n";
    return EXIT_SUCCESS;
}
