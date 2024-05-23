#include <fstream>
#include <iostream>
#include <string>
#include <vector>
#include <ranges>
#include <numeric>

int main(int argc, char const *argv[])
{
    std::ifstream inputFile;
    inputFile.open(argv[1]);
    std::string line;
    std::vector<int> values;
    int last = INT_MAX;
    int count = 0;
    while (std::getline(inputFile, line))
    {
        values.push_back(std::stoi(line));
    }
    for (auto chunk : (values | std::ranges::views::slide(3)))
    {
        int v = chunk[0] + chunk[1] + chunk[2];
        count += (v > last);
        last = v;
    }

    std::cout << count << "\n";
    return 0;
}
