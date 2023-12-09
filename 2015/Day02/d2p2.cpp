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
    long ans = 0;
    int x, y, z;
    char c;
    while (inputFile >> x >> c >> y >> c >> z) {
        int s1 = 2 * x * y;
        int s2 = 2 * y * z;
        int s3 = 2 * x * z;
        int ribbon = 2 * (x + y + z) - 2 * std::max(std::max(x, y), z) + x * y * z;
        ans += ribbon;
    }
    inputFile.close();
    std::cout << ans;
    return 0;
}
