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
Refector ideas:
    - swap hashset for something more performant?
        -> std::set: could lead to issues with sorting? if set uses binary search and i have lots of similar valies
        -> 4Dimensional datastucture:
            ->very bad readability, prone to bugs, is it even better?
    - scoped enum
        -> could it include the isReversing() function?
    - Setting member variables in .solve() feels ugly
    - think of something more performant for path finding like a more solid heuristic that wont mess with the constraints.
        - DijkstraState doesn't know about inputMap2D size
        - Dijkstra has the advantage that we can quit once we find the finish for the first time.
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
    uint32_t solve(uint32_t maxsteps, uint32_t minsteps)
    {
        Timer t;
        this->min_steps = minsteps;
        this->max_steps = maxsteps;

        std::unordered_set<DijkstraState, HashDijkstraState> visited;

        DijkstraQueue dijkstraQueue = initDijkstraQueue();

        while (!dijkstraQueue.empty())
        {
            DijkstraState current = dijkstraQueue.top();
            dijkstraQueue.pop();

            if (current.straight_steps >= min_steps && current.x == input_map[0].size() - 1 && current.y == input_map.size() - 1)
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
                        dijkstraQueue.push(*new_state);
                    }
                }
            }
        }
        return INT_MAX;
    };

private:
    struct DijkstraState;
    using inputMap2D = std::vector<std::vector<uint32_t>>;
    using DijkstraQueue = std::priority_queue<DijkstraState, std::vector<DijkstraState>, std::greater<DijkstraState>>;

    const inputMap2D input_map;
    uint32_t min_steps;
    uint32_t max_steps;

    struct DijkstraState
    {
        uint32_t x;
        uint32_t y;
        uint32_t path_length;
        uint32_t straight_steps;
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
            // Due to the constraints of our problem we know that the finish is always at the maximum
            // x and y indices. This is why I am using them as an additional heuristic for a small speed up.
            return (path_length - x - y) > (other.path_length - other.x - other.y);
        }

        void printState() const
        {
            std::cout << '\n';
            std::cout << "Path length: " << path_length << '\n';
            std::cout << "Position: " << x << ',' << y << '\n';
            std::cout << "Direction: " << getDirectionChar() << '\n';
            std::cout << "Straight steps: " << straight_steps << '\n';
        }

    private:
        char getDirectionChar() const
        {
            switch (direction)
            {
            case Directions::Up:
                return '^';
            case Directions::Right:
                return '>';
            case Directions::Down:
                return 'v';
            case Directions::Left:
                return '<';
            case Directions::None:
                return 'x';
            }
        }

        void takeStep()
        {
            // Better to let underflow happen and handle it in inputMap2D bounds check. 
            // only decrementing if > 0 leads to the case where you're allowed to take "steps" in-place by running into walls
            switch (direction)
            {
            case Directions::Up:
                y--; 
                break;
            case Directions::Left:
                x--;
                break;
            case Directions::Down:
                y++;
                break;
            case Directions::Right:
                x++;
                break;
            case Directions::None:
                throw(std::runtime_error("Can't take step in Directions::None\n"));
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
            return std::hash<uint32_t>()(step.x) ^
                   std::hash<uint32_t>()(step.y) ^
                   std::hash<Directions>()(step.direction) ^
                   std::hash<uint32_t>()(step.straight_steps);
        }
    };

    /*
    Tries to create a new DijkstraStep based on the previous DijkstraStep and a Direction.
    Only returns a DijkstraStep if the step is valid and all conditions are met.
    */
    std::optional<DijkstraState> getNewStateInTravelDir(const DijkstraState &previous, const Directions direction, const inputMap2D &map)
    {
        DijkstraState current(previous, direction);

        if (checkInBounds2D(current.x, current.y, map) &&
            !isReversing(previous.direction, current.direction) &&
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
    inputMap2D retrievePuzzleInput(const std::string &inputfilepath)
    {
        inputMap2D ret;
        std::ifstream inputFile;
        std::string line;
        inputFile.open(inputfilepath);

        if (!inputFile.is_open())
        {
            throw std::runtime_error("Couldn't open file.\n");
        }

        while (std::getline(inputFile, line))
        {
            ret.push_back(std::vector<unsigned int>(line.size()));
            std::transform(line.begin(), line.end(), ret[ret.size() - 1].begin(), [](char c)
                           { return c - '0'; });
        }

        inputFile.close();
        
        if(ret.size() == 0) {
            throw std::runtime_error("Incorrect file input.\n");
        }

        return ret;
    }

    /*
    Checks if a Position is inside of a 2D vector
    */
    inline bool checkInBounds2D(uint32_t x, uint32_t y, const inputMap2D &vec)
    {
        return x < vec[0].size() && y < vec.size();
    }

    /*
    Checks if 2 directions of travel cancel eachother out (e.g. reversing)
    Enum layout lets us calculate the opposite direction instead of checking it with if else or switch
    */
    inline bool isReversing(Directions dir1, Directions dir2)
    {
        return (static_cast<int>(dir1) == ((static_cast<int>(dir2) + 2) % 4));
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
    DijkstraQueue initDijkstraQueue()
    {
        DijkstraQueue dq;
        dq.emplace();
        return dq;
    }
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
        uint32_t p1 = solver.solve(3, 0);
        std::cout << "Answer: " << p1 << '\n';

        std::cout << "Part 2:\n";
        uint32_t p2 = solver.solve(10, 4);
        std::cout << "Answer: " << p2 << '\n';
    }
    catch (const std::runtime_error &e)
    {
        std::cerr << e.what() << std::endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}