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

    int red = 12;
    int green = 13;
    int blue = 14;

    while (std::getline(inputFile, line)) {
        bool possible = true;
        int gameid = stoi(line.substr(line.find(' '), line.find(':') - line.find(' ')));
        line = line.substr(line.find(':') + 1, line.size() - line.find(':'));
        std::string roll;
        std::stringstream ss(line);
        while (possible && std::getline(ss, roll, ';')) {
            std::string color;
            std::stringstream rs(roll);
            while (possible && std::getline(rs, color, ',')) {
                color.erase(color.begin());
                int amount = stoi(color.substr(0, color.find(' ')));
                color = color.substr(color.find(' ') + 1, color.size() - color.find(' ') + 1);
                int max_amount = 0;

                if (color == "red")
                    max_amount = red;
                if (color == "blue")
                    max_amount = blue;
                if (color == "green")
                    max_amount = green;

                if (amount > max_amount)
                    possible = false;
            }
        }
        ans = possible ? ans + gameid : ans;
    }

    inputFile.close();
    std::cout << ans;
    return 0;
}