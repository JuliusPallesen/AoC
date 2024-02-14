#include <algorithm>
#include <chrono>
#include <fstream>
#include <iostream>
#include <optional>
#include <queue>
#include <sstream>
#include <unordered_set>
#include <vector>

/*
Refector ideas
swap hashset for normal set? -> could lead to issues with sorting? if set uses binary search and i have lots of similar valies
scoped enum?
*/
enum class Directions
{
    Up = 0,
    Right = 1,
    Down = 2,
    Left = 3,
    None = 4
};

class Timer
{
public:

    Timer() : start(std::chrono::steady_clock::now())
    {
    }
    ~Timer()
    {
        std::cout << "time elapsed: " << std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - start).count()
                  << "ms" << std::endl;
    }

private:
    const std::chrono::steady_clock::time_point start;
};

class D17Solver
{
private:
    const std::vector<std::vector<unsigned int>> input_map;
    unsigned int min_steps;
    unsigned int max_steps;

    struct DijkstraState
    {
        unsigned int x;
        unsigned int y;
        unsigned int path_length;
        unsigned int straight_steps;
        Directions direction;

        DijkstraState() : x(0), y(0), path_length(0), straight_steps(0), direction(Directions::None)
        {
        }

        DijkstraState(const DijkstraState &previous, const Directions dir) : x(previous.x), y(previous.y), direction(dir)
        {
            takeStep();
            calcStraightSteps(previous);
        }

        ~DijkstraState() = default;
        DijkstraState(const DijkstraState &) = default;
        DijkstraState(DijkstraState &&) = default;
        DijkstraState &operator=(const DijkstraState &) = default;
        DijkstraState &operator=(DijkstraState &&) = default;

        bool operator==(const DijkstraState &other) const
        {
            return x == other.x &&
                   y == other.y &&
                   direction == other.direction &&
                   straight_steps == other.straight_steps;
        }

        bool operator>(DijkstraState const &other) const
        {
            const int v1 = path_length - x - y;
            const int v2 = other.path_length - other.x - other.y;
            return v1 > v2;
        }

        void printState()
        {
            std::cout << '\n';
            std::cout << "Path length: " << path_length << '\n';
            std::cout << "Position: " << x << ',' << y << '\n';
            std::cout << "Direction: " << getDirectionChar() << '\n';
            std::cout << "Straight steps: " << straight_steps << '\n';
        }

    private:
        char getDirectionChar()
        {
            switch (direction)
            {
            case Directions::Up:
                return '^';
                break;
            case Directions::Right:
                return '>';
                break;
            case Directions::Down:
                return 'v';
                break;
            case Directions::Left:
                return '<';
                break;
            default:
                return 'x';
                break;
            }
        }

        void takeStep()
        {
            switch (direction)
            {
            case Directions::Up:
                y == 0 ? 0 : --y;
                break;
            case Directions::Left:
                x == 0 ? 0 : --x;
                break;
            case Directions::Down:
                ++y;
                break;
            case Directions::Right:
                ++x;
                break;
            default:
                break;
            }
        }

        void calcStraightSteps(const DijkstraState &previous)
        {
            if (previous.direction == direction)
            {
                straight_steps = previous.straight_steps + 1;
            }
            else
            {
                straight_steps = 1;
            }
        }
    };

    /*
    Hashes a DijkstraStep.
    */
    class HashDijkstraState
    {
    public:
        size_t operator()(const DijkstraState &step) const
        {
            return std::hash<unsigned int>()(step.x) ^
                   std::hash<unsigned int>()(step.y) ^
                   std::hash<Directions>()(step.direction) ^
                   std::hash<unsigned int>()(step.straight_steps);
        }
    };

    /*
    Tries to create a new DijkstraStep based on the previous DijkstraStep and a Direction.
    Only returns a DijkstraStep if the step is valid and all conditions are met.
    */
    std::optional<DijkstraState> getNewStateInTravelDir(const DijkstraState &previous, const Directions direction, const std::vector<std::vector<unsigned int>> &map)
    {
        DijkstraState current(previous, direction);

        if (checkInBounds2D(current.x, current.y, map) &&
            notReversing(previous.direction, current.direction) &&
            isValidMove(previous, current))
        {
            current.path_length = previous.path_length + map[current.y][current.x];
            return current;
        }
        return {};
    }

    /*
    Retrieves the puzzleinput from a file and converts it to a suitable format
    */
    std::vector<std::vector<unsigned int>> retrievePuzzleInput(const std::string &inputfilepath)
    {
        std::vector<std::vector<unsigned int>> ret;
        std::ifstream inputFile;
        std::string line;
        inputFile.open(inputfilepath);

        if (!inputFile.is_open())
        {
            throw std::runtime_error("Couldn't open file.");
        }

        while (std::getline(inputFile, line))
        {
            ret.push_back(std::vector<unsigned int>(line.size()));
            std::transform(line.begin(), line.end(), ret[ret.size() - 1].begin(), [](char c)
                           { return c - '0'; });
        }

        inputFile.close();
        return ret;
    }

    /*
    Checks if a Position is inside of a 2D vector
    */
    inline bool checkInBounds2D(unsigned int x, unsigned int y, const std::vector<std::vector<unsigned int>> &vec)
    {
        return x < vec[0].size() && y < vec.size();
    }

    /*
    Checks if 2 directions of travel cancel eachother out (e.g. reversing)
    */
    inline bool notReversing(Directions dir1, Directions dir2)
    {
        return !(static_cast<int>(dir1) == ((static_cast<int>(dir2) + 2) % 4));
    }

    /*
    Checks if a move from previous to current is a Valid move.
    A valid move is one that only changes directions after the minimum straight_steps taken
    and changes directions before the maximum straight_steps taken are exceeded.
    */
    bool isValidMove(const DijkstraState &previous, const DijkstraState &current) const
    {
        if (previous.direction == Directions::None || previous.direction == current.direction)
        {
            return current.straight_steps <= max_steps;
        }
        else
        {
            return previous.straight_steps >= min_steps;
        }
    }

    /*
    Initializes a Proiority queue for the dijkstra algorithm
    */
    std::priority_queue<DijkstraState, std::vector<DijkstraState>, std::greater<DijkstraState>> initDijkstraQueue()
    {
        std::priority_queue<DijkstraState, std::vector<DijkstraState>, std::greater<DijkstraState>> pq;
        const DijkstraState start;
        pq.emplace(start);
        return pq;
    }

public:
    D17Solver(std::string inputfilepath) : input_map(retrievePuzzleInput(inputfilepath))
    {
    }

    ~D17Solver() = default;
    D17Solver(const D17Solver &) = default;
    D17Solver(D17Solver &&) = default;
    D17Solver &operator=(const D17Solver &) = default;
    D17Solver &operator=(D17Solver &&) = default;

    /*
    Solves the Advent of Code Day 17
        max_steps: maximum number of steps allowed in a straight line
        min_steps: minimum number of steps that need to be taken before you can turn
    */
    unsigned int solve(unsigned int max_steps, unsigned int min_steps)
    {
        Timer t;
        this->min_steps = min_steps;
        this->max_steps = max_steps;

        std::unordered_set<DijkstraState, HashDijkstraState> visited;

        std::priority_queue<DijkstraState, std::vector<DijkstraState>, std::greater<DijkstraState>> dijkstraQueue = initDijkstraQueue();

        while (!dijkstraQueue.empty())
        {
            DijkstraState current = dijkstraQueue.top();
            dijkstraQueue.pop();

            if (current.x == input_map[0].size() - 1 && current.y == input_map.size() - 1 && current.straight_steps >= min_steps)
            {
                return current.path_length;
            }
            if (visited.find(current) == visited.end())
            {
                visited.insert(current);
                for (const auto &dir : {Directions::Up, Directions::Right, Directions::Down, Directions::Left})
                {
                    const auto new_state = getNewStateInTravelDir(current, dir, input_map);
                    if (new_state)
                    {
                        dijkstraQueue.emplace(*new_state);
                    }
                }
            }
        }
        return INT_MAX;
    };
};

int main(int argc, char const *argv[])
{
    if (argc < 2)
    {
        return EXIT_FAILURE;
    }

    try
    {
        D17Solver solver(argv[1]);
        std::cout << "Part 1:\n";
        unsigned int p1 = solver.solve(3, 0);
        std::cout << "Answer: " << p1 << '\n';

        std::cout << "Part 2:\n";
        unsigned int p2 = solver.solve(10, 4);
        std::cout << "Answer: " << p2 << '\n';
    }
    catch (const std::runtime_error &e)
    {
        std::cerr << e.what() << std::endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
