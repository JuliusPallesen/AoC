#include <fstream>
#include <iostream>
#include <sstream>
#include <vector>

int main(int argc, char const* argv[])
{
    std::ifstream inputFile;
    inputFile.open(argv[1]);
    std::string line;
    int ans;
    static const int N = 50;
    std::getline(inputFile, line);
    for (int n = 0; n < N; ++n) {
        std::string newstr;
        char c = line[0];
        int count = 0;
        for (size_t i = 0; i <= line.size(); i++) {
            if (c != line[i] || i == line.size()) {
                newstr += std::to_string(count) + c;
                count = 1;
                if (i != line.size())
                    c = line[i];
            } else {
                count++;
            }
        }
        line = newstr;
    }

    inputFile.close();
    std::cout << line.size();
    return 0;
}
