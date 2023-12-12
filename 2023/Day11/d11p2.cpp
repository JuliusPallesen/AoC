#include <algorithm>
#include <fstream>
#include <iostream>
#include <sstream>
#include <vector>

std::vector<std::string> addLinesIfNoGalaxies(std::vector<std::string>& input)
{
    std::vector<std::string> output;
    for (size_t y = 0; y < input.size() - 1; y++) {
    }
    return output;
}

int main(int argc, char const* argv[])
{
    std::ifstream inputFile;
    inputFile.open(argv[1]);
    std::string line;
    std::vector<std::string> input;
    uint64_t ans = 0;
    std::vector<bool> empty_y(256, true);
    std::vector<bool> empty_x(256, true);
    std::vector<std::pair<int, int>> galaxies;
    int y = 0;
    while (std::getline(inputFile, line)) {
        input.push_back(line);
        bool found = false;
        for (size_t x = 0; x < line.size(); x++) {
            if (line[x] == '#') {
                empty_y[y] = false;
                empty_x[x] = false;
                galaxies.push_back({ y, x });
            }
        }
        y++;
    }
    empty_y.resize(y);
    empty_x.resize(input[0].size());

    for (size_t i = 0; i < galaxies.size(); i++) {
        for (size_t j = i + 1; j < galaxies.size(); j++) {
            int max_y = std::max(galaxies[i].first, galaxies[j].first);
            int min_y = std::min(galaxies[i].first, galaxies[j].first);
            int max_x = std::max(galaxies[i].second, galaxies[j].second);
            int min_x = std::min(galaxies[i].second, galaxies[j].second);
            ans += (max_x - min_x) + (max_y - min_y);
            ans += 999999 * std::count_if(empty_x.begin() + min_x, empty_x.begin() + max_x, [](bool b){return b;});
            ans += 999999 * std::count_if(empty_y.begin() + min_y, empty_y.begin() + max_y, [](bool b){return b;});
        }
    }

    inputFile.close();
    std::cout << ans;
    return 0;
}
