#include <algorithm>
#include <fstream>
#include <iostream>
#include <sstream>
#include <vector>

int main(int argc, char const* argv[])
{
    std::ifstream inputFile;
    inputFile.open(argv[1]);
    std::string line;
    long sum = 0;
    std::vector<std::string> numbers({ "zero", "one", "two", "three", "four", "five", "six", "seven", "eight", "nine" });
    while (std::getline(inputFile, line)) {
        char first = 0;
        char last = 0;
        std::vector<char> digits;
        for (size_t i = 0; i < line.size(); i++) {
            if (isdigit(line[i])) {
                digits.push_back(line[i] - '0');
            } else {
                for (size_t j = 0; j < numbers.size(); j++) {
                    if (numbers[j] == line.substr(i, numbers[j].size())) {
                        digits.push_back(j);
                    }
                }
            }
        }
        first = digits.empty() ? 0 : digits[0];
        last = digits.empty() ? 0 : digits[digits.size() - 1];
        sum += 10 * first + last;
    }
    std::cout << sum;
    inputFile.close();
    return 0;
}
