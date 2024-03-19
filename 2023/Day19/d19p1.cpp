#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <algorithm>
#include <string>
#include <unordered_map>
#include <functional>
#include <memory>

class StepFactory;
class Step;

class MachinePart
{
public:
    unsigned int x, m, a, s;
    MachinePart() : x(0), m(0), a(0), s(0){};
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
    friend StepFactory;
    Rule() {}
    ~Rule() {}
    std::shared_ptr<Step> apply(const MachinePart &m)
    {
        return (condition(m)) ? target : nullptr;
    };
    std::function<bool(const MachinePart &)> condition;
    std::shared_ptr<Step> target;
};

class Step
{
public:
    friend StepFactory;
    virtual bool isValid(const MachinePart &m) { return false; };
    void addRule(std::shared_ptr<Rule> rule)
    {
        rules.push_back(rule);
    }
    std::vector<std::shared_ptr<Rule>> rules;
    std::string name;
    Step() = default;
    virtual ~Step() = default;
};

class ConcreteStep : public Step
{
public:
    friend StepFactory;
    bool isValid(const MachinePart &m) override
    {
        for (auto &&r : rules)
        {
            std::shared_ptr<Step> next = r->apply(m);
            if (next)
            {
                return next->isValid(m);
            }
        }
        return false;
    }
};

class AcceptingStep : public Step
{
public:
    friend StepFactory;
    bool isValid(const MachinePart &m) override
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
    bool isValid(const MachinePart &m) override
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
                r->target = step;
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
            r->condition = [](const MachinePart &m)
            { return true; };
        }
        else
        {
            std::function<bool(int, int)> comp = getCompFun(rulestr);
            r->condition = getCondition(rulestr, comp);
            target = rulestr.substr(rulestr.find(':') + 1, rulestr.size());
        }
        tryToAddTarget(r, target);
        return r;
    }

    void tryToAddTarget(std::shared_ptr<Rule> &r, std::string target)
    {
        if (steps.count(target))
        {
            r->target = steps[target];
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

    std::function<bool(const MachinePart &)> getCondition(std::string rulestr, std::function<bool(int, int)> comp)
    {
        std::string numstr = rulestr.substr(rulestr.find_first_of("<>") + 1, rulestr.find(":") - 1);
        int value = stoi(numstr);
        switch (rulestr.substr(0, rulestr.find_first_of("<>"))[0])
        {
        case 'x':
            return [comp, value](const MachinePart &m)
            { return comp(m.x, value); };
            break;
        case 'm':
            return [comp, value](const MachinePart &m)
            { return comp(m.m, value); };
            break;
        case 'a':
            return [comp, value](const MachinePart &m)
            { return comp(m.a, value); };
            break;
        case 's':
            return [comp, value](const MachinePart &m)
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

    while (std::getline(inputFile, line))
    {
        line = line.substr(1, line.size() - 1);
        std::stringstream ss(line);
        std::string str;
        MachinePart part;
        while (std::getline(ss, str, ','))
        {
            int val = stoi(str.substr(str.find('=') + 1, str.size()));
            switch (str[0])
            {
            case 'x':
                part.x = val;
                break;
            case 'm':
                part.m = val;
                break;
            case 'a':
                part.a = val;
                break;
            case 's':
                part.s = val;
                break;
            }
        }
        if (sf.getFirstStep()->isValid(part))
            ans += part.getSum();
    }
    inputFile.close();
    std::cout << ans;
    return 0;
}
