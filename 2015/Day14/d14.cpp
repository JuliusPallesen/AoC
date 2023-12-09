#include <algorithm>
#include <fstream>
#include <iostream>
#include <sstream>
#include <stdio.h>
#include <vector>

int calcDistance(const int velocity, const int time, const int resting)
{
    std::cout << velocity << " " << time << " " << resting << "\n";
    const int SECONDS = 2503;
    bool rest = false;
    int travelled = 0;
    int t2 = 0;
    int time_spent = 0;
    while (time_spent <= SECONDS) {
        t2++;
        time_spent++;
        if (!rest) {
            travelled += velocity;
            if (t2 >= time) {
                t2 = 0;
                rest = true;
            }
        } else if (t2 >= resting) {
            t2 = 0;
            rest = false;
        }
    }
    std::cout << travelled << " ";
    return travelled;
}

int main(int argc, char const* argv[])
{
    std::ifstream inputFile;
    inputFile.open(argv[1]);
    std::string line;
    int ans = 0;
    while (std::getline(inputFile, line)) {
        char name[128];
        int v = 0;
        int s = 0;
        int r = 0;
        if (sscanf(line.c_str(), "%s can fly %d km/s for %d seconds, but then must rest for %d seconds.", name, &v, &s, &r)) {
            std::cout << "\n\n";
            std::cout << name << "\n";
            ans = std::max(ans, calcDistance(v, s, r));
        }
    }
    inputFile.close();

    std::cout << ans;
    return 0;
}
