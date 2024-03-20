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

    unsigned int &operator[](const char c)
    {
        switch (c)
        {
        case 'x':
            return x;
        case 'm':
            return m;
        case 'a':
            return a;
        case 's':
            return s;
        default:
            std::cerr << "not a variable.\n";
            exit(EXIT_FAILURE);
        }
    }

private:
};

class Rule
{
public:
    friend StepFactory;
    Rule() : operand(0), value(0), var(0) {}
    ~Rule() {}
    std::shared_ptr<Step> apply(MachinePart &m)
    {
        if (operand)
        {
            if (operand == '<')
            {
                return m[var] < value ? target : nullptr;
            }
            else
            {
                return m[var] > value ? target : nullptr;
            }
        }
        return target;
    };
    char operand;
    char var;
    int value;
    std::shared_ptr<Step> target;
};

class Step
{
public:
    friend StepFactory;
    virtual bool isValid(MachinePart &m) { return false; };
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
    bool isValid(MachinePart &m) override
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
    bool isValid(MachinePart &m) override
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
    bool isValid(MachinePart &m) override
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
        }
        else
        {
            r->var = rulestr[0];
            r->operand = rulestr[rulestr.find_first_of("<>")];
            r->value = stoi(rulestr.substr(rulestr.find_first_of("<>") + 1, rulestr.find(":") - 1));
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
            part[str[0]] = val;
        }
        if (sf.getFirstStep()->isValid(part))
            ans += part.getSum();
    }
    inputFile.close();
    std::cout << ans;
    return 0;
}
