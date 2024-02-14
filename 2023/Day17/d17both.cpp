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
enum Directions
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
    Timer() : start(std::chrono::high_resolution_clock::now())
    {
    }
    ~Timer()
    {
        std::cout << "time elapsed: " << std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - start).count()
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
        Directions last_direction;
        unsigned int straight_steps;

        DijkstraState() : last_direction(None), x(0), y(0), path_length(0), straight_steps(0)
        {
        }

        DijkstraState(const DijkstraState &previous, const Directions dir) : last_direction(dir), x(previous.x), y(previous.y)
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
                   last_direction == other.last_direction &&
                   straight_steps == other.straight_steps;
        }

        bool operator>(DijkstraState const &other) const
        {
            return this->path_length < other.path_length;
        }

        bool operator<(DijkstraState const &other) const
        {
            return this->path_length > other.path_length;
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
            switch (last_direction)
            {
            case Up:
                return '^';
                break;
            case Right:
                return '>';
                break;
            case Down:
                return 'v';
                break;
            case Left:
                return '<';
                break;
            default:
                return 'x';
                break;
            }
        }
        void takeStep()
        {
            switch (last_direction)
            {
            case Up:
                y--;
                break;
            case Left:
                x--;
                break;
            case Down:
                y++;
                break;
            case Right:
                x++;
                break;
            default:
                break;
            }
        }

        void calcStraightSteps(const DijkstraState &previous)
        {
            if (previous.last_direction == last_direction)
            {
                straight_steps = previous.straight_steps + 1;
            }
            else
            {
                straight_steps = 1;
            }
        }
    };

    class HashDijkstraState
    {
    public:
        size_t operator()(const DijkstraState &step) const
        {
            return (step.x << 16) | (step.y << 8) | (static_cast<int>(step.last_direction) << 4) | step.straight_steps;
        }
    };

    std::optional<DijkstraState> getNewStateInTravelDir(const DijkstraState &previous, Directions direction, const std::vector<std::vector<unsigned int>> &map)
    {
        DijkstraState new_state(previous, direction);

        if (checkInBounds2D(new_state.x, new_state.y, map) &&
            notReversing(previous.last_direction, direction) &&
            isValidMove(previous, new_state))
        {
            new_state.path_length = previous.path_length + map[new_state.y][new_state.x];
            return new_state;
        }
        return {};
    }

    std::vector<std::vector<unsigned int>> retrievePuzzleInput(std::string inputfilepath)
    {
        std::vector<std::vector<unsigned int>> ret;
        std::ifstream inputFile;
        std::string line;
        inputFile.open(inputfilepath);

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
    inline bool checkInBounds2D(int x, int y, const std::vector<std::vector<unsigned int>> &vec)
    {
        return x >= 0 && y >= 0 && x < vec[0].size() && y < vec.size();
    }

    /*
    Checks if 2 directions of travel cancel eachother out (e.g. reversing)
    */
    inline bool notReversing(Directions dir1, Directions dir2)
    {
        return !(static_cast<int>(dir1) == ((static_cast<int>(dir2) + 2) % 4));
    }

    bool isValidMove(const DijkstraState &previous, const DijkstraState &current) const
    {
        if (previous.last_direction == None || previous.last_direction == current.last_direction)
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
    std::priority_queue<DijkstraState, std::vector<DijkstraState>> initDijkstraQueue()
    {
        std::priority_queue<DijkstraState, std::vector<DijkstraState>> pq;
        ;
        DijkstraState start;
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

    unsigned int solve(unsigned int max_steps, unsigned int min_steps)
    {
        Timer t;
        this->min_steps = min_steps;
        this->max_steps = max_steps;
        // Keeps track of visited "Configurations". Because of the limitations of either having to turn after a certain number of steps
        // or not being able to turn for a certain amount of steps, it's possible that the shortest path to a tile in the map isn't
        // necesserily the best one, if it means that you're not able to use the shortest path to the next tile due to the direction you arrived from
        std::unordered_set<DijkstraState, HashDijkstraState> visited;
        std::priority_queue<DijkstraState, std::vector<DijkstraState>> dijkstraQueue = initDijkstraQueue();

        while (!dijkstraQueue.empty())
        {
            DijkstraState current = dijkstraQueue.top();
            dijkstraQueue.pop();

            if (current.x == input_map[0].size() - 1 && current.y == input_map.size() - 1 /*&& current.straight_steps >= min_steps*/)
            {
                return current.path_length;
            }

            if (visited.find(current) == visited.end())
            {
                visited.insert(current);
                for (const auto &dir : {Up, Right, Down, Left})
                {
                    auto new_state = getNewStateInTravelDir(current, dir, input_map);
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
    D17Solver solver(argv[1]);

    std::cout << "Part 1:\n";
    unsigned int p1 = solver.solve(3, 0);
    std::cout << "Answer: " << p1 << '\n';

    std::cout << "Part 2:\n";
    unsigned int p2 = solver.solve(10, 4);
    std::cout << "Answer: " << p2 << '\n';

    return 0;
}
