#include <cstring>
#include <fstream>
#include <iostream>
#include <sstream>

int main(int argc, char const* argv[])
{
    std::ifstream inputFile;
    inputFile.open(argv[1]);
    std::string line;
    int ans = 0;
    while (std::getline(inputFile, line)) {
        bool has_seperated_pair = false;
        bool has_unique_pair = false;
        int pair_positions[256][256] = {};
        for (size_t i = 0; i < line.size() && (!has_seperated_pair || !has_unique_pair); i++) {
            has_seperated_pair = (!has_seperated_pair && i >= 2 && line[i] == line[i - 2]) ? true : has_seperated_pair;
            if (!has_unique_pair && i >= 1) {
                has_unique_pair = (pair_positions[line[i]][line[i - 1]] > 0 && i - pair_positions[line[i]][line[i - 1]] > 1) ? true : has_unique_pair;
                pair_positions[line[i]][line[i - 1]] = i;
            }
        }
        ans += (has_seperated_pair && has_unique_pair);
    }

    inputFile.close();
    std::cout << ans;
    return 0;
}
