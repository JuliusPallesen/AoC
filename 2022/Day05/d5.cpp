#include <iostream>
#include <ranges>
#include <algorithm>
#include <fstream>
#include <sstream>
#include <vector>
#include <regex>
#include <format>

/*
    - Parse Bottom up
*/

struct MoveInstruction
{
    MoveInstruction(int c, int f, int t) : count(c), from(f), to(t) {}
    const int count;
    const int from;
    const int to;
};

int main(int argc, char const *argv[])
{
    std::fstream inputFile(argv[1]);
    std::string line;
    std::vector<std::string> stacks;
    std::vector<MoveInstruction> instructions;
    while (std::getline(inputFile, line) && !line.empty())
    {
        stacks.emplace_back(std::move(line));
    }
    const std::regex R("move (\\d+) from (\\d+) to (\\d+)");
    std::smatch matches;
    while (std::getline(inputFile, line))
    {
        std::regex_search(line, matches, R);
        instructions.emplace_back(std::stoi(matches[1]), std::stoi(matches[2]) - 1, std::stoi(matches[3]) - 1); // initializer list?
    }

    size_t stacks_width = stacks.at(0).size() / 3;
    std::vector<std::vector<char>> stack_mat(stacks_width, std::vector<char>());

    for (auto it = stacks.rbegin() + 1; it != stacks.rend(); it++)
    {
        size_t j = 0;
        for (size_t i = 1; i < it->size(); i += 4)
        {
            if (it->at(i) != ' ')
                stack_mat[j].push_back(it->at(i));
            j++;
        }
    }

    for (auto &&inst : instructions)
    {
        for (size_t i = stack_mat[inst.from].size() - 1;
             i < stack_mat[inst.from].size() && i >= stack_mat[inst.from].size() - inst.count;
             i--)
        {
            stack_mat[inst.to].push_back(stack_mat[inst.from][i]);
        }
        stack_mat[inst.from].resize(std::max(0, static_cast<int>(stack_mat[inst.from].size()) - inst.count));
    }

    std::ranges::for_each(stack_mat, [](const auto &s)
                          { std::cout << s[s.size() - 1]; });

    return 0;
}
