#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <algorithm>
#include <unordered_map>
#include <unordered_set>

using StringGraphMap = std::unordered_map<std::string, std::unordered_set<std::string>>;

struct Connection
{
    std::string a, b;

    bool operator==(const Connection &other) const
    {
        return (a == other.a && b == other.b) || (a == other.b && b == other.a);
    }
};

void printMap(const StringGraphMap &mep)
{
    for (auto &&p : mep)
    {
        std::cout << p.first << " : ";
        for (auto &&tgt : p.second)
        {
            std::cout << tgt << " ";
        }
        std::cout << "\n";
    }
}

int floodFillGraph(
    StringGraphMap &mep,
    const std::string &key,
    const std::vector<Connection> &cut_cons,
    std::unordered_set<std::string> &reached)
{
    int count = 1;
    if (reached.count(key) > 0)
    {
        return 0;
    }
    reached.insert(key);
    for (const auto &tgt : mep[key])
    {
        Connection curr_con = {key, tgt};
        if (std::find(cut_cons.begin(), cut_cons.end(), curr_con) == cut_cons.end())
        {
            count += floodFillGraph(mep, tgt, cut_cons, reached);
        }
    }
    return count;
}

/*
maybe rank nodes in the graph by "connectivity" and use that order to improve the brute force

pretty sure there are certain clustering algorithms that would allow for the seperation of the graph into 2 groups
    - give nodes a position
    - iteratively:
        - increase distance to nodes that are not connected
        - decrease distance to nodes that are connected
*/
int main(int argc, char const *argv[])
{
    std::ifstream inputFile;
    inputFile.open(argv[1]);
    std::string line;
    StringGraphMap mep;
    std::vector<Connection> cons;
    int ans = 0;
    while (std::getline(inputFile, line))
    {
        std::string name = line.substr(0, line.find(':'));
        std::stringstream ss(line.substr(line.find(':') + 1));
        std::string tgt;
        while (ss >> tgt)
        {
            cons.push_back({name, tgt});
            mep[tgt].insert(name);
            mep[name].emplace(std::move(tgt));
        }
    }
    inputFile.close();

    std::cout << "cons amount:" << cons.size() << "\n";

    std::sort(cons.begin(), cons.end(), [&mep](const Connection &a, const Connection &b)
              { return (mep[a.a].size() + mep[a.b].size()) < (mep[b.a].size() + mep[b.b].size()); });

    int elems = mep.size();

    std::cout << "amount = " << elems << "\n";
    for (const auto &i : cons)
    {
        std::cout << "con i: " << i.a << "/" << i.b << " priority: " << mep[i.a].size() + mep[i.b].size() << "\n";
        int ji = 0;
        for (const auto &j : cons)
        {
            if (!(i == j))
            {
                std::cout << ji++ << "/" << cons.size() << "\n";
                std::cout << "con j: " << j.a << "/" << j.b << " priority: " << mep[j.a].size() + mep[j.b].size() << "\n";
                for (const auto &k : cons)
                {
                    if (!(i == k) && !(k == j))
                    {
                        std::unordered_set<std::string> reached;
                        std::string first_elem = mep.begin()->first;
                        std::vector<Connection> cut_cons = {i, j, k};
                        int group_size = floodFillGraph(mep, first_elem, cut_cons, reached);
                        if (group_size < elems - 1 && group_size != 1)
                        {
                            for (auto &&con : cut_cons)
                            {
                                std::cout << con.a << "/" << con.b << " ";
                            }
                            std::cout << "\n";
                            ans = group_size * (elems - group_size);
                        }
                    }
                    if (ans != 0)
                        break;
                }
            }
            if (ans != 0)
                break;
        }
        if (ans != 0)
            break;
    }

    std::cout << ans;
    return 0;
}
