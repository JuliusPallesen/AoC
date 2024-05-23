#include <fstream>
#include <iostream>
#include <string>
#include <cstdint>

struct position
{
    int aim = 0, depth = 0, h_pos = 0;
};

void move(position &pos, char dir, uint64_t steps)
{
    switch (dir)
    {
    case 'u':
        pos.aim -= steps;
        break;
    case 'd':
        pos.aim += steps;
        break;
    case 'f':
        pos.depth += steps * pos.aim;
        pos.h_pos += steps;
        break;
    default:
        throw(std::runtime_error("invalid direction"));
    }
}

int main(int argc, char const *argv[])
{
    std::ifstream inputFile;
    inputFile.open(argv[1]);
    std::string line;
    position pos;
    while (std::getline(inputFile, line))
        move(pos, line[0], std::stoi(line.substr(line.find(' '))));

    std::cout << pos.h_pos * pos.depth << "\n";
    return 0;
}
