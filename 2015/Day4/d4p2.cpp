#include "./hash-library/md5.cpp"
#include <algorithm>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <vector>

int main(int argc, char const* argv[])
{
    std::ifstream inputFile;
    inputFile.open(argv[1]);
    std::string line;
    std::string linebuf;
    int ans = 0;
    inputFile.close();
    MD5 md5;
    std::cin >> line;
    bool found = false;
    while (!found) {
        ans++;
        linebuf = line + std::to_string(ans);
        std::string hash = md5(linebuf.c_str());
    	found = hash.substr(0, 6) == "000000";
    }
    std::cout << ans;
    return 0;
}
