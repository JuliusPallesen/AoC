#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <algorithm>
#include <set>
#include <unordered_map>
#include <array>
#include <optional>
#include <queue>

using Path = std::vector<std::string>;
using Position = std::pair<int, int>;

namespace Dir
{
    constexpr std::array<Position, 4> dir_vec = {Position({-1, 0}), {0, 1}, {1, 0}, {0, -1}};

    enum Direction
    {
        Up = 0,
        Right = 1,
        Down = 2,
        Left = 3
    };

    // adding 2 and taking the result modulo the amount of directions works due to the opposite
    // directions always being 2 appart.
    Direction getOppositeDirection(const Direction dir)
    {
        if (dir == Direction::Up)
            return Direction::Down;
        if (dir == Direction::Left)
            return Direction::Right;
        if (dir == Direction::Right)
            return Direction::Left;
        return Direction::Up;
    }
}

struct PositionHash
{
    size_t operator()(const Position &p) const
    {
        return std::hash<int>()(p.first) ^ std::hash<int>()(p.second);
    }
};

class SnowIsland
{
public:
    SnowIsland(const std::string &inputFilePath) : m_path(getInputFromFile(inputFilePath)),
                                                   m_start({0, 1}),
                                                   m_end({m_path.size() - 1, m_path.at(0).size() - 2}) {}

    int longestPath()
    {
        fillGraph();
        std::set<Position> visited;
        return dfsMaxPathLength(m_start, 0, visited);
    }

private:
    Path m_path;
    Position m_start, m_end;
    int m_max = 0;

    using ConnectionDirections = std::vector<std::optional<std::pair<int, Position>>>;

    /*
    Graph implemented using hash map
    Intersection points are entered into the hash map and know the distance to the next intersection node
    for each direction of travel (if applicable)
    */
    std::unordered_map<Position, ConnectionDirections, PositionHash> graph;

    void fillGraph()
    {
        auto v = getEmptyVisited();
        analyzePath(m_start, 0, m_start, v, Dir::Direction::Down, Dir::Direction::Down);
    }

    /*
    Turning the maze into a graph let's me brute force it using dfs in ~3minutes
    i'm sure im not far off of solving it in a couple of second but it would take some thinking
    */
    int dfsMaxPathLength(const Position &p, int path_length, std::set<Position> &visited)
    {
        int max = INT_MIN;
        visited.insert(p);

        if (p == m_end)
            return path_length;

        for (auto &&next : graph[p])
        {
            if (next && visited.find(next->second) == visited.end())
            {
                std::set<Position> v(visited);
                const int path_len = dfsMaxPathLength(next->second, path_length + next->first, v);
                max = std::max(max, path_len);
            }
        }
        return max;
    }

    bool isIntersection(const Position &p)
    {
        return graph.count(p) ? true : getPossibleSteps(p).size() > 2;
    }

    void setOptionalConnectionToMax(std::optional<std::pair<int, Position>> &opt, const int &len, const Position &p)
    {
        if (!opt)
            opt = {len, p};
        opt = opt->first > len ? opt : std::optional<std::pair<int, Position>>({len, p});
    }

    void analyzePath(const Position &p, int path_length, const Position &last, std::vector<std::vector<bool>> &visited, Dir::Direction dir, Dir::Direction last_dir)
    {
        visited[p.first][p.second] = true;

        if (isIntersection(p) || (p == m_end) || (p == m_start))
        {
            if (graph.count(p) == 0)
                graph[p] = std::vector<std::optional<std::pair<int, Position>>>(4, std::optional<std::pair<int, Position>>());

            if (p != last)
            {
                setOptionalConnectionToMax(graph[p][static_cast<int>(Dir::getOppositeDirection(dir))], path_length, last);
                setOptionalConnectionToMax(graph[last][last_dir], path_length, p);
            }
        }

        for (size_t i = 0; i < Dir::dir_vec.size(); i++)
        {
            const Dir::Direction curr_dir = static_cast<Dir::Direction>(i);
            const Position new_pos = {p.first + Dir::dir_vec[i].first, p.second + Dir::dir_vec[i].second};
            std::vector<std::vector<bool>> v(visited);
            if (isValidPosition(new_pos) && (!visited[new_pos.first][new_pos.second] || isIntersection(new_pos)))
            {
                if (isIntersection(p))
                {
                    if (!graph[p][i])
                        analyzePath(new_pos, 1, p, v, curr_dir, curr_dir);
                }
                else
                {
                    analyzePath(new_pos, path_length + 1, last, v, curr_dir, last_dir);
                }
            }
        }
    }

    std::vector<std::vector<bool>> getEmptyVisited()
    {
        return std::vector<std::vector<bool>>(m_path.size(), std::vector<bool>(m_path.at(0).size(), false));
    }

    Path getInputFromFile(const std::string &inputFilePath)
    {
        std::ifstream inFile(inputFilePath);
        std::string line;
        Path path;
        while (std::getline(inFile, line))
        {
            path.emplace_back(line);
        }
        return path;
    }

    std::vector<Position> getPossibleSteps(const Position &p)
    {
        std::vector<Position> next_positions;
        for (const Position &delta : Dir::dir_vec)
        {
            Position new_pos = {p.first + delta.first, p.second + delta.second};
            if (isValidPosition(new_pos))
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
};

int main(int argc, char const *argv[])
{
    int ans = 0;
    SnowIsland si(argv[1]);

    ans = si.longestPath();
    std::cout << ans;
    return 0;
}
