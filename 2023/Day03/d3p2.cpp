#include <algorithm>
#include <fstream>
#include <iostream>
#include <sstream>
#include <vector>

struct Number {
    int num;
    int start;
    int end;
};

bool isAdjacent(Number num, int x)
{
    return (num.start <= x + 1 && num.end >= x - 1) || (num.end >= x - 1 && num.start <= x + 1);
}

int main(int argc, char const* argv[])
{
    std::ifstream inputFile;
    inputFile.open(argv[1]);
    std::string line;
    int ans = 0;
    int number = 0;
    int start = -1;
    std::vector<std::string> lines;
    while (std::getline(inputFile, line)) {
        lines.push_back(line);
    }

    std::vector<std::vector<Number>> nums(lines.size());
    for (int y = 0; y < lines.size(); y++) {
        for (int x = 0; x < lines[y].size(); x++) {
            if (isdigit(lines[y][x])) {
                if (start == -1) {
                    start = x;
                }
                number = number * 10 + (lines[y][x] - '0');
            }
            if ((!isdigit(lines[y][x]) || x == lines[y].size() - 1) && number != 0) {
                nums[y].push_back({ number, start, x - 1 });
                number = 0;
                start = -1;
            }
        }
    }

    for (int y = 0; y < lines.size(); y++) {
        for (int x = 0; x < lines[y].size(); x++) {
            if (lines[y][x] == '*') {
                number = 1;
                int gears = 0;
                for (int i = std::max(0, (int)y - 1); i < std::min(y + 2, (int)lines.size()); i++) {
                    for (auto& num : nums[i]) {
                        if (isAdjacent(num, x)) {
                            number *= num.num;
                            gears++;
                        }
                    }
                }
                if (gears == 2) {
                    ans += number;
                }
            }
        }
    }
    inputFile.close();
    std::cout << ans;
    return 0;
}
