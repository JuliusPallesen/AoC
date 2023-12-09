#include <algorithm>
#include <array>
#include <fstream>
#include <functional>
#include <iostream>
#include <sstream>
#include <vector>

int main(int argc, char const* argv[])
{
    std::ifstream inputFile;
    inputFile.open(argv[1]);
    std::string line;
    std::string word;
    int ans = 0;
    std::array<bool, 1000> initRow;
    initRow.fill(false);
    std::array<std::array<bool, 1000>, 1000> lights;
    lights.fill(initRow);
    while (getline(inputFile, line)) {
        unsigned int xstart;
        unsigned int xend;

        unsigned int ystart;
        unsigned int yend;

        bool state;

        std::function<bool(bool)> f;

        std::stringstream ss(line);
        ss >> word;
        if (word == "toggle") {
            f = [](bool light) { return !light; };
        }
        if (word == "turn") {
            ss >> word;
            if (word == "on") {
                state = true;
            } else {
                state = false;
            }
            f = [&state](bool light) { return state; };
        }
        ss >> word;
        xstart = stoi(word.substr(0, word.find(',')));
        ystart = stoi(word.substr(word.find(',') + 1, word.size()));
        ss >> word;
        ss >> word;

        xend = stoi(word.substr(0, word.find(',')));
        yend = stoi(word.substr(word.find(',') + 1, word.size()));

        for (size_t i = xstart; i <= xend; i++) {
            for (size_t j = ystart; j <= yend; j++) {
                lights[i][j] = f(lights[i][j]);
            }
        }
    }

    for (size_t i = 0; i < 1000; i++) {
        for (size_t j = 0; j < 1000; j++) {
            ans += lights[i][j];
        }
    }

    inputFile.close();
    std::cout << ans;
    return 0;
}
