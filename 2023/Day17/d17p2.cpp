#include <algorithm>
#include <fstream>
#include <iostream>
#include <optional>
#include <queue>
#include <sstream>
#include <unordered_set>
#include <vector>

/*
Refector ideas

Unify both parts into one solution
add conditions to the dijkstraSolver

class D17Solver {
    private:
    std::vector<std::function<(DijkstraState,DijkstraState)->bool>> conditions
    std::vector<std::vector<unsigned int> input_map;

    Class Dijkstrastate : friend D17sovler {

    }

    public:
    D17Solver(std::string inputfilepath) {

    }

    addCondition(std::function<(DijkstraState,DijkstraState)->bool>)

}



*/

enum Directions {
    Up = 0,
    Right = 1,
    Down = 2,
    Left = 3,
    None = 4
};

struct DijkstraState {
    int x;
    int y;
    int path_length;
    Directions last_direction;
    int straight_steps;

    DijkstraState()
    {
        this->x = 0;
        this->y = 0;
        this->last_direction = None;
        this->straight_steps = 0;
        this->path_length = 0;
    }

    bool operator==(const DijkstraState& other) const
    {
        return x == other.x && y == other.y && last_direction == other.last_direction && straight_steps == other.straight_steps;
    }

    bool operator>(DijkstraState const& other) const
    {
        return this->path_length < other.path_length;
    }

    bool operator<(DijkstraState const& other) const
    {
        return this->path_length > other.path_length;
    }
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

    while (std::getline(inputFile, line)) {
        ret.push_back(std::vector<int>(line.size()));
        std::transform(line.begin(), line.end(), ret[ret.size() - 1].begin(), [](char c) { return c - '0'; });
    }

    inputFile.close();
    return ret;
}

/*
Keeps track of the amount of steps taken in a straight line
*/
void calcStraightStepsForNewState(const DijkstraState& previous, DijkstraState& new_state)
{
    if (previous.last_direction == new_state.last_direction) {
        new_state.straight_steps = previous.straight_steps + 1;
    } else {
        new_state.straight_steps = 1;
    }
}

/*
Updates a states position in the direction of travel

TODO:
- set init position from previous in here
*/
void changePositionByDirection(DijkstraState& state)
{
    switch (state.last_direction) {
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
bool checkInBounds2D(int x, int y, const std::vector<std::vector<int>>& vec)
{
    return x >= 0 && y >= 0 && x < vec[0].size() && y < vec.size();
}

/*
Checks if 2 directions of travel cancel eachother out (e.g. reversing)
*/
bool notReversing(Directions dir1, Directions dir2)
{
    return !(static_cast<int>(dir1) == ((static_cast<int>(dir2) + 2) % 4));
}

bool validTurn(const DijkstraState& previous, const DijkstraState& current)
{
    if (previous.last_direction == None || previous.last_direction == current.last_direction) {
        return true;
    } else {
        return previous.straight_steps >= 4;
    }
}

/*
Try to get an updated state in the direction of travel. If reversing, out of bounds or too many straight steps don't return anything.
TODO:
- remove magic numbers
- Maybe unify part1 and part2
    - could recieve a list of conditions that have to be met
- im not sure if i like the "sub" functions with varying sets of parameters when all of them
    serve a similar purpose
*/
std::optional<DijkstraState> getNewStateInTravelDir(const DijkstraState& previous, Directions direction, const std::vector<std::vector<int>>& map)
{
    DijkstraState new_state;
    new_state.last_direction = direction;
    new_state.x = previous.x;
    new_state.y = previous.y;

    calcStraightStepsForNewState(previous, new_state);
    changePositionByDirection(new_state);

    if (checkInBounds2D(new_state.x, new_state.y, map) && notReversing(previous.last_direction, direction) && validTurn(previous, new_state) && new_state.straight_steps <= 10) {
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
    switch (dir) {
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
void printState(const DijkstraState& s)
{
    std::cout << "\n";
    std::cout << "Path length: " << s.path_length << "\n";
    std::cout << "Position: " << s.x << "," << s.y << "\n";
    std::cout << "Direction: " << getDirectionChar(s.last_direction) << "\n";
    std::cout << "Straight steps: " << s.straight_steps << "\n";
}

/*
Initializes a Proiority queue for the dijkstra algorithm
*/
std::priority_queue<DijkstraState, std::vector<DijkstraState>> initPrioQueue()
{
    std::priority_queue<DijkstraState, std::vector<DijkstraState>> pq;
    DijkstraState start;
    pq.emplace(start);
    return pq;
}

/*
Used to hash Dijkstra states to keep track of "visited" configurations
TODO:
- Check if possible to move to struct itself?
- Check how unordered_set works. is this efficient?
- we already implemented < and > what is the performance impact if we make it a normal (ordered) set
*/
class HashDijkstraState {
public:
    size_t operator()(const DijkstraState& step) const
    {
        return (step.x << 16) | (step.y << 8) | (static_cast<int>(step.last_direction) << 4) | step.straight_steps;
    }
};

/*
Performs Dijkstras algorithm (with restrictions) on the input.
priority queue is used to keep track of which configurations / states should be expanded next.
*/
unsigned int getMinimalHeatloss(const std::vector<std::vector<int>>& map)
{
    // Keeps track of visited "Configurations". Because of the limitations of either having to turn after a certain number of steps
    // or not being able to turn for a certain amount of steps, it's possible that the shortest path to a tile in the map isn't
    // necesserily the best one, if it means that you're not able to use the shortest path to the next tile due to the direction you arrived from
    std::unordered_set<DijkstraState, HashDijkstraState> visited;
    std::priority_queue<DijkstraState, std::vector<DijkstraState>> dijkstraQueue = initPrioQueue();

    while (!dijkstraQueue.empty()) {
        DijkstraState current = dijkstraQueue.top();
        dijkstraQueue.pop();
        if (current.x == map[0].size() - 1 && current.y == map.size() - 1 && current.straight_steps >= 4) {
            return current.path_length;
        }

        if (visited.find(current) == visited.end()) {
            visited.insert(current);
            for (const auto& dir : { Up, Right, Down, Left }) {
                auto new_state = getNewStateInTravelDir(current, dir, map);
                if (new_state) {
                    dijkstraQueue.emplace(*new_state);
                }
            }
        }
    }
    return INT_MAX;
}

int main(int argc, char const* argv[])
{
    std::vector<std::vector<int>> map;
    map = retrievePuzzleInput(argv[1]);

    std::cout << getMinimalHeatloss(map) << std::endl;
    return 0;
}
