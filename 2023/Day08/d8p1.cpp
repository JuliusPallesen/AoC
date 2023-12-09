#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <algorithm>
#include <unordered_map>

struct NetworkNode {
    std::string name;
    NetworkNode * left;
    NetworkNode * right;
};

NetworkNode * getNode(const std::string & nodename , std::unordered_map<std::string, NetworkNode *> & nodes) {
    if(nodes.count(nodename) > 0) {
        return nodes[nodename];
    } else {
        nodes[nodename] = new NetworkNode();
        nodes[nodename]->name = nodename;
        return nodes[nodename];
    }
}

int main(int argc, char const *argv[])
{
    std::ifstream inputFile;
    inputFile.open(argv[1]);
    std::string line;
    std::string instructions;
    std::unordered_map<std::string, NetworkNode *> nodes;
    int ans = 0;
    bool found = false;
    std::getline(inputFile,instructions);

    while(std::getline(inputFile,line)) {
        if(!line.empty()) {
            NetworkNode * node = getNode(line.substr(0, line.find(' ')), nodes);
            line = line.substr(line.find('=') + 1, line.size());
            std::string left = line.substr(line.find('(') + 1, line.find(',') - line.find('(') - 1);
            std::string right = line.substr(line.find(',') + 2, line.find(')') - line.find(',') - 2);
            node->left = getNode(left, nodes);
            node->right = getNode(right,nodes);
        }
    }

    NetworkNode * ptr = nodes["AAA"];

    int i = 0;
    while(ptr->name != "ZZZ") {
        if(instructions[i % (instructions.size())] == 'L') {
            ptr = ptr->left;
        } else {
            ptr = ptr->right;
        }
        i++;
    }

    inputFile.close();
    std::cout << i ;
    return 0;
}
