#include <fstream>
#include <iostream>
#include <sstream>

#include <algorithm>

int main(int argc, char const* argv[])
{
    std::ifstream inputFile;
    inputFile.open(argv[1]);
    std::string line;
    long floor = 0;
    char ch;
    int i = 1;
    while (inputFile >> ch) {
        if (ch == '(') {
            floor++;
        } else {
            floor--;
        }
        if (floor == -1) {
            break;
        }
        i++;
    }
    inputFile.close();
    std::cout << i;
    return 0;
}
