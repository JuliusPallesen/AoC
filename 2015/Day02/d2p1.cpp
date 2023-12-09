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
        ans += s1 + s2 + s3 + std::min(std::min(s1, s2), s3) / 2;
    }
    inputFile.close();
    std::cout << ans;
    return 0;
}
