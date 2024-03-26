#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <algorithm>
#include <functional>
#include <memory>
#include <set>

struct Point3D
{
    int x, y, z;

    Point3D(int xp, int yp, int zp) : x(xp), y(yp), z(zp)
    {
    }

    friend std::ostream &operator<<(std::ostream &out, const Point3D &p)
    {
        return out << "[" << p.x << "," << p.y << "," << p.z << "]";
    }

    static Point3D produceNewPoint(const Point3D &p1, const Point3D &p2, std::function<int(int, int)> fun)
    {
        return {fun(p1.x, p2.x), fun(p1.y, p2.y), fun(p1.z, p2.z)};
    }
};

struct Brick
{
    int m_id;
    Point3D start, end;

    std::set<int> supports;
    std::set<int> supported_by;

    Brick(Point3D corner1, Point3D corner2) : start(std::move(corner1)),
                                              end(std::move(corner2))
    {
    }
};

Point3D getCoords(std::string &input)
{
    std::replace(input.begin(), input.end(), ',', ' ');
    std::stringstream ss(input);
    std::string x, y, z;
    ss >> x >> y >> z;
    return {std::stoi(x), std::stoi(y), std::stoi(z)};
}

class BrickPile
{
public:
    BrickPile(std::vector<Brick> &bricks, Point3D dims) : m_bricks(std::move(bricks))
    {
        int i = 0;
        m_heights.resize(dims.x + 1, std::vector<int>(dims.y + 1, 0));
        m_space.resize(dims.x + 1, std::vector<std::vector<int>>(dims.y + 1));
        std::for_each(m_bricks.begin(), m_bricks.end(), [&](auto &b)
                      { 
                        b.m_id = ++i;
                        dropBrick(b); });
    }

    int countRemovableBricks()
    {
        return std::count_if(m_bricks.begin(), m_bricks.end(), [&](const auto &b)
                             { return std::all_of(b.supports.begin(), b.supports.end(), [&](const auto &id)
                                                  { return m_bricks[id - 1].supported_by.size() > 1; }); });
    }

    int getMostAmountofMayhem()
    {
        int sum = 0;
        for (auto &&b : m_bricks)
        {
            std::set<int> destroyed;
            sum += getChainReactionSize(b.m_id, destroyed) - 1;
        }
        return sum;
    }

private:
    std::vector<std::vector<int>> m_heights;
    std::vector<std::vector<std::vector<int>>> m_space;
    std::vector<Brick> m_bricks;
    int zSize = 0;

    void dropBrick(Brick &brick)
    {
        int collision = 0;
        for (int x = brick.start.x; x <= brick.end.x; x++)
        {
            for (int y = brick.start.y; y <= brick.end.y; y++)
            {
                collision = std::max(collision, m_heights[x][y]);
            }
        }
        placeBrick(brick, collision + 1);
        updateFloorHeights(brick);
        addToSupportBricks(brick);
    }

    int getChainReactionSize(int id, std::set<int> &destroyed)
    {
        if (destroyed.find(id) != destroyed.end())
        {
            return 0;
        }
        int ans = 1;
        destroyed.insert(id);
        for (auto &&supported : m_bricks[id - 1].supports)
        {
            const auto sb = m_bricks[supported - 1].supported_by;
            if (std::all_of(sb.begin(), sb.end(), [&](const int &i)
                            { return destroyed.find(i) != destroyed.end(); }))
            {
                ans += getChainReactionSize(supported, destroyed);
            }
        }
        return ans;
    }

    void placeBrick(Brick &brick, int z)
    {
        brick.end.z = brick.end.z - brick.start.z + z;
        brick.start.z = z;
        for (int x = brick.start.x; x <= brick.end.x; x++)
        {
            for (int y = brick.start.y; y <= brick.end.y; y++)
            {
                for (int z = brick.start.z; z <= brick.end.z; z++)
                {
                    if (z >= m_space[x][y].size())
                    {
                        m_space[x][y].resize(z + 1, 0);
                        zSize = std::max(zSize, z + 1);
                    }
                    m_space[x][y][z] = brick.m_id;
                }
            }
        }
    }

    void updateFloorHeights(Brick &brick)
    {
        for (int x = brick.start.x; x <= brick.end.x; x++)
        {
            for (int y = brick.start.y; y <= brick.end.y; y++)
            {
                m_heights[x][y] = brick.end.z;
            }
        }
    }

    void addToSupportBricks(Brick &brick)
    {
        for (int x = brick.start.x; x <= brick.end.x; x++)
        {
            for (int y = brick.start.y; y <= brick.end.y; y++)
            {
                const auto brickIDinSpace = m_space[x][y][brick.start.z - 1];
                if (brickIDinSpace != 0)
                {
                    m_bricks[brickIDinSpace - 1].supports.insert(brick.m_id);
                    brick.supported_by.insert(brickIDinSpace);
                }
            }
        }
    }
};

int main(int argc, char const *argv[])
{
    std::ifstream inputFile;
    inputFile.open(argv[1]);
    std::string line;
    int ans = 0;
    std::vector<Brick> bricks;
    Point3D maxDims(0, 0, 0);
    while (std::getline(inputFile, line))
    {
        std::string corner1 = line.substr(0, line.find('~'));
        std::string corner2 = line.substr(line.find('~') + 1);
        Brick brick(getCoords(corner1), getCoords(corner2));
        maxDims = Point3D::produceNewPoint(brick.end, maxDims, [](int a, int b)
                                           { return std::max(a, b); });

        bricks.push_back(brick);
    }
    inputFile.close();
    std::sort(bricks.begin(), bricks.end(), [](Brick &b1, Brick &b2)
              { return b1.start.z < b2.start.z; });

    BrickPile bp(bricks, maxDims);
    ans = bp.countRemovableBricks();
    std::cout << "part 1: " << ans << "\n";

    ans = bp.getMostAmountofMayhem();
    std::cout << "part 2: " << ans << "\n";
    return 0;
}
