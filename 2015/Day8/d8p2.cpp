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
        for (size_t i = 0; i < line.size(); ++i) {
            if (line[i] == '\\' || line[i] == '"') {
                num++;
            }
            num++;
        }
        ans += num + 2;
    }

    inputFile.close();
    std::cout << ans - size;
    return 0;
}
