#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <algorithm>

int main(int argc, char const *argv[])
{
    std::ifstream inputFile;
    inputFile.open(argv[1]);
    std::string line;
    int64_t ans = 0;
    while (std::getline(inputFile, line))
    {
        (void)0; // do something
    }
    inputFile.close();
    std::cout << ans;
    return 0;
}
