#include <algorithm>
#include <fstream>
#include <iostream>
#include <sstream>
#include <vector>

bool isVowel(const char c)
{
    return (c == 'a') || (c == 'e') || (c == 'i') || (c == 'o') || (c == 'u');
}

bool isIllegal(const char c, const char prev)
{
    if (c == 'b' || c == 'd' || c == 'q' || c == 'y') {
        return prev == c - 1;
    }
    return false;
}

int main(int argc, char const* argv[])
{
    std::ifstream inputFile;
    inputFile.open(argv[1]);
    std::string line;
    int ans = 0;
    while (std::getline(inputFile, line)) {
        int vowels = 0;
        bool hasDouble = false;
        bool containsIllegal = false;
        for (size_t i = 0; i < line.size() && !containsIllegal; i++) {
            vowels += isVowel(line[i]);
            if (i > 0) {
                hasDouble = line[i] == line[i - 1] ? true : hasDouble;
                containsIllegal = isIllegal(line[i], line[i - 1]) ? true : containsIllegal;
            }
        }
        if (vowels >= 3 && !containsIllegal && hasDouble) {
            ans++;
        }
    }

    inputFile.close();
    std::cout << ans;
    return 0;
}
