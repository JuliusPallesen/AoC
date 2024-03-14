#include <algorithm>
#include <fstream>
#include <iostream>
#include <set>
#include <sstream>
#include <vector>

int countVerticalReflectionSmidges(const std::string& s, const int index)
{
    int ret = 0;
    int bw = index - 1;
    int fw = index;
    while (bw >= 0 && fw < s.size()) {
        if (s[bw] != s[fw]) {
            ret++;
        }
        bw--;
        fw++;
    }
    return ret;
}

bool countHorizontalReflectionSmidges(const std::vector<std::string>& input, const int col, const int index)
{
    int ret = 0;
    int bw = index - 1;
    int fw = index;
    while (bw >= 0 && fw < input.size()) {
        if (input[bw][col] != input[fw][col]) {
            ret++;
        }
        bw--;
        fw++;
    }
    return ret;
}

int getHorizontalMirror(const std::vector<std::string>& input)
{
    std::vector<int> smudges(input.size());
    for (size_t x = 0; x < input[0].size(); x++) {
        for (int y = 0; y < smudges.size(); y++) {
            smudges[y] += (countHorizontalReflectionSmidges(input, x, y));
        }
    }
    auto it = std::find(smudges.begin(), smudges.end(), 1);
    return (it != smudges.end()) ? std::distance(smudges.begin(), it) : 0;
}

int getVerticalMirror(const std::vector<std::string>& input)
{
    std::vector<int> smudges(input[0].size());
    for (auto&& str : input) {
        for (int i = 0; i < smudges.size(); i++) {
            smudges[i] += countVerticalReflectionSmidges(str, i);
        }
    }
    auto it = std::find(smudges.begin(), smudges.end(), 1);
    return (it != smudges.end()) ? std::distance(smudges.begin(), it) : 0;
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
