#include <vector>
#include <ranges>
#include <string>
#include <algorithm>
#include <functional>
#include <fstream>
#include <iostream>
#include <format>

constexpr int ITEMS_INDEX = sizeof("  Starting items:") - 1;
constexpr int OP_INDEX = sizeof("  Operation: new = ") - 1;
constexpr int TEST_INDEX = sizeof("  Test: divisible by ") - 1;
constexpr int TRUE_INDEX = sizeof("    If true: throw to monkey ") - 1;
constexpr int FALSE_INDEX = sizeof("    If false: throw to monkey ") - 1;

struct Monkey
{
    std::vector<int> m_items;
    std::function<int(int)> m_op_func;
    std::function<int(int)> m_next_func;
    int m_touches = 0;

    Monkey(const std::vector<std::string> &inputlines) : m_items(std::move(parseItems(inputlines[0]))),
                                                         m_op_func(std::move(parseOp(inputlines[1]))),
                                                         m_next_func(std::move(parseFunction(inputlines | std::ranges::views::drop(2) | std::ranges::to<std::vector<std::string>>()))),
                                                         m_touches(0)
    {
    }

    void inspect_items(std::vector<Monkey> &others)
    {
        for (const auto &item : m_items)
        {
            m_touches++;
            int worry = m_op_func(item);
            others.at(m_next_func(worry)).m_items.push_back(worry);
        }
        m_items.clear();
    }

private:
    std::vector<int> parseItems(const std::string &input)
    {
        // clang-format off
        return input.substr(ITEMS_INDEX) 
            | std::ranges::views::split(',') 
            | std::ranges::views::transform([](const auto &s)
                { return std::stoi(std::string(s.begin(), s.end())); }) 
            | std::ranges::to<std::vector<int>>();
        // clang-format on
    }

    std::function<int(int)> parseOp(const std::string &str)
    {
        const auto opstr = str.substr(OP_INDEX);
        const auto lstr = opstr.substr(0, opstr.find(' '));
        const auto rstr = opstr.substr(opstr.find_last_of(' ') + 1);
        const auto opchar = opstr.at(opstr.find(' ') + 1);
        return [=](int a)
        {
            const auto lhs = std::isdigit(lstr[0]) ? std::stoi(lstr) : a;
            const auto rhs = std::isdigit(rstr[0]) ? std::stoi(rstr) : a;
            switch (opchar)
            {
            case '*':
                return (lhs * rhs) / 3;
            case '+':
                return (lhs + rhs) / 3;
            default:
                std::cerr << "not a valid operand\n";
                exit(1);
            }
        };
    }

    std::function<int(int)> parseFunction(const std::vector<std::string> &input)
    {
        const int next_true = std::stoi(input[1].substr(TRUE_INDEX));
        const int next_false = std::stoi(input[2].substr(FALSE_INDEX));
        const int divisor = std::stoi(input[0].substr(TEST_INDEX));
        return [=](int a)
        { return (a % divisor == 0) ? next_true : next_false; };
    }
};

// Cool new functional style ranges parsing
auto parseMonkeys(std::string fp)
{
    // clang-format off
    std::ifstream inputFile;
    inputFile.open(fp);
    const auto monkeys = std::string(std::istreambuf_iterator<char>(inputFile), std::istreambuf_iterator<char>()) 
    | std::ranges::views::split('\n') 
    | std::ranges::views::transform([](const auto &s)
        { return std::string(s.begin(), s.end()); }) 
    | std::ranges::views::split(std::string("")) 
    | std::ranges::views::transform([](auto &&subrange)
        { return Monkey(subrange 
                        | std::ranges::views::drop(1) 
                        | std::ranges::to<std::vector<std::string>>()); }) 
    | std::ranges::to<std::vector<Monkey>>();
    // clang-format on
    inputFile.close();
    return monkeys;
}

int main(int argc, char const *argv[])
{
    auto monkeys = parseMonkeys(argv[1]);

    /* future twoliner
    for(auto && [_,m] : std::ranges::views::cartesian_product(std::ranges::views::iota(0,20), monkeys)) {
        m.inspect_items(monkeys);
    }

    */
    for (auto &&i : std::ranges::views::iota(0, 20))
    {
        for (auto &&m : monkeys)
        {
            m.inspect_items(monkeys);
        }
    }
    std::sort(monkeys.begin(), monkeys.end(), [](const auto &m1, const auto &m2)
              { return m1.m_touches > m2.m_touches; });

    std::cout << monkeys[0].m_touches * monkeys[1].m_touches;
}