#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <algorithm>
#include <numeric>

struct Point
{
    int x;
    int y;

    Point(int xval, int yval) : x(xval), y(yval)
    {
    }

    Point operator+(const Point &other)
    {
        return {x + other.x,
                y + other.y};
    }

    Point operator*(const int mult)
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
    case 'U':
        return {0, -1};
    case 'D':
        return {0, 1};
    case 'L':
        return {-1, 0};
    case 'R':
        return {1, 0};
    default:
        std::cerr << "Invalid direction\n";
        exit(EXIT_FAILURE);
    }
}

double shoelaceFormula(const std::vector<Point> &vertices)
{
    int n = vertices.size();
    double area = 0.0;

    for (int i = 0; i < n; ++i)
    {
        const Point &current = vertices[i];
        const Point &next = vertices[(i + 1) % n];

        area += (current.x * next.y - next.x * current.y);
    }

    return std::abs(area) / 2;
}

int picksTheoremI(int A, int b)
{
    // Picks Theorem: We know the perimiter size and can calculate the INNER area using shoelace. only need i.
    // A = i + (b / 2) - 1
    // -i = (b/2) - 1 -A
    //  i = A + 1 - (b / 2)
    return A + static_cast<int>(static_cast<double>(b) / 2.0f) + 1;
}

int main(int argc, char const *argv[])
{
    std::ifstream inputFile;
    inputFile.open(argv[1]);
    std::string line;
    int ans = 0;
    int p;
    std::vector<Point> points = {{0, 0}};
    while (std::getline(inputFile, line))
    {
        std::stringstream ss(line);
        std::string dir, numstr, color;
        ss >> dir >> numstr >> color;
        Point delta = getDirDelta(dir[0]) * stoi(numstr);
        p += std::abs(delta.x) + std::abs(delta.y);

        points.push_back(points[points.size() - 1] + delta);
    }
    inputFile.close();

    // Inner Area A
    int a = shoelaceFormula(points);
    ans = picksTheoremI(a, p);

    std::cout << ans;
    return 0;
}
