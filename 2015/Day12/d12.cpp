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
    std::getline(inputFile, line);
    int ans = 0;
    int num = 0;
    bool isNeg = false;
    for (int i = 0; i < line.size(); ++i) {
        if (isdigit(line[i])) {
            isNeg = (line[i - 1] == '-') ? true : isNeg;
            num = (10 * num) + (line[i] - '0');
        } else {
            if (num != 0) {
                ans = isNeg ? ans - num : ans + num;
                isNeg = false;
                num = 0;
            }
        }
    }

    inputFile.close();
    std::cout << ans;
    return 0;
}
