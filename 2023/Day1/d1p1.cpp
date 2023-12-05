#include <algorithm>
#include <fstream>
#include <iostream>
#include <sstream>

int main(int argc, char const* argv[])
{
    std::ifstream inputFile;
    inputFile.open(argv[1]);
    std::string line;
    long sum = 0;
    while (std::getline(inputFile, line)) {
        char first = 0;
        char last = 0;
        const auto isdigit = [](char& c) { return c - '0' >= 0 && c - '0' <= 10; };
        first = *std::find_if(line.begin(), line.end(), isdigit) - '0';
        last = *std::find_if(line.rbegin(), line.rend(), isdigit) - '0';
        std::cout << 10 * first + last << "\n";
        sum += 10 * first + last;
    }
    std::cout << sum;
    inputFile.close();
    return 0;
}
