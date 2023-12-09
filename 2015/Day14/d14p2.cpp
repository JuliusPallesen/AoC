#include <algorithm>
#include <fstream>
#include <iostream>
#include <sstream>
#include <stdio.h>
#include <unordered_map>
#include <vector>

struct ReindeerState {
    std::string name;
    int velocity;
    int time;
    int resting;
    int points = 0;
    int travelled = 0;
    int cooldown = 0;
    bool rest = false;
};

static const int SECONDS = 2503;

void travelOneSecond(ReindeerState* r)
{
    r->cooldown++;

    if (!r->rest) {
        r->travelled += r->velocity;
        if (r->cooldown >= r->time) {
            r->cooldown = 0;
            r->rest = true;
        }
    } else if (r->cooldown >= r->resting) {
        r->cooldown = 0;
        r->rest = false;
    }
}

int main(int argc, char const* argv[])
{
    std::ifstream inputFile;
    inputFile.open(argv[1]);
    std::string line;
    int ans = 0;
    std::vector<ReindeerState*> deers;
    while (std::getline(inputFile, line)) {
        char name[128];
        int v = 0;
        int s = 0;
        int r = 0;
        if (sscanf(line.c_str(), "%s can fly %d km/s for %d seconds, but then must rest for %d seconds.", name, &v, &s, &r)) {
            deers.push_back(new ReindeerState({ name, v, s, r, 0, 0, 0, false }));
        }
    }

    for (size_t i = 0; i < SECONDS; i++) {
        for (auto & d : deers) {
            travelOneSecond(d);
        }
        (*std::max_element(deers.begin(), deers.end(), [](auto& r1, auto& r2) {
            return r1->travelled <= r2->travelled;
        }))->points++;
    }

    for(ReindeerState * r : deers) {
        std::cout << r->name << ": " << r->points << "\n";
        std::cout << r->velocity << " " << r->time << " "<< r->resting << " "<< r->travelled << " "<< r->cooldown << "\n\n";
    }

    inputFile.close();

    std::cout << (*std::max_element(deers.begin(), deers.end(), [](auto& r1, auto& r2) {
        return r1->points <= r2->points;
    }))->points;
    return 0;
}
