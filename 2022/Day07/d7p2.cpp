#include <string>
#include <fstream>
#include <algorithm>
#include <array>
#include <iostream>
#include <ranges>
#include <vector>
#include <variant>
#include <sstream>
#include <set>
#include <unordered_map>
#include <memory>
#include <numeric>

constexpr int MAX_VAL = 100000;
constexpr int STORAGE_SPACE = 70000000;
constexpr int UNUSED_SPACE_NEEDED = 30000000;

// Probably switch to variant now
struct Directory;

struct FSO
{
    std::string name;
    std::shared_ptr<Directory> parent;
    virtual int getSize() = 0;
    virtual int getSumLessThanMaxVal() = 0;
    virtual int getBestToDelete(const int needed) = 0;
    virtual ~FSO() = default;

protected:
    FSO(std::string &&n, std::shared_ptr<Directory> &&p) : name(n), parent(p) {}
};

struct File : public FSO
{
    File(std::string &&n, std::shared_ptr<Directory> p, int sz) : FSO(std::move(n), std::move(p)), size(sz) {}
    int getSize() override { return size; }
    int getSumLessThanMaxVal() override { return 0; }
    int getBestToDelete(const int needed) override { return INT_MAX; }

private:
    int size;
};

struct Directory : public FSO
{
    Directory(std::string &&n, std::shared_ptr<Directory> p) : FSO(std::move(n), std::move(p)) {}
    std::set<std::shared_ptr<FSO>> contents;
    int getSize() override
    {
        return std::accumulate(contents.begin(), contents.end(), 0, [](int acc, const auto &child)
                               { return acc + child->getSize(); });
    }
    int getSumLessThanMaxVal() override
    {
        const int childSum = std::accumulate(contents.begin(), contents.end(), 0, [](int acc, const auto &child)
                                             { return acc + child->getSumLessThanMaxVal(); });
        const int mySize = getSize();
        return (mySize > MAX_VAL) ? childSum : childSum + mySize;
    }
    int getBestToDelete(const int needed) override
    {
        const int mySize = getSize();
        const int me = (mySize < needed) ? 0 : mySize;
        int min = INT_MAX;
        std::ranges::for_each(contents, [&min,&needed](const auto &c)
                              {
            int v = c->getBestToDelete(needed);
            min = std::min(min, (v > needed) ? v : INT_MAX); });
        return std::min(me, min);
    }
};

int main(int argc, char const *argv[])
{
    std::fstream inputFile(argv[1]);
    std::string line;
    std::string dummy;
    std::string name;
    std::shared_ptr<Directory> dir;
    std::shared_ptr<Directory> root = nullptr;
    int ans = 0;
    while (std::getline(inputFile, line))
    {
        std::cout << line << "\n";
        std::stringstream ss(line);
        // SWITCH TO REGEX!!!!
        if (line[0] == '$')
        {
            std::string command;
            ss >> dummy >> command;
            if (command == "cd")
            {
                ss >> name;
                if (name != "..")
                {
                    const std::shared_ptr<Directory> d_ptr = std::make_shared<Directory>(std::move(name), dir);
                    if (dir == nullptr)
                    {
                        root = d_ptr;
                    }
                    else
                    {
                        dir->contents.insert(d_ptr);
                    }
                    dir = d_ptr;
                }
                else
                {
                    dir = dir->parent;
                }
            }
        }
        else
        {
            ss >> dummy >> name;
            if (dummy == "dir")
            {
                dir->contents.emplace(std::make_unique<Directory>(std::move(name), dir));
            }
            else
            {
                int sz = std::stoi(dummy);
                dir->contents.emplace(std::make_unique<File>(std::move(name), dir, sz));
            }
        }
    }
    std::cout << root->getBestToDelete(UNUSED_SPACE_NEEDED - (STORAGE_SPACE - root->getSize()));
    return EXIT_SUCCESS;
}
