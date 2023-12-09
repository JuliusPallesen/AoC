#include <algorithm>
#include <fstream>
#include <iostream>
#include <iterator>
#include <sstream>
#include <vector>

std::vector<long> scuffedDerive(const std::vector<long>& sensor)
{
    std::vector<long> derived;
    for (size_t i = 1; i < sensor.size(); i++) {
        derived.push_back(sensor[i] - sensor[i - 1]);
    }
    return derived;
}

long getNextValue(const std::vector<long>& sensor)
{
    std::vector<long> derived = scuffedDerive(sensor);
    if (derived.size() == 0 || std::count_if(derived.begin(), derived.end(), [](const long r) { return r == 0; }) == static_cast<int>(derived.size())) {
        return 0;
    } else {
        return derived[derived.size() - 1] + getNextValue(derived);
    }
}

int main(int argc, char const* argv[])
{
    std::ifstream inputFile;
    inputFile.open(argv[1]);
    std::string line;
    long ans = 0;
    std::vector<std::vector<long>> sensorReadings;
    while (std::getline(inputFile, line)) {
        std::istringstream iss(line);
        std::vector<long> sensor = std::vector<long>(std::istream_iterator<long>(iss), std::istream_iterator<long>());
        ans += getNextValue(sensor) + sensor[sensor.size() - 1];
    }

    inputFile.close();
    std::cout << ans;
    return 0;
}
