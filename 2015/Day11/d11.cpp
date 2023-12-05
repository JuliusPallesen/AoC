#include <algorithm>
#include <fstream>
#include <iostream>
#include <sstream>
#include <vector>

std::string incPass(std::string password)
{
    int i = password.size() - 1;
    bool done = false;
    while (!done) {
        if (!((password[i] + 1) > 'z')) {
            password[i] = (password[i] + 1);
            done = true;
        } else {
            password[i] = 'a';
        }
        i--;
    }
    return password;
}

int main(int argc, char const* argv[])
{
    std::ifstream inputFile;
    inputFile.open(argv[1]);
    std::string line;
    std::getline(inputFile, line);
    bool found = false;
    int ans;
    while (!found) {
        line = incPass(line);
        bool hasThreeAscending = false;
        bool hasTwoPairs = false;
        bool hasNoIOL = true;
        int lastPairPos = -1;
        for (size_t i = 0; i < line.size(); i++) {
            hasNoIOL = (line[i] == 'i' || line[i] == 'o' || line[i] == 'l') ? false : hasNoIOL;
            if (i >= 1) {
                if (line[i] == line[i - 1]) {
                    hasTwoPairs = (lastPairPos != -1 && i - lastPairPos > 1) ? true : hasTwoPairs;
                    lastPairPos = i;
                }
                if (i >= 2) {
                    hasThreeAscending = (line[i] == line[i - 1] + 1 && line[i] == line[i - 2] + 2) ? true : hasThreeAscending;
                }
            }
            found = hasThreeAscending && hasTwoPairs && hasNoIOL;
        }
    }
    inputFile.close();
    std::cout << line;
    return 0;
}
