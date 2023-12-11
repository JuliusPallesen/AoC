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
    bool part_of_loop = false;
    std::pair<int, int> position;
    std::vector<std::pair<int, int>> neighbours;
    int distance;
};

void printChars(std::vector<std::string> chars)
{
    for (const std::string& s : chars) {
        std::cout << s << "\n";
    }
}

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
        pipes[pos].part_of_loop = true;
        for (auto& n : pipes[pos].neighbours) {
            if (pipes.count(n) > 0) {
                if (std::find(pipes[n].neighbours.begin(), pipes[n].neighbours.end(), pos) != pipes[n].neighbours.end()) {
                    if (pipes[n].distance > dist + 1) {
                        visitPipes(pipes, n, dist + 1);
                    }
                } else {
                    // in theory you could remove the neighbor here
                }
            }
        }
    }
}

int floodfill(std::vector<std::string>& chars, std::unordered_map<std::pair<int, int>, pipe, pair_hash>& pipes, std::pair<int, int> pos, char fill = ' ')
{
    if (pos.first < 0 || pos.second < 0 || pos.first >= chars.size() || pos.second >= chars[pos.first].size()) {
        return 0;
    }
    if (chars[pos.first][pos.second] == fill || (pipes.count(pos) > 0 && pipes[pos].part_of_loop)) {
        return 0;
    } else {
        chars[pos.first][pos.second] = fill;
        return 1 + floodfill(chars, pipes, { pos.first - 1, pos.second }, fill)
            + floodfill(chars, pipes, { pos.first + 1, pos.second }, fill)
            + floodfill(chars, pipes, { pos.first, pos.second - 1 }, fill)
            + floodfill(chars, pipes, { pos.first, pos.second + 1 }, fill);
    }
}

void safeCharChange(std::vector<std::string>& chars, const int y, const int x, const char c)
{
    if (x >= 0 && y >= 0 && y < chars.size() - 1  && x < chars[y].size() - 1) {
        chars[y][x] = c;
    }
}

int main(int argc, char const* argv[])
{
    std::ifstream inputFile;
    inputFile.open(argv[1]);
    std::string line;
    std::vector<std::string> chars;
    std::pair<int, int> start;
    std::unordered_map<std::pair<int, int>, pipe, pair_hash> pipes;
    int ans = 0;
    int y = 0;
    while (std::getline(inputFile, line)) {
        chars.push_back(line);
        for (int x = 0; x < line.size(); ++x) {
            if (line[x] != '.') {
                pipe p;
                p.distance = INT32_MAX;
                p.position = { y, x };
                p.type = line[x];
                if (line[x] == 'S') {
                    start = { y, x };
                    p.distance = 0;
                }
                p.part_of_loop = false;
                p.neighbours = getNeighbours(p.position, p.type);
                pipes[p.position] = p;
            }
        }
        y++;
    }

    visitPipes(pipes, start);

    std::vector<std::string> doubledChars(chars.size() * 2);
    for (int y = 0; y < chars.size(); ++y) {
        doubledChars[y * 2] = std::string(chars[y].size() * 2, '.');
        for (size_t x = 0; x < chars[y].size(); x++) {
            doubledChars[y * 2][x * 2] = chars[y][x];
        }
        doubledChars[y * 2 + 1] = std::string(chars[0].size() * 2, '.');
    }
    for (const auto& e : pipes) {
        int y = 2 * e.second.position.first;
        int x = 2 * e.second.position.second;
        switch (e.second.type) {
        case '|':
            safeCharChange(doubledChars, y - 1, x, '|');
            safeCharChange(doubledChars, y + 1, x, '|');
            break;
        case 'J':
            safeCharChange(doubledChars, y - 1, x, '|');
            safeCharChange(doubledChars, y, x - 1, '-');
            break;
        case 'F':
            safeCharChange(doubledChars, y + 1, x, '|');
            safeCharChange(doubledChars, y, x + 1, '-');
            break;
        case 'L':
            safeCharChange(doubledChars, y - 1, x, '|');
            safeCharChange(doubledChars, y, x + 1, '-');
            break;
        case '7':
            safeCharChange(doubledChars, y + 1, x, '|');
            safeCharChange(doubledChars, y, x - 1, '-');
            break;
        case '-':
            safeCharChange(doubledChars, y, x + 1, '-');
            safeCharChange(doubledChars, y, x - 1, '-');
            break;
        case 'S':
            safeCharChange(doubledChars, y, x + 1, '-');
            safeCharChange(doubledChars, y, x - 1, '-');
            safeCharChange(doubledChars, y + 1, x, '|');
            safeCharChange(doubledChars, y - 1, x, '|');
            break;
        default:
            break;
        }
    }

    
    std::unordered_map<std::pair<int, int>, pipe, pair_hash> doublepipes;
    for (int y = 0; y < doubledChars.size(); ++y) {
        for (int x = 0; x < doubledChars[y].size(); ++x) {
            if (doubledChars[y][x] != '.') {
                pipe p;
                p.distance = INT32_MAX;
                p.position = { y, x };
                p.type = doubledChars[y][x];
                if (doubledChars[y][x] == 'S') {
                    start = { y, x };
                    p.distance = 0;
                }
                p.neighbours = getNeighbours(p.position, p.type);
                doublepipes[p.position] = p;
            }
        }
    }

    visitPipes(doublepipes, start);
    
    for (int i = -1; i <= 1; i++) {
        for (int j = -1; j <= 1; j++) {
            std::vector<std::string> dc(doubledChars);
            floodfill(dc, doublepipes, { start.first + i, start.second + j }, 'I');

            for (int y = 0; y < dc.size(); ++y) {
                for (int x = 0; x < dc[y].size(); ++x) {
                    if (x % 2 == 0 && y % 2 == 0 && dc[y][x] == 'I') {
                        ans++;
                    }
                }
            }
            std::cout << ans << "\n";
            ans = 0;
        }
    }
    return 0;
}
