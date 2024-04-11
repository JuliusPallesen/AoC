#include <string>
#include <fstream>
#include <algorithm>
#include <array>
#include <iostream>
#include <ranges>
#include <vector>
#include <variant>
#include <sstream>
#include <set>
#include <unordered_map>
#include <memory>
#include <numeric>
#include <regex>
#include <stack>

class Day7
{
private:
    const uint32_t MAX_VAL = 100000, STORAGE_SPACE = 70000000, UNUSED_SPACE_NEEDED = 30000000;

    template <typename... Ts> // https://www.youtube.com/watch?v=eD-ceG-oByA&list=LL&index=2
    struct overload : Ts...
    {
        using Ts::operator()...;
    };

    struct Directory;
    using Tree = std::variant<std::unique_ptr<Directory>, int>;
    struct Directory
    {
        std::vector<Tree> contents;
    };

    std::unique_ptr<Directory> parseInput(std::fstream &input)
    {
        std::string line;
        std::unique_ptr<Directory> root;
        std::stack<Directory *> history;
        const std::regex CD_REGEX("^\\$ cd .*$");
        const std::regex FILE_REGEX("^\\d+ .*$");

        while (std::getline(input, line))
        {
            std::smatch match;
            if (std::regex_match(line, match, CD_REGEX))
            {
                if (match[1] == "..")
                {
                    history.pop();
                }
                else
                {
                    if (!history.empty())
                    {
                        history.top()->contents.emplace_back(std::make_unique<Directory>());
                        history.push(std::get<std::unique_ptr<Directory>>(history.top()->contents.back()).get());
                    }
                    else
                    {
                        history.push(root.get());
                    }
                }
            }
            else if (std::regex_match(line, match, FILE_REGEX))
            {
                std::cout << match[1];
                history.top()->contents.push_back(std::stoi(match[1]));
            }
        }
        return root;
    }

public:
    std::unique_ptr<Directory> m_root;
    Day7(std::fstream &input) : m_root(std::move(parseInput(input))) {}

    int getSize(Tree &t) const
    {
        // remember to return to this once GCC14 or Clang18 are supported for windows 😂 deducing this is still not supported
        return std::visit(overload{[](this auto &&self, const std::unique_ptr<Directory> &d) -> int
                                   {
                                       return std::accumulate(d->contents.begin(), d->contents.end(), 0, [&](const auto &n)
                                                              { return std::visit(self, n); }); // recursive lambda
                                   },
                                   [](const int i) -> int
                                   { return i; }},
                          t);
    };
};

int main(int argc, char const *argv[])
{
    std::fstream inputFile(argv[1]);
    Day7 solver(inputFile);

    return EXIT_SUCCESS;
}
