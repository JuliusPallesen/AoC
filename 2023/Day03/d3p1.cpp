#include <algorithm>
#include <fstream>
#include <iostream>
#include <sstream>
#include <vector>

bool isPart(const char& c)
{
    return !isdigit(c) && c != '.';
}

bool checkAdjecent(const int x, const int y, const std::vector<std::string>& lines)
{
    for (size_t i = std::max(0, x - 1); i <= std::min(x + 1, (int)lines.size() - 1); i++) {
        for (size_t j = std::max(0, y - 1); j <= std::min(y + 1, (int)lines[i].size() - 1); j++) {
            if (isPart(lines[i][j]))
                return true;
        }
    }
    return false;
}

int main(int argc, char const* argv[])
{
    std::ifstream inputFile;
    inputFile.open(argv[1]);
    std::string line;
    int ans = 0;
    int number = 0;
    bool is_adjacent = false;
    std::vector<std::string> lines;
    while (std::getline(inputFile, line)) {
        lines.push_back(line);
    }

    for (size_t x = 0; x < lines.size(); x++) {
        for (size_t y = 0; y < lines[x].size(); y++) {
            if (!isdigit(lines[x][y]) || y == lines[x].size() - 1) {
                ans = is_adjacent ? ans + number : number;
                number = 0;
                is_adjacent = false;
            } else {
                number = number * 10 + (lines[x][y] - '0');
                is_adjacent = checkAdjecent(x, y, lines) ? true : is_adjacent;
            }
        }
    }
    inputFile.close();
    std::cout << ans;
    return 0;
}
