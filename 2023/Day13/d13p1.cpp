#include <algorithm>
#include <fstream>
#include <iostream>
#include <set>
#include <sstream>
#include <vector>

bool isVerticalReflection(const std::string& s, const int index)
{
    int bw = index - 1;
    int fw = index;
    while (bw >= 0 && fw < s.size()) {
        if (s[bw] != s[fw]) {
            return false;
        }
        bw--;
        fw++;
    }
    return true;
}

bool isHorizontalReflection(const std::vector<std::string>& input, const int col, const int index)
{
    auto bw = index - 1;
    auto fw = index;
    while (bw >= 0 && fw < input.size()) {
        if (input[bw][col] != input[fw][col]) {
            return false;
        }
        bw--;
        fw++;
    }
    return true;
}

int getHorizontalMirror(const std::vector<std::string>& input)
{
    std::set<int> horLines;
    std::set<int> removeLines;
    std::generate_n(std::inserter(horLines, horLines.begin()), input.size() - 1, [i = 1]() mutable { return i++; });
    for (size_t x = 0; x < input[0].size(); x++) {
        for (const auto& y : horLines) {
            if (!isHorizontalReflection(input, x, y)) {
                removeLines.insert(y);
            }
        }
        for (const auto line : removeLines) {
            horLines.erase(line); // remove non-candidates for next iteration
        }
    }
    return horLines.empty() ? 0 : *(horLines.begin());
}

int getVerticalMirror(const std::vector<std::string>& input)
{
    std::set<int> vertLines;
    std::set<int> removeLines;
    std::generate_n(std::inserter(vertLines, vertLines.begin()), input[0].size() - 1, [i = 1]() mutable { return i++; });
    for (auto&& str : input) {
        for (const auto& i : vertLines) {
            if (!isVerticalReflection(str, i)) {
                removeLines.insert(i);
            }
        }
        for (const auto line : removeLines) {
            vertLines.erase(line); // remove non-candidates for next iteration
        }
    }
    return vertLines.empty() ? 0 : *(vertLines.begin());
}

int main(int argc, char const* argv[])
{
    std::ifstream inputFile;
    inputFile.open(argv[1]);
    std::string line;
    int ans = 0;
    std::vector<std::vector<std::string>> inputs(1);
    while (std::getline(inputFile, line)) {
        if (!line.empty()) {
            inputs[inputs.size() - 1].push_back(line);
        } else {
            inputs.push_back(std::vector<std::string>());
        }
    }
    inputFile.close();

    for (auto&& input : inputs) {
        ans += getHorizontalMirror(input) * 100 + getVerticalMirror(input);
    }

    std::cout << ans << '\n';
    return 0;
}
