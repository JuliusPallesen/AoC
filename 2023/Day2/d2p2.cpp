#include <algorithm>
#include <array>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

int main(int argc, char const* argv[])
{
    std::ifstream inputFile;
    inputFile.open(argv[1]);
    std::string line;
    int ans = 0;

    while (std::getline(inputFile, line)) {
        int min_r = 0;
        int min_g = 0;
        int min_b = 0;
        int gameid = stoi(line.substr(line.find(' '), line.find(':') - line.find(' ')));
        line = line.substr(line.find(':') + 1, line.size() - line.find(':'));
        std::string roll;
        std::stringstream ss(line);
        while (std::getline(ss, roll, ';')) {
            std::string color;
            std::stringstream rs(roll);
            while (std::getline(rs, color, ',')) {
                color.erase(color.begin());
                int amount = stoi(color.substr(0, color.find(' ')));
                color = color.substr(color.find(' ') + 1, color.size() - color.find(' ') - 1);
                int max_amount = 0;
                if (color == "red")
                    min_r = std::max(min_r, amount);
                if (color == "blue")
                    min_b = std::max(min_b, amount);
                if (color == "green")
                    min_g = std::max(min_g, amount);
            }
        }
        ans += min_r * min_b * min_g;
    }

    inputFile.close();
    std::cout << ans;
    return 0;
}