#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <queue>
#include <optional>
#include <algorithm>
#include <unordered_set>

enum Directions
{
    Up = 0,
    Right = 1,
    Down = 2,
    Left = 3
};

struct DijkstraState
{
    DijkstraState()
    {
        this->x = 0;
        this->y = 0;
        this->last_direction = Down;
        this->straight_steps = 1;
        this->path_length = 0;
    }

    bool operator==(const DijkstraState &other) const
    {
        return x == other.x && y == other.y && last_direction == other.last_direction && straight_steps == other.straight_steps;
    }

    int x;
    int y;
    int path_length;
    Directions last_direction;
    int straight_steps;
};

/*
Retrieves puzzle input from an inputfilepath and returns
*/
std::vector<std::vector<int>> retrievePuzzleInput(std::string inputfilepath)
{
    std::vector<std::vector<int>> ret;
    std::ifstream inputFile;
    std::string line;
    inputFile.open(inputfilepath);

    int i = 0;
    while (std::getline(inputFile, line))
    {
        ret.push_back(std::vector<int>());
        for (const auto &c : line)
        {
            ret[i].push_back(c - '0');
        }
        ++i;
    }
    inputFile.close();
    return ret;
}

/*
Keeps track of the amount of steps taken in a straight line
*/
void calcStraightStepsForNewState(const DijkstraState &previous, DijkstraState &new_state)
{
    if (previous.last_direction == new_state.last_direction)
    {
        new_state.straight_steps = previous.straight_steps + 1;
    }
    else
    {
        new_state.straight_steps = 1;
    }
}

/*
Updates a states position in the direction of travel
*/
void changePositionByDirection(DijkstraState &state)
{
    switch (state.last_direction)
    {
    case Up:
        state.y = state.y - 1;
        break;
    case Left:
        state.x = state.x - 1;
        break;
    case Down:
        state.y = state.y + 1;
        break;
    case Right:
        state.x = state.x + 1;
        break;
    default:
        break;
    }
}

/*
Checks if a Position is inside of a 2D vector
*/
bool checkInBounds2D(int x, int y, const std::vector<std::vector<int>> &vec)
{
    return x >= 0 &&
           y >= 0 &&
           x < vec[0].size() &&
           y < vec.size();
}

/*
Checks if 2 directions of travel cancel eachother out (e.g. reversing)
*/
bool notReversing(Directions dir1, Directions dir2)
{
    return !(static_cast<int>(dir1) == ((static_cast<int>(dir2) + 2) % 4));
}

/*
Try to get an updated state in the direction of travel. If reversing, out of bounds or too many straight steps don't return anything.
*/
std::optional<DijkstraState> getNewStateInTravelDir(const DijkstraState &previous, Directions direction, const std::vector<std::vector<int>> &map)
{
    DijkstraState new_state;
    new_state.last_direction = direction;
    new_state.x = previous.x;
    new_state.y = previous.y;

    calcStraightStepsForNewState(previous, new_state);
    changePositionByDirection(new_state);

    if (checkInBounds2D(new_state.x, new_state.y, map) &&
        notReversing(previous.last_direction, direction) &&
        new_state.straight_steps <= 3)
    {
        new_state.path_length = previous.path_length + map[new_state.y][new_state.x];
        return new_state;
    }
    return {};
}
/*
Used for prettyprinting / debugging
*/
char getDirectionChar(Directions dir)
{
    switch (dir)
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

/*
Prints a Dijkstrastate
*/
void printState(const DijkstraState &s)
{
    std::cout << "\n";
    std::cout << "Path length: " << s.path_length << "\n";
    std::cout << "Position: " << s.x << "," << s.y << "\n";
    std::cout << "Direction: " << getDirectionChar(s.last_direction) << "\n";
    std::cout << "Straight steps: " << s.straight_steps << "\n";
}

/*
Used to sort Dijkstrastates by their path length in the priority queue (needs to be a struct to be passed as template parameter)
*/
struct stateCmp
{
    bool operator()(DijkstraState const &state1, DijkstraState const &state2)
    {
        return state1.path_length > state2.path_length;
    }
};

/*
Initializes a Proiority queue for the dijkstra algorithm
*/
std::priority_queue<DijkstraState, std::vector<DijkstraState>, stateCmp> initPrioQueue()
{
    std::priority_queue<DijkstraState, std::vector<DijkstraState>, stateCmp> pq;
    DijkstraState start;
    pq.emplace(start);
    return pq;
}

/*
Used to hash Dijkstra states to keep track of "visited" configurations
*/
class HashDijkstra
{
public:
    size_t operator()(const DijkstraState &step) const
    {
        return (step.x << 16) | (step.y << 8) | (static_cast<int>(step.last_direction) << 4) | step.straight_steps;
    }
};

unsigned int getMinimalHeatloss(const std::vector<std::vector<int>> &map)
{
    std::unordered_set<DijkstraState, HashDijkstra> visited;
    std::priority_queue<DijkstraState, std::vector<DijkstraState>, stateCmp> dijkstraQueue = initPrioQueue();
    const Directions directions[] = {Up, Right, Down, Left};

    while (!dijkstraQueue.empty())
    {
        DijkstraState current = dijkstraQueue.top();
        dijkstraQueue.pop();

        if (current.x == map[0].size() - 1 && current.y == map.size() - 1)
        {
            return current.path_length;
        }

        if (visited.find(current) == visited.end())
        {
            visited.insert(current);
            for (const auto &dir : directions)
            {
                auto new_state = getNewStateInTravelDir(current, dir, map);
                if (new_state)
                {
                    dijkstraQueue.emplace(*new_state);
                }
            }
        }
    }
    return INT_MAX;
}

int main(int argc, char const *argv[])
{
    std::vector<std::vector<int>> map;
    map = retrievePuzzleInput(argv[1]);

    std::cout << getMinimalHeatloss(map);
    return 0;
}
