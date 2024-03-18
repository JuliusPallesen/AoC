#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <algorithm>
#include <numeric>

struct Point
{
    int64_t x;
    int64_t y;

    Point(int64_t xval, int64_t yval) : x(xval), y(yval)
    {
    }

    Point operator+(const Point &other)
    {
        return {x + other.x,
                y + other.y};
    }

    Point operator*(const int64_t mult)
    {
        return {x * mult, y * mult};
    }

    friend std::ostream &operator<<(std::ostream &out, const Point &p)
    {
        out << "[" << p.x << ", " << p.y << "]";
        return out;
    }
};

Point getDirDelta(char dir)
{
    switch (dir)
    {
    case 0:
        return {1, 0};
    case 1:
        return {0, 1};
    case 2:
        return {-1, 0};
    case 3:
        return {0, -1};

    default:
        std::cerr << "Invalid direction\n";
        exit(EXIT_FAILURE);
    }
}

double shoelaceFormula(const std::vector<Point> &vertices)
{
    int64_t n = vertices.size();
    int64_t area = 0;

    for (int64_t i = 0; i < n; ++i)
    {
        const Point &current = vertices[i];
        const Point &next = vertices[(i + 1) % n];

        area += (current.x * next.y - next.x * current.y);
    }

    return std::abs(area) / 2;
}

uint64_t picksTheoremI(uint64_t A, uint64_t b)
{
    // Picks Theorem
    // A = i + (b / 2) - 1
    //-i = (b/2) - 1 -A
    //  i = A + 1 - (b / 2)
    return A + (b / 2) + 1;
}

int main(int argc, char const *argv[])
{
    std::ifstream inputFile;
    inputFile.open(argv[1]);
    std::string line;
    uint64_t ans = 0;
    uint64_t p = 0;
    std::vector<Point> points = {{0, 0}};
    while (std::getline(inputFile, line))
    {
        std::stringstream ss(line);
        std::string dir, numstr, hexStr;
        int64_t hexVal, hexDir, hexAmount;
        ss >> dir >> numstr >> hexStr;
        hexStr = "0x" + hexStr.substr(2, hexStr.size() - 3);
        hexVal = std::stoul(hexStr, nullptr, 16);
        hexDir = (hexVal & 16 - 1);
        hexAmount = (hexVal >> 4);

        Point delta = getDirDelta(hexDir) * hexAmount;
        p += std::abs(delta.x) + std::abs(delta.y);
        Point newPoint = points[points.size() - 1] + delta;
        points.push_back(newPoint);
    }
    inputFile.close();

    // Inner Area A
    uint64_t a = shoelaceFormula(points);
    ans = picksTheoremI(a, p);

    std::cout << ans;
    return 0;
}
