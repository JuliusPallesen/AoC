#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <algorithm>
#include <optional>

int ids = 0;

struct Vec3D
{
    long double x, y, z;

    Vec3D(std::string s)
    {
        std::replace(s.begin(), s.end(), ',', ' ');
        std::stringstream ss(s);
        ss >> x >> y >> z;
    }
    Vec3D(long double _x, long double _y, long double _z) : x(_x), y(_y), z(_z) {}

    bool operator==(const Vec3D &other)
    {
        return x == other.x && y == other.y && z == other.z;
    }

    Vec3D operator+(const Vec3D &other) const
    {
        return {x + other.x, y + other.y, z + other.z};
    }

    Vec3D operator-(const Vec3D &other) const
    {
        return {x - other.x, y - other.y, z - other.z};
    }

    Vec3D operator*(const long double mult) const
    {
        return {x * mult, y * mult, z * mult};
    }

    Vec3D cross(const Vec3D &other) const
    {
        return {y * other.z - z * other.y, z * other.x - x * other.z, x * other.y - y * other.x};
    }

    long double dot(const Vec3D &other) const
    {
        return x * other.x + y * other.y + z * other.z;
    }

    friend std::ostream &operator<<(std::ostream &out, const Vec3D &v)
    {
        return out << v.x << ", " << v.y << ", " << v.z;
    }
};

struct Hail
{
    int id;
    Vec3D p, v;

    Hail(const std::string &s) : id(ids++), p(s.substr(0, s.find('@') - 1)), v(s.substr(s.find('@') + 1))
    {
    }

    static std::string idToName(int id)
    {
        constexpr int charNum = 'Z' - 'A';
        if (id < charNum)
        {
            const char c = 'A' + static_cast<char>(id);
            return std::string(1, c);
        }
        else
        {
            return idToName(id % charNum) + idToName(id / charNum);
        }
    }

    friend std::ostream &operator<<(std::ostream &out, const Hail &h)
    {
        return out << "Hailstone " << idToName(h.id) << ": " << h.p << " @ " << h.v;
    }
};

std::optional<Vec3D> findIntersection3D(const Hail &hail1, const Hail &hail2)
{
    double det = hail2.v.cross(hail1.v).dot(hail2.v.cross(hail1.v));

    if (std::abs(det) < 1e-6)
    {

        return std::nullopt;
    }

    double x = ((hail2.p.y - hail1.p.y) * hail2.v.cross(hail1.v).x -
                (hail2.p.x - hail1.p.x) * hail2.v.cross(hail1.v).y) /
               det;

    Vec3D intersectionPoint = hail1.p + hail1.v * x;

    return intersectionPoint;
}

std::optional<Vec3D> findIntersection2D(const Hail &hail1, const Hail &hail2)
{
    // Convert hail positions and velocities to Vec3D objects (ignoring or setting z dimension to zero)
    Vec3D as(hail1.p.x, hail1.p.y, 0);
    Vec3D bs(hail2.p.x, hail2.p.y, 0);
    Vec3D ad(hail1.v.x, hail1.v.y, 0);
    Vec3D bd(hail2.v.x, hail2.v.y, 0);

    if (as == bs)
    {
        return as;
    }
    auto dx = bs.x - as.x;
    auto dy = bs.y - as.y;
    auto det = bd.x * ad.y - bd.y * ad.x;
    if (det != 0)
    {
        long double u = (dy * bd.x - dx * bd.y) / (long double)det;
        long double v = (dy * ad.x - dx * ad.y) / (long double)det;
        if (u >= 0 && v >= 0)
        {
            return as + ad * u;
        }
    }
    return std::nullopt;
}

bool isInBoundaries(const Vec3D &v, int64_t s, int64_t e, bool p1 = false)
{
    return (v.x <= e && v.x >= s) &&
           (v.y <= e && v.y >= s) &&
           (p1 || (v.z <= e && v.z >= s));
}

int main(int argc, char const *argv[])
{
    std::ifstream inputFile;
    inputFile.open(argv[1]);
    std::string line;
    long double ans = 0;
    std::vector<Hail> hailstorm;
    while (std::getline(inputFile, line))
    {
        Hail hail(line);
        hailstorm.push_back(hail);
    }
    inputFile.close();

    for (size_t i = 0; i < hailstorm.size(); i++)
    {
        for (size_t j = 0; j < i; j++)
        {
            const auto intersection = findIntersection3D(hailstorm[i], hailstorm[j]);
            if (intersection && isInBoundaries(*intersection, 200000000000000, 400000000000000))
            {
                std::cout << hailstorm[i] << " intersects " << hailstorm[j] << " at: " << *intersection << "\n";
                ans++;
            }
        }
    }

    std::cout << ans;
    return 0;
}
