#include <algorithm>
#include <fstream>
#include <iostream>
#include <sstream>

int main(int argc, char const* argv[])
{
    std::ifstream inputFile;
    std::string line;
    uint64_t ans = 0;
    uint64_t time;
    uint64_t distance;

    inputFile.open(argv[1]);

    std::getline(inputFile, line);
    line = line.substr(line.find(':') + 1, line.size() - line.find(':'));
    line.erase(std::remove(line.begin(), line.end(), ' '), line.end());
    time = stoll(line);

    std::getline(inputFile, line);
    line = line.substr(line.find(':') + 1, line.size() - line.find(':'));
    line.erase(std::remove(line.begin(), line.end(), ' '), line.end());
    distance = stoll(line);

    for (size_t j = 1; j <= time; ++j) {
        ans += ((j * (time - j)) > distance);
    }

    inputFile.close();
    std::cout << ans;
    return 0;
}
