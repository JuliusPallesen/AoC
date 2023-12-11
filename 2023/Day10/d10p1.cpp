#include <algorithm>
#include <fstream>
#include <iostream>
#include <sstream>
#include <unordered_map>
#include <vector>

struct pair_hash {
    template <class T1, class T2>
    std::size_t operator()(const std::pair<T1, T2>& p) const
    {
        auto h1 = std::hash<T1> {}(p.first);
        auto h2 = std::hash<T2> {}(p.second);
        return h1 ^ h2;
    }
};

struct pipe {
    char type;
    std::pair<int, int> position;
    std::vector<std::pair<int, int>> neighbours;
    int distance;
};

std::vector<std::pair<int, int>> getNeighbours(std::pair<int, int> pos, char type)
{
    std::vector<std::pair<int, int>> n;
    switch (type) {
    case '|':
        n.push_back({ pos.first - 1, pos.second });
        n.push_back({ pos.first + 1, pos.second });
        break;
    case 'J':
        n.push_back({ pos.first - 1, pos.second });
        n.push_back({ pos.first, pos.second - 1 });
        break;
    case 'F':
        n.push_back({ pos.first + 1, pos.second });
        n.push_back({ pos.first, pos.second + 1 });
        break;
    case 'L':
        n.push_back({ pos.first - 1, pos.second });
        n.push_back({ pos.first, pos.second + 1 });
        break;
    case '7':
        n.push_back({ pos.first + 1, pos.second });
        n.push_back({ pos.first, pos.second - 1 });
        break;
    case '-':
        n.push_back({ pos.first, pos.second + 1 });
        n.push_back({ pos.first, pos.second - 1 });
        break;
    case 'S':
        n.push_back({ pos.first + 1, pos.second });
        n.push_back({ pos.first - 1, pos.second });
        n.push_back({ pos.first, pos.second + 1 });
        n.push_back({ pos.first, pos.second - 1 });
        break;
    default:
        break;
    }
    return n;
};

void visitPipes(std::unordered_map<std::pair<int, int>, pipe, pair_hash>& pipes, std::pair<int, int> pos, int dist = 0)
{
    if (pipes.count(pos) > 0) {
        pipes[pos].distance = dist;
        for (auto& n : pipes[pos].neighbours) {
            if (pipes.count(n) > 0) {
                if (std::find(pipes[n].neighbours.begin(), pipes[n].neighbours.end(), pos) != pipes[n].neighbours.end()) {
                    if (pipes[n].distance > dist + 1) {
                        visitPipes(pipes, n, dist + 1);
                    }
                } 
            } 
        }
    }
}

int main(int argc, char const* argv[])
{
    std::ifstream inputFile;
    inputFile.open(argv[1]);
    std::string line;
    std::pair<int, int> start;
    std::unordered_map<std::pair<int, int>, pipe, pair_hash> pipes;
    int ans = 0;
    int y = 0;
    while (std::getline(inputFile, line)) {
        for (int x = 0; x < line.size(); ++x) {
            if (line[x] != '.') {
                pipe p;
                p.distance = INT32_MAX;
                p.position = { y, x };
                p.type = line[x];
                if (line[x] == 'S') {
                    start = {y, x};
                    p.distance = 0;
                }
                p.neighbours = getNeighbours(p.position, p.type);
                pipes[p.position] = p;
            }
        }
        y++;
    }

    visitPipes(pipes, start);

    for (auto& p : pipes) {
        if(p.second.distance != INT32_MAX) {
            ans = std::max(ans, p.second.distance);
        }
    }

    inputFile.close();
    std::cout << ans;
    return 0;
}
