#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <algorithm>
#include <unordered_map>
#include <queue>

using Garden = std::vector<std::string>;

struct State
{
    int y;
    int x;
    int steps_left;

    State(int ypos = 0, int xpos = 0, int steps = 0) : y(ypos), x(xpos), steps_left(steps) {}

    bool operator==(const State &other) const
    {
        return x == other.x && y == other.y && steps_left == other.steps_left;
    }
};

class HashState
{
public:
    size_t operator()(const State &state) const
    {
        return (static_cast<uint64_t>(state.y) << 32) | (static_cast<uint64_t>(state.x) << 16) | state.steps_left;
    }
};

std::vector<State> genNextStates(const State &state)
{
    const std::vector<std::pair<int, int>> dirs = {{-1, 0}, {1, 0}, {0, -1}, {0, 1}};
    std::vector<State> ret;
    for (auto &&d : dirs)
    {
        ret.push_back(State(state.y + d.first, state.x + d.second, state.steps_left - 1));
    }
    return ret;
}

bool isValidStep(const State &s, const Garden &garden)
{
    return (s.steps_left >= 0 && s.x >= 0 && s.y >= 0 &&
            s.x < garden[0].size() && s.y < garden.size() &&
            garden[s.y][s.x] != '#');
}

int main(int argc, char const *argv[])
{
    std::ifstream inputFile;
    inputFile.open(argv[1]);
    std::string line;
    uint64_t ans = 0;
    Garden garden;
    std::unordered_map<State, bool, HashState> reached;
    std::queue<State> nextSteps;
    int steps = 64;
    int y = 0;
    while (std::getline(inputFile, line))
    {
        garden.emplace_back(std::move(line));
        const auto start_x_pos = garden[garden.size() - 1].find('S');
        if (start_x_pos != garden[garden.size() - 1].npos)
        {
            garden[y][start_x_pos] = '.';
            nextSteps.emplace(y, static_cast<int>(start_x_pos), steps);
        }
        y++;
    }

    inputFile.close();
    while (!nextSteps.empty())
    {
        State currState = nextSteps.front();
        nextSteps.pop();
        reached[currState] = true;

        if (currState.steps_left == 0)
        {
            garden[currState.y][currState.x] = 'O';
            ans++;
        }
        else
        {
            const auto newStates = genNextStates(currState);
            for (auto &&s : newStates)
            {
                if (isValidStep(s, garden) && reached.count(s) == 0)
                {
                    reached[s] = true;
                    nextSteps.push(s);
                }
            }
        }
    }

    std::cout << ans << "\n";
    return 0;
}
