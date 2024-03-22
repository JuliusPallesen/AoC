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
    return (garden[s.y % garden.size()][s.x % garden[0].size()] != '#');
}

/*
Garden is infinite now.
Steps are very large now.

BFS wont work -> input too large
Idea for infinite grid -> take indices modulo

something with determining oscillations between odd and even steps?

- possibilities grow in diamon shape
    - are there reocurring patterns which always expand in the same way?
- can we determine once the amount of visited tiles in a garden seciton is stagnant?
    - eliminate those

- count how long it takes to fill?
    - how would you even detect that?
    - count amount of occurring States with the same Step ?

- the Start point is the only non-"." on both the x and y axis
    - reaching

https://zuhlke-my.sharepoint.com/:wb:/p/julius_pallesen/EQjXpblFt4xGrUxd4x5VdwgB0iYQemQQLgC9ot2HKTVADw?email=julius.pallesen%40zuhlke.com&e=icJeW9


how to set S to 0,0?
    - we want to set S to 0,0 so we have less possibilities to worry about.
      ^             /\
     / \    ==>    /  \
    <   >         /    \


gNew[y][x] = g[(y + s.y) % width][(x + s.x) % width]?

gardenSize = input.width // doesnt matter if height or width, it's square
Rest = N % gardenSize
FullGardens = N / gardenSize

Step 1:
    Calculate the maximum fill of a square without the center.
        - check if input is even or odd
        - start from center
        - remember the amounts of steps with the same steps_left at even / odd
        - once it doesnt change return
        - THIS SEEMS BAD!

        ans += fullArea * b²
Step 2:
    for start each in {{0,0}, {0,gardenSize}, {gardenSize,0}, {gardenSize, gardenSize}}
        ans += getArea() * (FullGardens+1)

*/

class ElfGarden
{
public:
    ElfGarden(const std::string &inputFilePath)
    {
        getGardenFromFilePath(inputFilePath);
        m_size = m_garden.size();
    }

    uint64_t getPossibleEndPositionsAmount(size_t N)
    {
        return 0;
    }

    friend std::ostream &operator<<(std::ostream &out, const ElfGarden &other)
    {
        for (auto &&str : other.m_garden)
        {
            out << str << "\n";
        }
        return out;
    }

private:
    Garden m_garden;
    size_t m_size;
    size_t m_full_gardens_on_axis_amount;
    size_t m_rest;

    void getGardenFromFilePath(const std::string &inputFilePath)
    {
        std::ifstream inputFile;
        inputFile.open(inputFilePath);
        std::string line;
        while (std::getline(inputFile, line))
        {
            m_garden.emplace_back(std::move(line));
        }
        inputFile.close();
    }

    void centerGarden()
    {
        Garden newGarden(m_size, std::string(m_size, ' '));
        const size_t half = m_size / 2;
        for (size_t y = 0; y < m_size; y++)
        {
            for (size_t x = 0; x < m_size; x++)
            {
                newGarden[y][x] = m_garden[(y + half) % m_size][(x + half) % m_size];
            }
        }
    }
};

uint64_t getPossibleLocationAmount(Garden &garden, const State start)
{
    uint64_t ans = 0;
    std::unordered_map<State, bool, HashState> reached;
    std::queue<State> nextSteps;
    nextSteps.push(start);
    while (!nextSteps.empty())
    {
        State currState = nextSteps.front();
        nextSteps.pop();
        reached[currState] = true;

        if (currState.steps_left == 0)
        {
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
    return ans;
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
    int steps = 500;
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

    State start(garden.size() / 2, garden.size() / 2, 64);
    std::cout << getPossibleLocationAmount(garden, start) << "\n";
    return 0;
}
