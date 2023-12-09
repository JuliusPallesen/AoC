#include <algorithm>
#include <fstream>
#include <iostream>
#include <sstream>
#include <unordered_map>
#include <vector>
#include <numeric>

struct NetworkNode {
    std::string name;
    NetworkNode* left;
    NetworkNode* right;
};

NetworkNode* getNode(const std::string& nodename, std::unordered_map<std::string, NetworkNode*>& nodes)
{
    if (nodes.count(nodename) > 0) {
        return nodes[nodename];
    } else {
        nodes[nodename] = new NetworkNode();
        nodes[nodename]->name = nodename;
        return nodes[nodename];
    }
}

int main(int argc, char const* argv[])
{
    std::ifstream inputFile;
    inputFile.open(argv[1]);
    std::string line;
    std::string instructions;
    std::unordered_map<std::string, NetworkNode*> nodes;
    uint64_t ans = 0;
    std::getline(inputFile, instructions);

    while (std::getline(inputFile, line)) {
        if (!line.empty()) {
            NetworkNode* node = getNode(line.substr(0, line.find(' ')), nodes);
            line = line.substr(line.find('=') + 1, line.size());
            node->left = getNode(line.substr(line.find('(') + 1, line.find(',') - line.find('(') - 1), nodes);
            node->right = getNode(line.substr(line.find(',') + 2, line.find(')') - line.find(',') - 2), nodes);
        }
    }

    std::vector<NetworkNode*> ptrs;

    for (auto& n : nodes) {
        if (n.first[n.first.size() - 1] == 'A') {
            ptrs.push_back(n.second);
        }
    }

    uint64_t i = 0;
    int foundLoops = 0;
    std::vector<int64_t> loopLengths(ptrs.size(), -1);
    while (foundLoops < ptrs.size()) {
        char dir = instructions[i % (instructions.size())];
        for (size_t j = 0; j < ptrs.size(); ++j) {
            if (dir == 'L') {
                ptrs[j] = ptrs[j]->left;
            } else {
                ptrs[j] = ptrs[j]->right;
            }
            if (ptrs[j]->name[ptrs[j]->name.size() - 1] == 'Z') {
                foundLoops++;
                loopLengths[j] = loopLengths[j] == -1 ? i + 1 : loopLengths[j];
            }
        }
        ++i;
    }
    // Find Lowest Commom Multiple of all LoopLengths (for lcm compile with std=c++17 or higher)
    ans = std::accumulate(loopLengths.begin(), loopLengths.end(), 1LL, std::lcm<int64_t, int64_t>);;
    inputFile.close();
    std::cout << ans;
    return 0;
}
