#include <algorithm>
#include <fstream>
#include <iostream>
#include <set>
#include <sstream>
#include <vector>

struct Santa {
    int x;
    int y;
};

void visitHouse(Santa* santa, const char c)
{
    switch (c) {
    case '<':
        santa->x--;
        break;
    case '>':
        santa->x++;
        break;
    case 'v':
        santa->y--;
        break;
    case '^':
        santa->y++;
        break;
    default:
        break;
    }
}

int main(int argc, char const* argv[])
{
    std::ifstream inputFile;
    inputFile.open(argv[1]);
    std::string line;
    int ans;
    char c;
    Santa santa = { 0, 0 };
    Santa robo_santa = { 0, 0 };
    Santa* curr_santa;
    std::set<std::pair<int, int>> visited;
    visited.emplace(std::pair<int, int>(0, 0));
    while (inputFile >> c) {
        ans++;
        if (ans % 2 == 0) {
            curr_santa = &santa;
        } else {
            curr_santa = &robo_santa;
        }
        visitHouse(curr_santa, c);
        visited.emplace(std::pair<int, int>(curr_santa->x, curr_santa->y));
    }
    inputFile.close();
    std::cout << visited.size();
    return 0;
}
