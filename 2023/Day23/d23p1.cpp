#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <algorithm>
#include <iterator>

using Path = std::vector<std::string>;
using Position = std::pair<int, int>;

class SnowIsland
{
public:
    SnowIsland(const std::string &inputFilePath) : m_path(std::move(getInputFromFile(inputFilePath))),
                                                   m_start({0, 1}),
                                                   m_end({m_path.size() - 1, m_path.at(0).size() - 2}) {}

    int getMaxPathLength()
    {
        auto v = getEmptyVisited();
        const auto path_lengths = getAllPathLengths(m_start, 0, v);
        const auto max_path = std::max_element(path_lengths.begin(), path_lengths.end());
        return max_path == path_lengths.end() ? 0 : *max_path;
    }

private:
    Path m_path;
    Position m_start, m_end;

    std::vector<std::vector<bool>> getEmptyVisited()
    {
        return std::vector<std::vector<bool>>(m_path.size(), std::vector<bool>(m_path.at(0).size(), false));
    }

    std::vector<int> getAllPathLengths(const Position &p, int path_length, std::vector<std::vector<bool>> &visited)
    {
        std::vector<int> possible_path_lengths;

        if (p.first == m_end.first && p.second == m_end.second)
        {
            return {path_length};
        }

        visited[p.first][p.second] = true;
        auto next_steps = getPossibleSteps(p);
        next_steps.erase(std::remove_if(next_steps.begin(), next_steps.end(),
                                        [&visited](const Position &p)
                                        {
                                            return visited[p.first][p.second];
                                        }),
                         next_steps.end()); // remove positions that have already been visited

        switch (next_steps.size())
        {
        case 0:
            return {};
        case 1:
            return getAllPathLengths(next_steps[0], path_length + 1, visited);
        default:
            for (auto &&next : next_steps) // only copy visited if necessery (technically one less copy would be necessary)
            {
                auto v = std::vector<std::vector<bool>>(visited);
                const auto path_lengths = getAllPathLengths(next, path_length + 1, v);
                std::for_each(path_lengths.begin(), path_lengths.end(), [&](const int l)
                              { possible_path_lengths.push_back(l); });
            }
            return possible_path_lengths;
        }
        return {};
    }

    Path getInputFromFile(const std::string &inputFilePath)
    {
        std::ifstream inFile(inputFilePath);
        std::string line;
        Path path;
        while (std::getline(inFile, line))
        {
            path.emplace_back(std::move(line));
        }
        return path;
    }

    std::vector<Position> getPossibleSteps(const Position &p)
    {
        std::vector<Position> next_positions;
        for (Position &delta : std::vector<Position>({{0, 1}, {1, 0}, {-1, 0}, {0, -1}}))
        {
            Position new_pos = {p.first + delta.first, p.second + delta.second};
            if (isValidPosition(new_pos) && correctDirectionOnSlope(p, delta))
                next_positions.push_back(new_pos);
        }
        return next_positions;
    }

    bool isValidPosition(const Position &p)
    {
        return p.first >= 0 && p.second >= 0 &&
               p.first < m_path.size() && p.second < m_path.at(0).size() &&
               m_path[p.first][p.second] != '#';
    }

    bool correctDirectionOnSlope(const Position &p, const Position &delta)
    {
        switch (m_path[p.first][p.second])
        {
        case '<':
            return delta.first == 0 && delta.second == -1;
        case '>':
            return delta.first == 0 && delta.second == 1;
        case '^':
            return delta.first == -1 && delta.second == 1;
        case 'v':
            return delta.first == 1 && delta.second == 0;
        default:
            return true;
        }
    }
};

int main(int argc, char const *argv[])
{
    int ans = 0;
    SnowIsland si(argv[1]);
    ans = si.getMaxPathLength();
    std::cout << ans;
    return 0;
}
