#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <queue>
#include <optional>

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
        this->position = {0, 0};
        this->last_direction = Down;
        this->straight_steps = 1;
        this->path_length = 0;
    }

    std::pair<int, int> position;
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
    int i = 0;
    std::ifstream inputFile;
    inputFile.open(inputfilepath);
    std::string line;
    while (std::getline(inputFile, line))
    {
        ret.push_back(std::vector<int>());
        for (const auto &c : line)
        {
            ret[i].push_back(c - '0'); // convert to int
        }
        i++;
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
        state.position = {state.position.first, state.position.second + 1};
        break;
    case Left:
        state.position = {state.position.first - 1, state.position.second};
        break;
    case Down:
        state.position = {state.position.first, state.position.second - 1};
        break;
    case Right:
        state.position = {state.position.first + 1, state.position.second};
        break;
    default:
        break;
    }
}

/*
Checks if a Position is inside of a 2D vector
*/
bool checkInBounds2D(const std::pair<int, int> position, const std::vector<std::vector<int>> &vec)
{
    return position.first >= 0 &&
           position.second >= 0 &&
           position.first < vec[0].size() &&
           position.second < vec.size();
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
    new_state.position = previous.position;
    calcStraightStepsForNewState(previous, new_state);
    changePositionByDirection(new_state);

    if (checkInBounds2D(new_state.position, map) &&
        notReversing(previous.last_direction, direction) &&
        new_state.straight_steps < 4)
    {
        new_state.path_length = previous.path_length + map[new_state.position.second][new_state.position.first];
        return new_state;
    }
    return {};
}

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

void printState(const DijkstraState &s)
{
    std::cout << "\n";
    std::cout << "Path length: " << s.path_length << "\n";
    std::cout << "Position: " << s.position.first << "," << s.position.second << "\n";
    std::cout << "Direction: " << getDirectionChar(s.last_direction) << "\n";
    std::cout << "Straight steps: " << s.straight_steps << "\n";
}

int main(int argc, char const *argv[])
{

    std::vector<std::vector<int>> map;
    map = retrievePuzzleInput(argv[1]);
    std::vector<std::vector<std::vector<int>>> visitedfrom(map.size(), std::vector<std::vector<int>>(map[0].size(), std::vector<int>(4, INT_MAX)));

    const auto stateCmp = [](const DijkstraState &state1, const DijkstraState &state2)
    {
        return state1.path_length >= state2.path_length;
    };
    std::priority_queue<DijkstraState, std::vector<DijkstraState>, decltype(stateCmp)> dijkstraQueue{stateCmp};

    DijkstraState start;
    start.path_length = map[0][0];
    dijkstraQueue.emplace(start);

    const Directions directions[] = {Up, Right, Down, Left};
    bool found = false;

    while (!dijkstraQueue.empty() && !found)
    {
        DijkstraState current = dijkstraQueue.top();
        dijkstraQueue.pop();
        if (visitedfrom[current.position.second][current.position.first][current.last_direction] >= current.path_length)
        {
            visitedfrom[current.position.second][current.position.first][current.last_direction] = current.path_length;
            for (const auto &dir : directions)
            {
                auto new_state = getNewStateInTravelDir(current, dir, map);
                if (new_state)
                {
                    if (new_state->position.first == map[0].size() - 1 && new_state->position.second == map.size() - 1)
                    {
                        found = true;
                        std::cout << new_state->path_length << std::endl;
                    }
                    dijkstraQueue.emplace(*new_state);
                }
            }
        }
    }
    return 0;
}
