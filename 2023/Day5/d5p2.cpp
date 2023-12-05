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

bool ranges_overlap(long long s1, long long e1, long long s2, long long e2)
{
    return (s1 <= e2 && e1 >= s2) || (e1 >= s2 && s1 <= e2);
}

long long findMinFromRangeTree(const long long start, const long long end, const int map_index, std::vector<std::vector<mapping>>& maps)
{
    long long min = LONG_MAX;
    long long i = start;
    if (map_index == maps.size() - 1) {
        min = start;
        for (mapping& m : maps[map_index]) {
            if (ranges_overlap(start, end, m.source_start, m.source_start + m.range)) {
                long long finalMin = start + (m.destination_start - m.source_start);
                min = min < finalMin ? min : finalMin;
            }
        }
    } else {
        for (mapping& m : maps[map_index]) {
            if (ranges_overlap(start, end, m.source_start, m.source_start + m.range)) {
                long long value;
                if (i < m.source_start) {
                    value = findMinFromRangeTree(i, m.source_start - 1, map_index + 1, maps);
                    min = min < value ? min : value;
                }
                long long s = std::max(start, m.source_start) + (m.destination_start - m.source_start);
                long long e = std::min(end, m.source_start + m.range) + (m.destination_start - m.source_start);
                value = findMinFromRangeTree(s, e, map_index + 1, maps);
                min = min < value ? min : value;
                i = m.source_start + m.range + 1;
            }
        }
        if (i < end) {
            long long value = findMinFromRangeTree(i, end, map_index + 1, maps);
            min = min < value ? min : value;
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

    for (auto& map : maps) {
        std::sort(map.begin(), map.end(), [](const mapping& m, const mapping& n) { return m.source_start < n.source_start; });
    }

    long long min_seed = LLONG_MAX;
    for (size_t i = 0; i < seeds.size(); i += 2) {
        long long value = findMinFromRangeTree(seeds[i], seeds[i] + seeds[i + 1] - 1, 0, maps);
        min_seed = min_seed < value ? min_seed : value;
    }

    inputFile.close();
    std::cout << min_seed;
    return 0;
}
