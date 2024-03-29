#include "C:/Program Files/Libs/json-develop/single_include/nlohmann/json.hpp"
#include <algorithm>
#include <fstream>
#include <iostream>
#include <sstream>
#include <vector>

using json = nlohmann::json;

int sumIntegers(const json& j)
{
    int sum = 0;

    if (j.is_object()) {
        for (auto& entry : j.items()) {
            sum += sumIntegers(entry.value());
        }
    } else if (j.is_number()) {
        sum += j.get<int>();
    } else if (j.is_array()) {
        for (auto& element : j) {
            sum += sumIntegers(element);
        }
    }

    return sum;
}

int main(int argc, char const* argv[])
{
    int ans = 0;
    std::ifstream inputFile(argv[1]);
    json data = json::parse(inputFile);

    std::cout << sumIntegers(data);
    return 0;
}