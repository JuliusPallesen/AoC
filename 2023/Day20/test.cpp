#include <vector>
#include <unordered_map>
#include <iostream>

struct TestFact
{
    std::vector<std::string> strs;
    std::unordered_map<int, std::string &> mep;
    int i = 0;
    void addStr(std::string string)
    {
        strs.push_back(string);
        mep[i++] = strs[strs.size() - 1];
    }
};

int main(int argc, char const *argv[])
{
    TestFact tf;

    tf.addStr("Hello World!\n");
    std::cout << tf.mep[0];

    return 0;
}
