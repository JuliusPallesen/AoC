#pragma once

#include <vector>
#include <string>
#include <fstream>
#include <format>
#include <chrono>
#include <ranges>
#include <algorithm>
#include <numeric>
#include <iostream>
#include <variant>
#include <optional>
#include <functional>
#include <type_traits>
#include <ranges>
#include <concepts>
#include <queue>
#include <climits>

namespace aoc
{
    /*
    Commonn string parsing Tasks
    */
    const std::vector<std::string> DIGIT_STRINGS = {"zero", "one", "two", "three", "four", "five", "six", "seven", "eight", "nine"};

    std::vector<std::string> splitString(const std::string &input_str, const char split_char = ' ', bool allow_empty = false)
    {
        std::vector<std::string> split_string;
        for (auto &&ch : input_str)
        {
            if (ch == split_char)
            {
                if (allow_empty || (!split_string.empty() && !split_string.back().empty()))
                    split_string.emplace_back("");
            }
            else
            {
                if (split_string.empty())
                    split_string.emplace_back("");
                split_string.back().push_back(ch);
            }
        }
        return split_string;
    }

    std::vector<std::string> splitString(const std::string &input_str, const std::string &split_by_any, bool allow_empty = false)
    {
        std::vector<std::string> split_string;
        for (auto &&ch : input_str)
        {
            if (split_by_any.find(ch) != split_by_any.npos)
            {
                if (allow_empty || (!split_string.empty() && !split_string.back().empty()))
                    split_string.emplace_back("");
            }
            else
            {
                if (split_string.empty())
                    split_string.emplace_back("");
                split_string.back().push_back(ch);
            }
        }
        return split_string;
    }

    /*Could be a lot more efficient without substringing*/
    std::vector<std::string> splitStringByString(const std::string &input_str, const std::string &split_by_string, bool allow_empty = false)
    {
        std::string working_str(input_str);
        std::vector<std::string> split_string;
        while (!working_str.empty())
        {
            auto pos = working_str.find(split_by_string);
            if (std::string::npos == pos)
            {
                split_string.emplace_back(std::move(working_str));
                break;
            }

            if (const auto sub_str = working_str.substr(0, pos); allow_empty || !sub_str.empty())
                split_string.emplace_back(std::move(sub_str));

            working_str = working_str.substr(pos + split_by_string.size());
        }
        return split_string;
    }

    /*merge and template these?*/
    std::string joinString(const std::vector<std::string> &input, const char join_char = '\0')
    {
        std::string str;
        for (auto &&s : input)
        {
            str += s;
            if (join_char != '\0' && &s != &input.back())
                str += join_char;
        }
        return str;
    }

    std::string joinString(const std::vector<std::string> &input, const std::string &join_string)
    {
        std::string str;
        for (auto &&s : input)
        {
            str += s;
            if (&s != &input.back())
                str += join_string;
        }
        return str;
    }

    /*
    Common File parsing
    */

    void checkFileValidity(const std::ifstream &file_stream)
    {
        if (!file_stream.is_open() || !file_stream.good())
            throw(std::runtime_error("Couldn't open file"));
    }

    std::vector<std::string> parseFileAsStringVector(const std::string &file_path)
    {
        std::ifstream input_file(file_path);
        checkFileValidity(input_file);
        std::string content((std::istreambuf_iterator<char>(input_file)), std::istreambuf_iterator<char>());
        return splitString(content, '\n', true);
    }

    std::vector<int> parseFileAsIntVector(const std::string &file_path)
    {
        std::vector<int> input;
        const auto strings = parseFileAsStringVector(file_path);
        std::ranges::transform(strings, std::back_inserter(input), [](const auto &s)
                               { return std::stoi(s); });
        return input;
    }

    std::vector<std::vector<std::variant<int, std::string>>> parseFileAsVariantVector(const std::string &file_path)
    {
        std::ifstream input_file(file_path);
        std::string line;
        std::vector<std::vector<std::variant<int, std::string>>> input(1);
        checkFileValidity(input_file);
        while (std::getline(input_file, line))
        {
            for (auto &&word : splitString(line, ' '))
            {
                if (int num = 0; std::sscanf(word.c_str(), "%d", &num))
                    input.back().emplace_back(num);
                else
                    input.back().emplace_back(std::move(word));
            }
            input.push_back(std::vector<std::variant<int, std::string>>());
        }
        return input;
    }
    /*
    Common operations on 2D grids for AoC Tasks
    */

    struct Position2D
    {
        int y, x;

        Position2D(int y_pos, int x_pos) : y(y_pos), x(x_pos) {}
        bool operator==(const Position2D &other) const { return y == other.y && x == other.x; }
        Position2D operator+(const Position2D &other) const { return {y + other.y, x + other.x}; }
        int distance(const Position2D &other) const { return std::abs(x - other.x) + std::abs(y - other.y); }
        std::size_t getHash() const { return hasher(y << 16) ^ hasher(x); }

    private:
        static std::hash<int> hasher;
    };

    std::hash<int> Position2D::hasher{};

    enum class Directions
    {
        Up = 0,
        Right = 1,
        Down = 2,
        Left = 3
    };

    static const std::vector<Directions> DIRECTIONS = {Directions::Up, Directions::Right, Directions::Down, Directions::Left};
    static const std::vector<Position2D> VON_NEUMANN_NEIGHBOUR_DELTAS = {{0, 1}, {1, 0}, {-1, 0}, {0, -1}};
    static const std::vector<Position2D> MOORE_NEIGHBOUR_DELTAS = {{0, 1}, {1, 0}, {-1, 0}, {0, -1}, {-1, -1}, {1, -1}, {-1, 1}, {1, 1}};

    Position2D getDelta(const Directions &dir)
    {
        return VON_NEUMANN_NEIGHBOUR_DELTAS[static_cast<size_t>(dir)];
    }

    Directions getDirFromCharURDL(const char ch)
    {
        switch (ch)
        {
        case 'U':
            return Directions::Up;
        case 'D':
            return Directions::Down;
        case 'R':
            return Directions::Right;
        case 'L':
            return Directions::Left;
        default:
            throw(std::runtime_error(std::format("Invalid Character '{}' cannot convert to direction.\n", ch)));
        }
    }

    std::string printDirection(const Directions &dir)
    {
        switch (dir)
        {
        case Directions::Up:
            return "Up";
        case Directions::Down:
            return "Down";
        case Directions::Left:
            return "Left";
        case Directions::Right:
            return "Right";
        default:
            return "No direction";
        }
    }

    Directions getDirFromCharNESW(const char ch)
    {
        switch (ch)
        {
        case 'N':
            return Directions::Up;
        case 'S':
            return Directions::Down;
        case 'E':
            return Directions::Right;
        case 'W':
            return Directions::Left;
        default:
            throw(std::runtime_error(std::format("Invalid Character '{}' cannot convert to direction.\n", ch)));
        }
    }

    template <typename T>
    concept TwoDimensionalContainer = requires(T data) {
        { data[0][0] } -> std::same_as<typename std::remove_reference<decltype(data[0][0])>::type &>;
    };

    bool opposite_directions(const Directions &lhs, const Directions &rhs) { return ((static_cast<int>(lhs) + 2) % 4) == static_cast<int>(rhs); }

    template <typename T>
    bool isInBounds1D(const T &vec, const int index) { return index >= 0 && static_cast<std::size_t>(index) < vec.size(); }

    template <TwoDimensionalContainer T>
    bool isInBounds2D(const T &vec2d, const Position2D &pos)
    {
        return pos.y >= 0 && static_cast<std::size_t>(pos.y) < vec2d.size() &&
               pos.x >= 0 && static_cast<std::size_t>(pos.x) < vec2d.at(pos.y).size();
    }

    template <TwoDimensionalContainer T>
    auto &at2D(T &vec2d, const Position2D &pos) { return vec2d[pos.y][pos.x]; }

    template <TwoDimensionalContainer T>
    auto &at2Ds(T &vec2d, const Position2D &pos)
    {
        if (isInBounds2D(vec2d, pos))
            return at2D(vec2d, pos);
        else
            throw(std::runtime_error(std::format("Index {},{} out of bounds in vector with dimensions {},{}",
                                                 pos.y, pos.x, vec2d.size(), vec2d.size() > 0 ? vec2d.at(0).size() : 0)));
    }

    /*
    Printing multi dimensional Datastructures
    */
    template <typename T>
    void printVector(const std::vector<T> &v, const char item_break = ' ')
    {
        std::ranges::for_each(v, [&](const auto &i)
                              { std::cout << i << item_break; });
    }

    template <TwoDimensionalContainer T>
    void print2Dvector(const T &vec)
    {
        std::ranges::for_each(vec, [&](const auto &v)
                              {
                printVector(v,' ');
                std::cout << "\n"; });
    }

    /*
    Timer used to evaluate performance of a program using RAII
    */
    class Timer
    {
    public:
        Timer() : start(std::chrono::steady_clock::now()) {}

        ~Timer()
        {
            std::cout << "time elapsed: " << std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - start).count()
                      << "ms" << std::endl;
        }

    private:
        const std::chrono::steady_clock::time_point start;
    };

    /*
    - Finding Items inside of 2D grids
    - Finding Paths inside of 2D grids
    */
    struct DijkstraState
    {
        Position2D pos;
        int length;

        DijkstraState(const Position2D &p, int len) : pos(p), length(len) {}
        bool operator==(const DijkstraState &other) const { return other.length == length && other.pos == pos; }
        bool operator<(const DijkstraState &other) const { return length > other.length; }
        bool operator>(const DijkstraState &other) const { return length < other.length; }
    };

    struct DijkstraCompOp
    {
        bool operator()(const DijkstraState &lhs, const DijkstraState &rhs) { return lhs.length > rhs.length; }
    };

    template <TwoDimensionalContainer T>
    std::optional<Position2D> find2D(const T &vec2d, const typename T::value_type::value_type &item)
    {
        for (int y = 0; y < static_cast<int>(vec2d.size()); ++y)
            for (int x = 0; x < static_cast<int>(vec2d.at(y).size()); ++x)
                if (vec2d[y][x] == item)
                    return Position2D{y, x};
        return std::nullopt;
    }

    template <TwoDimensionalContainer T>
    std::vector<Position2D> getPossibleNeighbours(
        const T &vec2d,
        const Position2D &pos,
        const std::function<bool(Position2D)> &rule = [](const Position2D &p)
        { return true; },
        const std::vector<Position2D> &neighbour_deltas = VON_NEUMANN_NEIGHBOUR_DELTAS)
    {
        std::vector<Position2D> neighbours;
        for (auto &&delta : neighbour_deltas)
        {
            const auto new_pos = pos + delta;
            if (isInBounds2D(vec2d, new_pos) &&
                rule(new_pos))
            {
                neighbours.emplace_back(std::move(new_pos));
            }
        }
        return neighbours;
    }

    template <TwoDimensionalContainer T>
    int dijkstra(
        const T &vec2d,
        const Position2D &start,
        const std::function<bool(Position2D)> &finished,
        const std::function<bool(Position2D)> &rule = [](const Position2D &p)
        { return true; },
        const std::vector<Position2D> &neighbour_deltas = VON_NEUMANN_NEIGHBOUR_DELTAS)
    {
        if (vec2d.empty())
            return INT_MAX;

        std::priority_queue<DijkstraState> q;
        std::vector<std::vector<bool>> visited(vec2d.size(), std::vector<bool>(vec2d.at(0).size(), false));
        const auto condition = [&visited, &rule](const Position2D &p)
        { return !visited[p.y][p.x] && rule(p); };

        q.push(DijkstraState{start, 0});

        while (!q.empty())
        {
            const auto [pos, length] = q.top();
            q.pop();

            if (finished(pos))
                return length;

            for (auto &&neighbour : getPossibleNeighbours(vec2d, pos, condition, neighbour_deltas))
            {
                visited[neighbour.y][neighbour.x] = true;
                q.push(DijkstraState{neighbour, length + 1});
            }
        }
        return INT_MAX;
    }
}