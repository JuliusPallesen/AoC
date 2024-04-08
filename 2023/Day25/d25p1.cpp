#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <algorithm>
#include <unordered_map>
#include <unordered_set>
#include <numeric>
#include <ranges>
#include <format>

using Nodes = std::unordered_set<std::string>;
using StringGraphMap = std::unordered_map<std::string, Nodes>;

class PComp
{
public:
    bool operator()(const std::pair<int, int> &lhs, const std::pair<int, int> &rhs) const
    {
        return lhs.first > rhs.first;
    }
};

// https://en.wikipedia.org/wiki/Stoer%E2%80%93Wagner_algorithm
std::pair<int, std::vector<int>> globalMinCut(std::vector<std::vector<int>> mat)
{
    std::pair<int, std::vector<int>> best = {INT_MAX, {}};
    std::vector<std::vector<int>> co;
    std::ranges::generate_n(std::back_inserter(co), mat.size(), [i = 0]() mutable
                            { return std::vector<int>({i++}); });

    for (int ph = 1; ph < mat.size(); ph++)
    {
        std::vector<int> w = mat[0]; // no clue how to implement this as prio queue, looks like it would have to be mutable
        size_t s = 0, t = 0;
        for (int it = 0; it < mat.size() - ph; it++)
        {
            w[t] = INT_MIN;
            s = t;
            t = std::max_element(w.begin(), w.end()) - w.begin();
            for (int i = 0; i < mat.size(); i++)
                w[i] += mat[t][i];
        }
        best = std::min(best, {w[t] - mat[t][t], co[t]});
        co[s].insert(co[s].end(), co[t].begin(), co[t].end());

        for (int i = 0; i < mat.size(); i++)
            mat[s][i] += mat[t][i];

        for (int i = 0; i < mat.size(); i++)
            mat[i][s] = mat[s][i];

        mat[0][t] = INT_MIN;
    }

    return best;
}

int getId(std::unordered_map<std::string, int> &names, const std::string &name)
{
    if (names.count(name) == 0)
        names[name] = names.size();
    return names[name];
}

int main(int argc, char const *argv[])
{
    std::ifstream inputFile;
    inputFile.open(argv[1]);
    std::string line;
    StringGraphMap mep;
    std::unordered_map<std::string, int> names;
    int ans = 0;
    while (std::getline(inputFile, line))
    {
        std::string name = line.substr(0, line.find(':'));
        std::stringstream ss(line.substr(line.find(':') + 1));
        std::string tgt;
        while (ss >> tgt)
        {
            mep[tgt].insert(name);
            mep[name].emplace(std::move(tgt));
        }
    }
    inputFile.close();

    std::vector<std::vector<int>> mat(mep.size(), std::vector<int>(mep.size(), 0));
    for (const auto &[name, targets] : mep)
    {
        for (const auto &t : targets)
        {
            int i = getId(names, name);
            int j = getId(names, t);
            mat[i][j] = mat[j][i] = 1;
        }
    }

    const auto result = globalMinCut(mat);
    std::cout << result.second.size() * (names.size() - result.second.size());
    return 0;
}
