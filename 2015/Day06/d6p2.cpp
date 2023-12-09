#include <algorithm>
#include <array>
#include <fstream>
#include <functional>
#include <iostream>
#include <sstream>
#include <vector>

int main(int argc, char const* argv[])
{

    std::cout << "Hello World!" << std::endl;

    std::ifstream inputFile;
    std::string line;
    std::string word;

    long ans = 0;

    // 1000x1000 Array of ints crashed due to excessive memory usage:
    std::array<char, 1000> initRow;
    initRow.fill(0);
    std::array<std::array<char, 1000>, 1000> lights;
    lights.fill(initRow);

    inputFile.open(argv[1]);
    if (!inputFile.is_open()) {
        std::cerr << "Couldn't open file.\n";
        exit(1);
    }

    while (getline(inputFile, line)) {
        unsigned int xstart;
        unsigned int xend;

        unsigned int ystart;
        unsigned int yend;

        std::function<char(char)> f;

        std::stringstream ss(line);
        ss >> word;
        if (word == "toggle") {
            f = [](char light) { return light + 2; };
        }
        if (word == "turn") {
            ss >> word;
            if (word == "on") {
                f = [](char light) { return light + 1; };
            } else {
                f = [](char light) { return std::max(0, light - 1); };
            }
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
