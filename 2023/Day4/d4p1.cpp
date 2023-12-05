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
    while (std::getline(inputFile, line)) {
        std::stringstream ss(line);
        std::string str;
        std::set<int> winning;
        std::vector<int> myNumbers;
        int winnings = 0;
        ss >> str >> str;

        while (ss >> str && str != "|") {
            winning.emplace(stoi(str));
        }
        while (ss >> str) {
            if (winning.find(stoi(str)) != winning.end()) {
                winnings = winnings == 0 ? 1 : winnings * 2;
            }
        }
        ans += winnings;
    }
    inputFile.close();
    std::cout << ans;
    return 0;
}
