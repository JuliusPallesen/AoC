#include <algorithm>
#include <fstream>
#include <iostream>
#include <set>
#include <sstream>
#include <vector>

int main(int argc, char const* argv[])
{
    std::ifstream inputFile;
    inputFile.open(argv[1]);
    std::string line;
    int ans = 0;
    int i = -1;
    std::vector<int> multi(256, 1);

    while (std::getline(inputFile, line)) {
        std::stringstream ss(line);
        std::string str;
        std::set<int> winning;
        std::vector<int> myNumbers;
        int copies = multi[++i];
        int amount = 0;

        ss >> str >> str;
        while (ss >> str && str != "|") {
            winning.emplace(stoi(str));
        }
        while (ss >> str) {
            amount += winning.find(stoi(str)) != winning.end();
        }

        std::for_each(multi.begin() + i + 1, multi.begin() + i + amount + 1, [copies](int& m) { m += copies; });
        ans += copies;
    }
    inputFile.close();
    std::cout << ans;
    return 0;
}
