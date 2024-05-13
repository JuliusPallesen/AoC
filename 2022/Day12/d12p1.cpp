#include <vector>
#include <ranges>
#include <string>
#include <algorithm>
#include <functional>
#include <fstream>
#include <iostream>
#include <format>
#include <optional>
#include <functional>
#include <queue>

class D12solver
{
public:
    D12solver(const std::string &filepath) : m_heightmap(std::move(parseInput(filepath))),
                                             m_visited(std::vector<std::vector<bool>>(m_heightmap.size(), std::vector<bool>(m_heightmap.at(0).size(), false))),
                                             m_queue(std::move(initQueue()))
    {
        if (!m_end || !m_start)
            std::cerr << "Inputfile didn't include start and end character.\n";
        else
            std::cout << std::format("Start: [{},{}]\tEnd: [{},{}]\n", m_start->y, m_start->x, m_end->y, m_end->x);
    }

    int solve()
    {
        return performAStar();
    }

private:
    struct Position
    {
        int y, x;
        Position(int y_pos, int x_pos) : y(y_pos), x(x_pos) {}

        Position operator+(const Position &rhs) const
        {
            return {y + rhs.y, x + rhs.x};
        }

        bool operator==(const Position &rhs) const
        {
            return y == rhs.y && x == rhs.x;
        }

        int distance(const Position &other) const
        {
            return std::abs(y - other.y) + std::abs(x - other.x);
        }
    };

    using State = std::tuple<int, int, Position>; // length, dist, pos

    struct stateCmp
    {
        bool operator()(const State &lhs, const State &rhs)
        {
            const auto &[ll, ld, lp] = lhs;
            const auto &[rl, rd, rp] = rhs;
            return (ll + ld) >= (rl + rd);
        }
    };

    using StateQueue = std::priority_queue<State, std::vector<State>, stateCmp>;

    int m_lines_count = 0;
    std::optional<Position> m_start, m_end;
    const std::vector<std::string> m_heightmap;
    std::vector<std::vector<bool>> m_visited;
    StateQueue m_queue;
    const std::vector<Position> NEIGHBOUR_DELTAS = {{0, 1}, {1, 0}, {0, -1}, {-1, 0}};

    auto initQueue() -> StateQueue const
    {
        StateQueue q;
        q.emplace(std::make_tuple(0, m_start->distance(*m_end), *m_start));
        return q;
    }

    auto performAStar() -> int const
    {
        while (!m_queue.empty())
        {
            const auto [length, dist, pos] = m_queue.top();
            m_queue.pop();
            if (*m_end == pos)
                return length;

            // m_visited[pos.y][pos.x] = true;

            const auto neighbours = getValidNeighbours(pos);
            std::ranges::for_each(neighbours, [&](const auto &p)
                                  { m_visited[pos.y][pos.x] = true;
                                    m_queue.emplace(std::make_tuple(length + 1, p.distance(*m_end), p)); });
        }
        return 0;
    }

    auto getValidNeighbours(const Position &pos) -> std::vector<Position> const
    {
        std::vector<Position> valid_positions;
        for (auto &&d : NEIGHBOUR_DELTAS)
        {
            const auto p = pos + d;
            if ((p.y >= 0 && p.x >= 0 &&
                 static_cast<size_t>(p.y) < m_heightmap.size() && static_cast<size_t>(p.x) < m_heightmap.at(0).size()) &&
                ((m_heightmap[p.y][p.x] - m_heightmap[pos.y][pos.x]) <= 1) &&
                !m_visited[p.y][p.x])
            {
                valid_positions.push_back(p);
            }
        }
        return valid_positions;
    }

    auto searchCharPos(const std::string &line, const char search_char) -> std::optional<Position> const
    {
        const auto s_pos = line.find(search_char);
        return (s_pos != line.npos) ? std::optional<Position>({m_lines_count, static_cast<int>(s_pos)}) : std::nullopt;
    }

    auto tryInitStartEnd(const std::string &line) -> void
    {
        m_start = m_start ? m_start : searchCharPos(line, 'S');
        m_end = m_end ? m_end : searchCharPos(line, 'E');
    }

    auto checkValidFile(const std::ifstream &inputFile) -> void
    {
        if (!inputFile.is_open())
        {
            std::cerr << "Couldn't open inputfile, please specify a valid filepath.\n";
            exit(EXIT_FAILURE);
        }
    }

    auto parseInput(const std::string &fp) -> std::vector<std::string>
    {
        std::ifstream inputFile;
        std::vector<std::string> input;
        std::string line;

        inputFile.open(fp);
        checkValidFile(inputFile);

        while (std::getline(inputFile, line))
        {
            tryInitStartEnd(line);
            input.emplace_back(std::move(line));
            ++m_lines_count;
        }
        input[m_start->y][m_start->x] = 'a';
        input[m_end->y][m_end->x] = 'z';
        return input;
    }
};

auto main(int argc, char const *argv[]) -> int
{
    if (argc <= 1)
    {
        std::cerr << "Usage: d12p1 <input filepath>\n";
        exit(EXIT_FAILURE);
    }

    D12solver solver{argv[1]};

    std::cout << std::format("D12 result: {}\n", solver.solve());

    return EXIT_SUCCESS;
}