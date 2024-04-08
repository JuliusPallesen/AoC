#include <iostream>
#include <iomanip>
#include <fstream>
#include <vector>
#include <array>
#include <algorithm>
#include <utility>
#include <map>
#include <queue>
#include <set>
#include <cmath>
#include <cstdio>
#include <cstring>
#include <functional>
#include <numeric>
#include <unordered_set>
#include <cassert>
#include <sstream>
#include <queue>
#include <bitset>
#include "Eigen/Dense"

Eigen::Matrix3d crossMatrix(Eigen::Vector3d v)
{
    Eigen::Matrix3d result;
    result << 0, -v[2], v[1],
        v[2], 0, -v[0],
        -v[1], v[0], 0;
    return result;
}

int main()
{
    struct Hailstone
    {
        Eigen::Vector3d p;
        Eigen::Vector3d v;
    };

    std::vector<Hailstone> hailstones;
    while (true)
    {
        Hailstone h;
        char dummy;
        std::cin >> h.p[0] >> dummy >> h.p[1] >> dummy >> h.p[2] >> dummy >> h.v[0] >> dummy >> h.v[1] >> dummy >> h.v[2];

        if (!std::cin)
            break;
        hailstones.push_back(h);
    }

    Eigen::MatrixXd M(6, 6);
    Eigen::VectorXd rhs(6);
    rhs.segment<3>(0) = -hailstones[0].p.cross(hailstones[0].v) + hailstones[1].p.cross(hailstones[1].v);
    rhs.segment<3>(3) = -hailstones[0].p.cross(hailstones[0].v) + hailstones[2].p.cross(hailstones[2].v);
    M.block<3, 3>(0, 0) = crossMatrix(hailstones[0].v) - crossMatrix(hailstones[1].v);
    M.block<3, 3>(3, 0) = crossMatrix(hailstones[0].v) - crossMatrix(hailstones[2].v);
    M.block<3, 3>(0, 3) = -crossMatrix(hailstones[0].p) + crossMatrix(hailstones[1].p);
    M.block<3, 3>(3, 3) = -crossMatrix(hailstones[0].p) + crossMatrix(hailstones[2].p);
    Eigen::Vector3d result = M.inverse() * rhs;
    std::cout << "pos: " << result.segment<3>(0) << " vel: " << result.segment<3>(3) << std::endl;
    int sum = 0;
    for (int i = 0; i < 3; i++)
        sum += static_cast<int>(result[i]);
    std::cout << sum << std::endl;
}