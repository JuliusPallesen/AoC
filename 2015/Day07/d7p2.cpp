#include <algorithm>
#include <fstream>
#include <functional>
#include <iostream>
#include <sstream>
#include <unordered_map>
#include <vector>

uint16_t evaluate(std::string key, std::unordered_map<std::string, std::vector<std::string>>& connections)
{
    if (isdigit(key[0])) {
        return static_cast<uint16_t>(stoi(key));
    }
    std::vector<std::string> leftRule = connections[key];
    uint16_t ret = 0;
    uint16_t left;
    uint16_t right;
    switch (leftRule.size()) {
    case 1:
        ret = evaluate(leftRule[0], connections);
        break;
    case 2:
        ret = ~evaluate(leftRule[1], connections);
        break;
    case 3:
        left = evaluate(leftRule[0], connections);
        right = evaluate(leftRule[2], connections);
        if (leftRule[1] == "LSHIFT") {
            ret = left << right;
        }
        if (leftRule[1] == "RSHIFT") {
            ret = left >> right;
        }
        if (leftRule[1] == "AND") {
            ret = left & right;
        }
        if (leftRule[1] == "OR") {
            ret = left | right;
        }
        break;
    default:
        break;
    }
    connections[key].clear();
    connections[key].push_back(std::to_string(ret));

    return ret;
}

int main(int argc, char const* argv[])
{
    std::ifstream inputFile;
    inputFile.open(argv[1]);
    std::string line;
    std::string word;
    std::unordered_map<std::string, std::vector<std::string>> connections;
    std::unordered_map<std::string, std::vector<std::string>> connections2;
    int ans;
    while (getline(inputFile, line)) {
        std::stringstream ss(line);
        std::vector<std::string> words;
        while (ss >> word) {
            if (word != "->") {
                words.push_back(word);
            }
        }
        std::string key = words[words.size() - 1];
        words.pop_back();
        connections[key] = words;
        connections2[key] = std::vector<std::string>(words);
    }

    inputFile.close();

    uint16_t ares = evaluate("a", connections);
    std::cout << "a: " << ares << std::endl;

    connections2["b"].clear();
    connections2["b"].push_back(std::to_string(ares));

    std::cout << "a after change: " << evaluate("a", connections2) << std::endl;

    return 0;
}
