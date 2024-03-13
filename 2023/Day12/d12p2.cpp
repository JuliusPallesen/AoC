#include <algorithm>
#include <fstream>
#include <iostream>
#include <optional>
#include <sstream>
#include <unordered_map>
#include <vector>

struct State {
    std::string rest_puzzle;
    std::vector<uint64_t> rest_input;
    State(std::string p, std::vector<uint64_t> i)
        : rest_puzzle(p)
        , rest_input(i)
    {
    }
    bool operator==(const State& other) const
    {
        return rest_input == other.rest_input && rest_puzzle == other.rest_puzzle;
    }
};

struct StateHash {
    std::size_t operator()(const State& state) const
    {
        std::size_t hash = std::hash<std::string> {}(state.rest_puzzle);
        for (uint64_t value : state.rest_input) {
            hash ^= std::hash<uint64_t> {}(value) + 0x9e3779b9 + (hash << 6) + (hash >> 2);
        }
        return hash;
    }
};

std::optional<State> performDot(State s, bool in = false)
{
    if (in && s.rest_input[s.rest_input.size() - 1] > 0) {
        return {};
    }
    if (!s.rest_input.empty() && s.rest_input[s.rest_input.size() - 1] == 0) {
        s.rest_input.pop_back();
    }
    s.rest_puzzle.pop_back();
    if (!(s.rest_puzzle.empty() && !s.rest_input.empty())) {
        return s;
    } else {
        return {};
    }
}

std::optional<State> performHash(State s, bool in = false)
{
    if (s.rest_input.empty() || s.rest_input[s.rest_input.size() - 1] == 0) {
        return {};
    }
    s.rest_input[s.rest_input.size() - 1]--;
    s.rest_puzzle.pop_back();
    return s;
}

uint64_t getAllPossibilities(State s,
    std::unordered_map<State, uint64_t, StateHash>& mep,
    bool in = false)
{
    if (s.rest_input.empty()) {
        return s.rest_puzzle.find('#') == std::string::npos;
    }
    if (s.rest_puzzle.empty()) {
        return s.rest_input.size() == 1 && s.rest_input[0] == 0;
    }

    if (mep.count(s) > 0) {
        return mep[s];
    }

    const auto curr = s.rest_puzzle[s.rest_puzzle.size() - 1];
    uint64_t ret = 0;

    if (curr == '.' || curr == '?') {
        auto dotState = performDot(s, in);
        auto val = dotState ? getAllPossibilities(*dotState, mep, false) : 0;
        mep[*dotState] = val;
        ret += val;
    }
    if (curr == '#' || curr == '?') {
        auto hashState = performHash(s, in);
        auto val = hashState ? getAllPossibilities(*hashState, mep, true) : 0;
        mep[*hashState] = val;
        ret += val;
    }
    mep[s] = ret;
    return ret;
}

std::string duplicatePuzzleFiveTimes(const std::string& input)
{
    std::string ret;
    for (size_t i = 0; i < 5; i++) {
        ret += input;
        ret += "?";
    }
    ret.pop_back();
    return ret;
}

std::vector<uint64_t> duplicateInputFiveTimes(const std::vector<uint64_t>& input)
{
    std::vector<uint64_t> ret;
    for (size_t i = 0; i < 5; i++) {
        ret.insert(ret.end(), input.begin(), input.end());
    }
    return ret;
}

int main(int argc, char const* argv[])
{
    std::ifstream inputFile;
    inputFile.open(argv[1]);
    std::string line;
    uint64_t ans;
    std::vector<std::string> puzzles;
    std::vector<std::vector<uint64_t>> inputs;
    while (std::getline(inputFile, line)) {
        const auto space_pos = line.find(' ');
        puzzles.push_back(duplicatePuzzleFiveTimes(line.substr(0, space_pos)));
        const auto input_str = line.substr(space_pos + 1, line.size());
        std::stringstream ss(input_str);
        std::string str;
        std::vector<uint64_t> inpt;
        while (std::getline(ss, str, ',')) {
            inpt.push_back(stoi(str));
        }
        inputs.push_back(duplicateInputFiveTimes(inpt));
    }
    inputFile.close();
    ans = 0;
    for (size_t i = 0; i < puzzles.size(); i++) {
        std::cout << i << '/' << puzzles.size() << '\n';
        std::unordered_map<State, uint64_t, StateHash> mep;
        auto val = getAllPossibilities({ puzzles[i], inputs[i] }, mep);
        std::cout << val << '\n';
        ans += val;
    }

    std::cout << ans;
    return 0;
}
