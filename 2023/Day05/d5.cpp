#include <algorithm>
#include <climits>
#include <fstream>
#include <iostream>
#include <sstream>
#include <vector>

struct mapping {
    long long source_start;
    long long destination_start;
    long long range;
};

int main(int argc, char const* argv[])
{
    std::ifstream inputFile;
    inputFile.open(argv[1]);
    std::string line;
    std::string str;
    long long ans = LONG_MAX;
    std::getline(inputFile, line);
    std::vector<long long> seeds;
    std::stringstream ss(line);
    std::vector<std::vector<mapping>> maps(7, std::vector<mapping>());
    ss >> str;
    while (ss >> str) {
        seeds.push_back(stoll(str));
    }
    int map_index = -1;
    while (std::getline(inputFile, line)) {
        if (!line.empty()) {
            if (isdigit(line[0])) {
                std::stringstream ss(line);
                mapping m;
                ss >> m.destination_start;
                ss >> m.source_start;
                ss >> m.range;
                maps[map_index].push_back(m);
            } else {
                map_index++;
            }
        }
    }
    long long min_seed = LLONG_MAX;
    for (long long& num : seeds) {
        long long val = num;
        for (int map_index = 0; map_index < 7; map_index++) {
            for (mapping& m : maps[map_index]) {
                if (val >= m.source_start && val < m.source_start + m.range) {
                    val += (m.destination_start - m.source_start);
                    break;
                }
            }
        }
        min_seed = min_seed > val ? val : min_seed;
    }

    inputFile.close();
    std::cout << min_seed;
    return 0;
}
