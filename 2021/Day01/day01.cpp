#include <fstream>
#include <iostream>
#include <string>

int main(int argc, char const *argv[])
{
    std::ifstream inputFile;
    inputFile.open(argv[1]);
    std::string line;
    int last = INT_MAX;
    int count = 0;
    while (std::getline(inputFile, line))
    {
        int value = std::stoi(line);
        if (value > last)
            count++;
        last = value;
    }
    std::cout << count << "\n";
    return 0;
}
