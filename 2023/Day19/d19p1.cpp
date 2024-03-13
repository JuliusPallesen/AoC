#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <algorithm>
#include <string>
#include <unordered_map>
#include <functional>

/*
1st eine liste von regeln
|--> eine Step besteht aus einem namen gefolgt von mehreren Regeln
    |--> Eine Regel besteht aus einer oder keiner kondition gefolgt von einer ziel, regel oder einem zustand
2nd Liste von teilen mit jwls. 4 eigenschaften
*/

class MachinePart
{
public:
    unsigned int x, m, a, s;
    MachinePart(int x, int m, int a, int s) : x(x), m(m), a(a), s(s){};
    ~MachinePart(){};

    unsigned int getSum()
    {
        return x + m + a + s;
    }

private:
};

class Rule
{
public:
    Rule(std::function<bool(unsigned int)> fun, std::string target) : condition(fun), target(target) {}
    ~Rule() {}

private:
    std::function<bool(unsigned int)> condition;
    std::string target;
};

class Step
{
public:
    void addRule(Rule rule)
    {
        rules.push_back(rule);
    }

private:
    std::vector<Rule> rules;
};

int main(int argc, char const *argv[])
{
    std::ifstream inputFile;
    inputFile.open(argv[1]);
    std::string line;
    int ans;
    inputFile.close();
    std::unordered_map<std::string, Step> steps;
    while(std::getline(inputFile, line)) {
        std::string name = line.substr(0,line.find('{'));
        std::stringstream ss(line);
    }


    std::cout << ans;
    return 0;
}
