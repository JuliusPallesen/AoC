#include <algorithm>
#include <fstream>
#include <iostream>
#include <sstream>
#include <vector>
#include <numeric>
#include <array>
#include <unordered_map>

struct Lense
{
    std::string label;
    int focal_length;
};

uint8_t holidayHash(const std::string &str)
{
    return std::accumulate(str.begin(), str.end(), 0,
                           [](int accumulator, const char c)
                           { return (static_cast<int>(accumulator + c) * 17) % 256; });
}

int main(int argc, char const *argv[])
{
    std::ifstream inputFile;
    inputFile.open(argv[1]);
    std::string line;
    int ans = 0;
    std::array<std::vector<Lense>, 256> boxes;
    while (std::getline(inputFile, line))
    {
        std::stringstream ss(line);
        std::string str;
        while (std::getline(ss, str, ','))
        {
            std::string label = str.substr(0, str.find_first_of("-="));
            uint8_t i = holidayHash(label);
            const auto l = std::find_if(boxes[i].begin(), boxes[i].end(), [&label](const auto &l)
                                        { return l.label == label; });
            if (str.find('-') != str.npos && l != boxes[i].end())
            {
                boxes[i].erase(l);
            }
            if (str.find('=') != str.npos)
            {
                std::string numstr = str.substr(str.find('=') + 1, str.size());
                if (l != boxes[i].end())
                {
                    l->focal_length = std::stoi(numstr);
                }
                else
                {
                    boxes[i].push_back({label, std::stoi(numstr)});
                }
            }
        }
    }
    inputFile.close();

    for (size_t i = 0; i < boxes.size(); i++)
    {
        for (size_t j = 0; j < boxes[i].size(); j++)
        {
            ans += (1 + i) * (1 + j) * boxes[i][j].focal_length;
        }
    }

    std::cout << ans << '\n';
    return 0;
}
