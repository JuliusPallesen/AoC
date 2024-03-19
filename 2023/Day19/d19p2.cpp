#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <algorithm>
#include <string>
#include <unordered_map>
#include <functional>
#include <memory>

/*
- Somehow inverse the tree.
    - Rules point to their Step
    - Steps point to all ingoing Rules

- create a new part structure that can represent ranges of values

- Start at A
- iterate through all rules that point to A (have it as its target)
- apply the rule to the range of the machine part
- visit allincoming rules of the rules assosiated step

- end at in. return (product of?) Ranged RangedMachinePart
*/

class StepFactory;
class Step;

class RangedMachinePart
{
public:
    struct range
    {
        int from;
        int to;
    };
    RangedMachinePart()
    {
        ranges['x'] = {0, 4000};
        ranges['m'] = {0, 4000};
        ranges['a'] = {0, 4000};
        ranges['s'] = {0, 4000};
    }
    std::unordered_map<char, range> ranges;
    uint64_t getProd()
    {
        uint64_t prod = 1;
        for (const auto &range : ranges)
        {
            prod *= (range.second.to - range.second.from);
        }
        return prod;
    };
};

class Rule
{
public:
    friend StepFactory;
    Rule() {}
    ~Rule() {}
    int apply(RangedMachinePart m)
    {
        return parent && restrictFunc(m) ? parent->apply(m) : 0;
    };
    std::function<bool(RangedMachinePart &)> restrictFunc;
    std::shared_ptr<Step> parent;
};

class Step
{
public:
    friend StepFactory;
    virtual int apply(RangedMachinePart m) { return 0; };
    std::vector<std::shared_ptr<Rule>> predecessors;
    std::string name;
    Step() = default;
    virtual ~Step() = default;

    void addPredecessor(std::shared_ptr<Rule> rule)
    {
        predecessors.push_back(rule);
    }
};

class ConcreteStep : public Step
{
public:
    friend StepFactory;
    int apply(RangedMachinePart m) override
    {
        for (auto &&pred : predecessors)
        {
            RangedMachinePart m2 = m;
            pred->apply(m2);
        }

        if (name == "in")
        {
            return true;
        }
    }
};

class AcceptingStep : public Step
{
public:
    friend StepFactory;
    int apply(RangedMachinePart m) override
    {
        return true;
    }

    AcceptingStep() { name = "A"; }
    ~AcceptingStep() {}
};

class RejectingStep : public Step
{
public:
    friend StepFactory;
    int apply(RangedMachinePart m) override
    {
        return false;
    }
    RejectingStep() { name = "R"; }
    ~RejectingStep() {}
};

class StepFactory
{
public:
    StepFactory()
    {
        steps["A"] = std::make_shared<AcceptingStep>();
        steps["R"] = std::make_shared<RejectingStep>();
    }
    ~StepFactory() {}

    void makeStep(std::string stepstr)
    {
        std::shared_ptr<Step> step = std::make_shared<ConcreteStep>();
        step->name = stepstr.substr(0, stepstr.find('{'));
        addRules(step, stepstr.substr(stepstr.find('{') + 1, stepstr.size() - stepstr.find('{') - 2));
        notifyWaiters(step);
        steps[step->name] = step;
    }

    std::shared_ptr<Step> getFirstStep()
    {
        return steps["in"];
    }

private:
    std::unordered_map<std::string, std::vector<std::shared_ptr<Rule>>> waiting;
    std::unordered_map<std::string, std::shared_ptr<Step>> steps;

    void notifyWaiters(std::shared_ptr<Step> step)
    {
        if (waiting.count(step->name))
        {
            for (auto &&r : waiting[step->name])
            {
                //r->target = step;
            }
            waiting.erase(step->name);
        }
    }

    std::shared_ptr<Rule> makeRule(std::string rulestr)
    {
        std::shared_ptr<Rule> r = std::make_shared<Rule>();
        std::string target;
        if (rulestr.find(":") == rulestr.npos)
        {
            target = rulestr;
            r->restrictFunc = [](const RangedMachinePart &m)
            { return true; };
        }
        else
        {
            std::function<bool(int, int)> comp = getCompFun(rulestr);
            r->restrictFunc = getrestrictFunc(rulestr, comp);
            target = rulestr.substr(rulestr.find(':') + 1, rulestr.size());
        }
        tryToAddTarget(r, target);
        return r;
    }

    void tryToAddTarget(std::shared_ptr<Rule> &r, std::string target)
    {
        if (steps.count(target))
        {
            r->parent = steps[target];
        }
        else
        {
            waiting[target].push_back(r);
        }
    }

    void addRules(std::shared_ptr<Step> step, std::string rulesstr)
    {
        std::string str;
        std::stringstream ss(rulesstr);
        while (std::getline(ss, str, ','))
        {
            step->addRule(makeRule(str));
        }
    }

    std::function<bool(int, int)> getCompFun(const std::string &rulestr)
    {
        std::function<bool(int, int)> comp;
        if (rulestr.find('<') != rulestr.npos)
        {
            comp = [](int a, int b)
            { return a < b; };
        }
        else
        {
            comp = [](int a, int b)
            { return a > b; };
        }
        return comp;
    }

    std::function<bool(const RangedMachinePart &)> getrestrictFunc(std::string rulestr, std::function<bool(int, int)> comp)
    {
        std::string numstr = rulestr.substr(rulestr.find_first_of("<>") + 1, rulestr.find(":") - 1);
        int value = stoi(numstr);
        switch (rulestr.substr(0, rulestr.find_first_of("<>"))[0])
        {
        case 'x':
            return [comp, value](const RangedMachinePart &m)
            { return comp(m.x, value); };
            break;
        case 'm':
            return [comp, value](const RangedMachinePart &m)
            { return comp(m.m, value); };
            break;
        case 'a':
            return [comp, value](const RangedMachinePart &m)
            { return comp(m.a, value); };
            break;
        case 's':
            return [comp, value](const RangedMachinePart &m)
            { return comp(m.s, value); };
            break;
        default:
            std::cerr << "unexpected char : " << rulestr.substr(0, rulestr.find("<>"))[0] << "\n";
            exit(EXIT_FAILURE);
            break;
        }
        return {};
    }
};

int main(int argc, char const *argv[])
{
    std::ifstream inputFile;
    inputFile.open(argv[1]);
    std::string line;
    int ans = 0;
    StepFactory sf;
    while (std::getline(inputFile, line) && !line.empty())
    {
        sf.makeStep(line);
    }
    RangedMachinePart rmp();
    inputFile.close();
    std::cout << ans;
    return 0;
}
