#include <algorithm>
#include <fstream>
#include <iostream>
#include <iterator>
#include <sstream>
#include <vector>

std::vector<long> scuffedDerive(const std::vector<long>& sensor)
{
    std::vector<long> derive;
    for (size_t i = 1; i < sensor.size(); i++) {
        derive.push_back(sensor[i] - sensor[i - 1]);
    }
    return derive;
}

long getNextValue(const std::vector<long>& sensor)
{
    std::vector<long> derive = scuffedDerive(sensor);
    if (derive.size() == 0 || std::count_if(derive.begin(), derive.end(), [](const long r) { return r == 0; }) == derive.size()) {
        return 0;
    } else {
        return derive[derive.size() - 1] + getNextValue(derive);
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
        sensorReadings.push_back(std::vector<long>(std::istream_iterator<long>(iss), std::istream_iterator<long>()));
    }

    for (const auto& sensor : sensorReadings) {
        ans += getNextValue(sensor) + sensor[sensor.size() -1];
    }

    inputFile.close();
    std::cout << ans;
    return 0;
}
