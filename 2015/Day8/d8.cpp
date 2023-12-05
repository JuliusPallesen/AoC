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
    int ans = 0;
    int size = 0;
    while (std::getline(inputFile, line)) {
        size += line.size();
        int num = 0;
        for (size_t i = 1; i < line.size() - 1; ++i) {
            if (line[i] == '\\') {
                i++;
                if (line[i] == 'x') {
                    i += 2;
                }
            }
            num++;
        }
        ans += num;
    }

    inputFile.close();
    std::cout << size - ans;
    return 0;
}
