#include <algorithm>
#include <fstream>
#include <iostream>
#include <sstream>
#include <vector>
#include <numeric>

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
    while (std::getline(inputFile, line))
    {
        std::stringstream ss(line);
        std::string str;
        while (std::getline(ss, str, ','))
        {
            ans += holidayHash(str);
        }
    }
    inputFile.close();
    std::cout << ans << '\n';
    return 0;
}
