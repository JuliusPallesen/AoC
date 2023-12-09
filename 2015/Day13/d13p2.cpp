#include <algorithm>
#include <fstream>
#include <iostream>
#include <sstream>
#include <unordered_map>
#include <vector>

struct Person {
    std::string name;
    std::vector<std::pair<std::string, int>> preferences;
};

int main(int argc, char const* argv[])
{
    std::ifstream inputFile;
    inputFile.open(argv[1]);
    std::string line;
    std::string str;
    std::unordered_map<std::string, Person> people;
    int ans = 0;
    while (std::getline(inputFile, line)) {
        std::stringstream ss(line);
        std::string name;
        std::string next;
        ss >> name;

        ss >> str >> str;

        int happieness = str == "gain" ? 1 : -1;
        ss >> str;
        happieness *= stoi(str);

        ss >> str >> str >> str >> str >> str >> str >> str >> str;
        next = str.substr(0, str.size() - 1);

        people[name].name = name;
        people[name].preferences.push_back({ next, happieness });
        std::cout << name << "-> " << next << " = " << happieness << "\n";
    }
    std::vector<Person> table;
    for (auto& p : people) {
        table.push_back(p.second);
    }
    table.push_back({"me", std::vector<std::pair<std::string,int>>()});

    int max_happy = 0;
    while (std::next_permutation(table.begin(), table.end(), [](const auto & p1, const auto & p2){return p1.name < p2.name;})) {
        int happieness = 0;
        for (int i = 0; i < table.size(); ++i) {
            std::string left = table[i - 1 == -1 ? table.size() - 1 : i - 1].name;
            std::string right = table[(i + 1) % table.size()].name;
            for (auto & p : table[i].preferences)
            {
                if(p.first == left || p.first == right) {
                    happieness += p.second;
                }
            }
        }
        max_happy = std::max(max_happy, happieness);
    }

    inputFile.close();
    std::cout << max_happy;
    return 0;
}
