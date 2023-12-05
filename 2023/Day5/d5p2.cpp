#include <algorithm>    // For sort  
#include <climits>      // For LLMAX
#include <fstream>      // For file parsing
#include <iostream>     // For printing
#include <sstream>      // For string parsing
#include <vector>       

struct mapping {
    uint64_t source_start;
    uint64_t destination_start;
    uint64_t range;
};

bool ranges_overlap(uint64_t s1, uint64_t e1, uint64_t s2, uint64_t e2)
{
    return (s1 <= e2 && e1 >= s2) || (e1 >= s2 && s1 <= e2);
}

uint64_t findMinFromRangeTree(const uint64_t start, const uint64_t end, const int map_index, std::vector<std::vector<mapping>>& maps)
{
    uint64_t min = LLONG_MAX;
    uint64_t i = start;
    uint64_t value;
    if (map_index == maps.size() - 1) {
        min = start;
        for (mapping& m : maps[map_index]) {
            if (ranges_overlap(start, end, m.source_start, m.source_start + m.range)) {
                value = start + (m.destination_start - m.source_start);
                min = std::min(min, value);
            }
        }
    } else {
        for (mapping& m : maps[map_index]) {
            if (ranges_overlap(start, end, m.source_start, m.source_start + m.range)) {
                if (i < m.source_start) {
                    value = findMinFromRangeTree(i, m.source_start - 1, map_index + 1, maps);
                    min = std::min(min, value);
                }
                uint64_t s = std::max(start, m.source_start) + (m.destination_start - m.source_start);
                uint64_t e = std::min(end, m.source_start + m.range) + (m.destination_start - m.source_start);
                value = findMinFromRangeTree(s, e, map_index + 1, maps);
                min = std::min(min, value);
                i = m.source_start + m.range + 1;
            }
        }
        if (i < end) {
            value = findMinFromRangeTree(i, end, map_index + 1, maps);
            min = std::min(min, value);
        }
    }
    return min;
}

int main(int argc, char const* argv[])
{
    std::ifstream inputFile;
    inputFile.open(argv[1]);
    std::string line;
    std::string str;
    std::getline(inputFile, line);
    std::vector<uint64_t> seeds;
    std::stringstream ss(line);
    std::vector<std::vector<mapping>> maps;
    uint8_t map_index = -1;

    ss >> str;
    while (ss >> str) {
        seeds.push_back(stoll(str));
    }

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
                maps.push_back(std::vector<mapping>());
                map_index++;
            }
        }
    }

    for (auto& map : maps) {
        std::sort(map.begin(), map.end(), [](const mapping& m, const mapping& n) { return m.source_start < n.source_start; });
    }

    uint64_t min_seed = LLONG_MAX;
    for (size_t i = 0; i < seeds.size(); i += 2) {
        uint64_t value = findMinFromRangeTree(seeds[i], seeds[i] + seeds[i + 1] - 1, 0, maps);
        min_seed = std::min(min_seed, value);
    }

    inputFile.close();
    std::cout << min_seed;
    return 0;
}
