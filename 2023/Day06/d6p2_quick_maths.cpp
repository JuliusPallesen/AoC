#include <algorithm>
#include <fstream>
#include <iostream>
#include <sstream>
#include <math.h>

int main(int argc, char const* argv[])
{
    std::ifstream inputFile;
    std::string line;
    unsigned long ans = 0;
    unsigned long time;
    unsigned long distance;

    inputFile.open(argv[1]);

    std::getline(inputFile, line);
    line = line.substr(line.find(':') + 1, line.size() - line.find(':'));
    line.erase(std::remove(line.begin(), line.end(), ' '), line.end());
    time = stoll(line);

    std::getline(inputFile, line);
    line = line.substr(line.find(':') + 1, line.size() - line.find(':'));
    line.erase(std::remove(line.begin(), line.end(), ' '), line.end());
    distance = stoll(line);
 
    inputFile.close();
    std::cout << (unsigned long)(time + sqrt(time * time - 4* distance) / 2) - (unsigned long)(time - sqrt(time * time - 4* distance) / 2);
    return 0;
}
