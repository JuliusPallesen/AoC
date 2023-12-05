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
    int ans;
    char c;
    int x = 0;
    int y = 0;
    std::set<std::pair<int, int>> visited;
    visited.emplace(std::pair<int, int>(0, 0));
    while (inputFile >> c) {
        switch (c) {
        case '<':
            x--;
            break;
        case '>':
            x++;
            break;
        case 'v':
            y--;
            break;
        case '^':
            y++;
            break;
        }
        visited.emplace(std::pair<int, int>(x, y));
    }
    inputFile.close();
    std::cout << visited.size();
    return 0;
}
