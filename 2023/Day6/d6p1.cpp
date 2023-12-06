#include <algorithm>
#include <fstream>
#include <iostream>
#include <sstream>
#include <vector>

int main(int argc, char const *argv[])
{
    std::ifstream inputFile;
    std::string line;
    std::string str;
    std::vector<int> time;
    std::vector<int> distance;
    int ans = 1;

    inputFile.open(argv[1]);
    std::getline(inputFile, line);
    std::stringstream ss(line);
    ss >> str;
    while (ss >> str)
    {
        time.push_back(stoi(str));
    }

    std::getline(inputFile, line);
    ss = std::stringstream(line);
    ss >> str;
    while (ss >> str)
    {
        distance.push_back(stoi(str));
    }

    for (size_t i = 0; i < time.size(); i++)
    {
        int winning_strategies = 0;
        for (size_t j = 0; j <= time[i]; j++)
        {
            winning_strategies += ((j * (time[i] - j)) > distance[i]);
        }
        ans *= winning_strategies;
    }

    inputFile.close();
    std::cout << ans;
    return 0;
}
