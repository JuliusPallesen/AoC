#include "C:/Program Files/Libs/json-develop/single_include/nlohmann/json.hpp"
#include <algorithm>
#include <fstream>
#include <iostream>
#include <sstream>
#include <vector>

using json = nlohmann::json;

int sumIntegersWithoutRed(const json& j)
{
    int sum = 0;

    if (j.is_object()) {
        bool red = false;
        for (auto& entry : j.items()) {
            if (entry.value() == "red") {
                red = true;
                break;
            }
        }
        if (!red) {
            for (auto& entry : j.items()) {
                sum += sumIntegersWithoutRed(entry.value());
            }
        }
    } else if (j.is_number()) {
        sum += j.get<int>();
    } else if (j.is_array()) {
        for (auto& element : j) {
            sum += sumIntegersWithoutRed(element);
        }
    }

    return sum;
}

int main(int argc, char const* argv[])
{
    int ans = 0;
    std::ifstream inputFile(argv[1]);
    json data = json::parse(inputFile);

    std::cout << sumIntegersWithoutRed(data);
    return 0;
}
