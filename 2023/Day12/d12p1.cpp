#include <algorithm>
#include <fstream>
#include <iostream>
#include <sstream>
#include <vector>

bool performDot(std::string& rest_puzzle, std::vector<int>& rest_input, bool in = false)
{
    if (in && rest_input[rest_input.size() - 1] > 0) {
        return false;
    }
    if (!rest_input.empty() && rest_input[rest_input.size() - 1] == 0) {
        rest_input.pop_back();
    }
    rest_puzzle.pop_back();
    return !(rest_puzzle.empty() && !rest_input.empty());
}

bool performHash(std::string& rest_puzzle, std::vector<int>& rest_input, bool in = false)
{
    if (rest_input.empty() || rest_input[rest_input.size() - 1] == 0) {
        return false;
    }
    rest_input[rest_input.size() - 1]--;
    rest_puzzle.pop_back();
    return true;
}

int getAllPossibilities(const std::string rest_puzzle, const std::vector<int> rest_input, bool in = false)
{
    if (rest_input.empty()) {
        return rest_puzzle.find('#') == std::string::npos ? 1 : 0;
    }
    if (rest_puzzle.empty()) {
        return (rest_input.size() == 1 && rest_input[0] == 0) ? 1 : 0;
    }

    const auto curr = rest_puzzle[rest_puzzle.size() - 1];
    int ret = 0;

    if (curr == '.' || curr == '?') {
        auto dot_rest_puzzle = rest_puzzle;
        auto dot_rest_input = rest_input;
        ret += performDot(dot_rest_puzzle, dot_rest_input, in) ? getAllPossibilities(std::move(dot_rest_puzzle), std::move(dot_rest_input), false) : 0;
    }
    if (curr == '#' || curr == '?') {
        auto hash_rest_puzzle = rest_puzzle;
        auto hash_rest_input = rest_input;
        ret += performHash(hash_rest_puzzle, hash_rest_input, in) ? getAllPossibilities(std::move(hash_rest_puzzle), std::move(hash_rest_input), true) : 0;
    }
    return ret;
}

int main(int argc, char const* argv[])
{
    std::ifstream inputFile;
    inputFile.open(argv[1]);
    std::string line;
    int ans;
    std::vector<std::string> puzzles;
    std::vector<std::vector<int>> inputs;
    while (std::getline(inputFile, line)) {
        const auto space_pos = line.find(' ');
        puzzles.push_back(line.substr(0, space_pos));
        const auto input_str = line.substr(space_pos + 1, line.size());
        inputs.push_back(std::vector<int>());
        std::stringstream ss(input_str);
        std::string str;
        while (std::getline(ss, str, ',')) {
            inputs[inputs.size() - 1].push_back(stoi(str));
        }
    }
    inputFile.close();

    for (size_t i = 0; i < puzzles.size(); i++) {
        auto val = getAllPossibilities(puzzles[i], inputs[i]);
        ans += val;
    }

    std::cout << ans;
    return 0;
}
