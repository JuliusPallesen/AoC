#include <algorithm>
#include <fstream>
#include <iostream>
#include <sstream>
#include <unordered_map>
#include <vector>

// needed to use positions in the hash map
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

// returns a vector of the positions of all possible neighbouts (connecting or not) for a given piipe piece
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

// recursively visits all pipes that are part of the loop. Does some initialization work, like removing 
// non-connecting pipes from the neighbours vector and marks pipes as being "part of the loop"
void visitPipes(std::unordered_map<std::pair<int, int>, pipe, pair_hash>& pipes, std::pair<int, int> pos, int dist = 0)
{
    if (pipes.count(pos) > 0) {
        pipes[pos].part_of_loop = true;
        pipes[pos].distance = dist;
        for (auto it = pipes[pos].neighbours.begin(); it != pipes[pos].neighbours.end();) {
            const auto& n = *it;
            if (pipes.count(n) > 0) {
                if (std::find(pipes[n].neighbours.begin(), pipes[n].neighbours.end(), pos) != pipes[n].neighbours.end()) {
                    if (pipes[n].distance > dist + 1) {
                        visitPipes(pipes, n, dist + 1);
                    }
                    ++it;
                } else {
                    it = pipes[pos].neighbours.erase(it);
                }
            } else {
                it = pipes[pos].neighbours.erase(it);
            }
        }
    }
}

// given a Position in the 2D input char vector this floodfills all neighbouring positions recursively
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

// Safe vector access. Could probably be removed by now, but i'm scared :<
void safeCharChange(std::vector<std::string>& chars, const int y, const int x, const char c)
{
    if (x >= 0 && y >= 0 && y < chars.size() - 1 && x < chars[y].size() - 1) {
        chars[y][x] = c;
    }
}

// Scales up the input 2D char array by 2 and fills empty spaces with .'s
std::vector<std::string> getDoubleSize(const std::vector<std::string>& chars)
{
    std::vector<std::string> doubledChars(chars.size() * 2);
    for (int y = 0; y < chars.size(); ++y) {
        doubledChars[y * 2] = std::string(chars[y].size() * 2, '.');
        for (size_t x = 0; x < chars[y].size(); x++) {
            doubledChars[y * 2][x * 2] = chars[y][x];
        }
        doubledChars[y * 2 + 1] = std::string(chars[0].size() * 2, '.');
    }
    return doubledChars;
}

// Parses a string vector into a map of pipes and retrieves the starting position
void parseLine(std::unordered_map<std::pair<int, int>, pipe, pair_hash>& pipes, const std::string& line, std::pair<int, int>* start, int y)
{
    for (int x = 0; x < line.size(); ++x) {
        if (line[x] != '.') {
            pipe p;
            p.distance = INT32_MAX;
            p.position = { y, x };
            p.type = line[x];
            if (line[x] == 'S') {
                *start = { y, x };
                p.distance = 0;
            }
            p.neighbours = getNeighbours(p.position, p.type);
            pipes[p.position] = p;
        }
    }
}

// Fills gaps created by the upscaling. Only straight pipe pieces are needed
// the x and y offsets between all neighbours (we know about neighbours from the previous initialization (part 1))
// tell us wether to fill the gap with a horizontal or vertical pipe.
void fillCharGaps(const std::unordered_map<std::pair<int, int>, pipe, pair_hash>& pipes, std::vector<std::string>& chars)
{
    for (auto& p : pipes) {
        for (auto& n : p.second.neighbours) {
            int y_offset = n.first - p.first.first;
            int x_offset = n.second - p.first.second;
            if (y_offset == 0) {
                safeCharChange(chars, 2 * p.first.first, 2 * p.first.second + x_offset, '-');
            }
            if (x_offset == 0) {
                safeCharChange(chars, 2 * p.first.first + y_offset, 2 * p.first.second, '|');
            }
        }
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
        parseLine(pipes, line, &start, y); // fills pipes map and finds the start
        y++;
    }
    visitPipes(pipes, start);

    // create a scaled up version of the input and fill the empty spaces with .'s 
    std::vector<std::string> doubledChars = getDoubleSize(chars);

    // fill up the gaps created inside of our loop by scaling them up
    fillCharGaps(pipes, doubledChars);

    // initialize "greater" loop
    std::unordered_map<std::pair<int, int>, pipe, pair_hash> doublepipes;
    for (int y = 0; y < doubledChars.size(); ++y) {
        parseLine(doublepipes, doubledChars[y], &start, y);
    }
    visitPipes(doublepipes, start);

    //Brute force all 9 neighbours and choose the most logical looking answer......
    for (int i = -1; i <= 1; i++) {
        for (int j = -1; j <= 1; j++) {
            std::vector<std::string> dc(doubledChars);
            floodfill(dc, doublepipes, { start.first + i, start.second + j }, 'I');
            for (int y = 0; y < dc.size(); ++y) {
                for (int x = 0; x < dc[y].size(); ++x) {
                    // because we doubled our chars size we only consider I's on even indices
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
