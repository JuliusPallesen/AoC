#include <algorithm>
#include <chrono>
#include <fstream>
#include <iostream>
#include <optional>
#include <sstream>
#include <unordered_map>
#include <vector>

class Timer {
public:
    Timer()
        : start(std::chrono::steady_clock::now())
    {
    }
    ~Timer()
    {
        std::cout << "time elapsed: " << std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - start).count()
                  << "ms" << std::endl;
    }

private:
    const std::chrono::steady_clock::time_point start;
};

struct State {
    std::string rest_puzzle;
    std::vector<uint64_t> rest_input;
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
            hash ^= std::hash<uint64_t> {}(value);
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
    return !(s.rest_puzzle.empty() && !s.rest_input.empty()) ? s : std::optional<State>();
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
        ret += dotState ? getAllPossibilities(*dotState, mep, false) : 0;
    }
    if (curr == '#' || curr == '?') {
        auto hashState = performHash(s, in);
        ret += hashState ? getAllPossibilities(*hashState, mep, true) : 0;
    }
    mep[s] = ret;
    return ret;
}

std::string duplicatePuzzleFiveTimes(const std::string& input)
{
    std::string ret;
    for (size_t i = 0; i < 5; i++) {
        ret += input + "?";
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
    uint64_t ans = 0;
    std::vector<std::string> puzzles;
    std::vector<std::vector<uint64_t>> inputs;
    Timer t;

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

    for (size_t i = 0; i < puzzles.size(); i++) {
        std::unordered_map<State, uint64_t, StateHash> mep;
        ans += getAllPossibilities({ puzzles[i], inputs[i] }, mep);
    }

    std::cout << ans << '\n';
    return 0;
}
