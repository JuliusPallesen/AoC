#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <algorithm>
#include <unordered_map>
#include <memory>
#include <optional>

class Step;
class StepFactory;

/*
MachinePartRanges represent possible solutions to the Problem
*/
struct MachinePartRange
{
    // default range 1-4000
    // could pick a smaller type here but too lazy to search for conversion errors
    struct Range
    {
        uint64_t from = 1;
        uint64_t to = 4000;
    };
    Range x, m, a, s;

    // Returns the number of possible combinations of variables for the ranges (watch out for the nasty off by 1 error)
    inline uint64_t getProd()
    {
        return (x.to - x.from + 1) * (m.to - m.from + 1) * (a.to - a.from + 1) * (s.to - s.from + 1);
    }

    inline bool isValid()
    {
        return (x.to >= x.from) && (m.to >= m.from) && (a.to >= a.from) && (s.to >= s.from);
    }

    // Scuffed []operator for easy variable access
    Range &operator[](const char c)
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
};

struct MachinePart
{
    uint32_t x, m, a, s;

    uint32_t getSum()
    {
        return x + m + a + s;
    }

    uint32_t &operator[](const char c)
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
};

struct Rule
{
    char operand;
    char var;
    int value;
    std::shared_ptr<Step> target;

    std::shared_ptr<Step> apply(MachinePart &m)
    {
        if (!operand)
            return target;

        if (operand == '<')
        {
            return m[var] < value ? target : nullptr;
        }
        else
        {
            return m[var] > value ? target : nullptr;
        }
    };
};

class Step
{
public:
    friend StepFactory;

    virtual bool isValidPart(MachinePart &m) { return false; }
    virtual uint64_t getPossibilities(MachinePartRange m) { return 0; }

protected:
    Step() {}
    virtual ~Step() {}
    std::vector<std::shared_ptr<Rule>> rules;
    std::string name;

private:
    void addRule(std::shared_ptr<Rule> rule)
    {
        rules.push_back(rule);
    }
};

class ConcreteStep : public Step
{
public:
    bool isValidPart(MachinePart &m) override
    {
        for (auto &&r : rules)
        {
            const std::shared_ptr<Step> next = r->apply(m);
            if (next)
                return next->isValidPart(m);
        }
        return false;
    }

    uint64_t getPossibilities(MachinePartRange m) override
    {
        uint64_t possibilities = 0;
        for (const auto &r : rules)
        {
            const auto next = getNext(m, r);
            if (next)
            {
                possibilities += r->target->getPossibilities(*next);
            }
            else
            {
                return possibilities + r->target->getPossibilities(m);
            }
        }
        return possibilities;
    }

private:
    /*
    Tries to apply a rule to a ranged machine part.
    if its not an empty operator we return a new range with only accepting values and modify the old
    range in a way that it only rejects the rule (done so you dont count ranges multiple times. a rule
    has to fail in order for you to reach the next rule in a step).
    */
    std::optional<MachinePartRange> getNext(MachinePartRange &m, const std::shared_ptr<Rule> &r)
    {
        if (!r->operand)
            return {};
        MachinePartRange next = m;
        if (r->operand == '<')
        {
            m[r->var].from = r->value;      // Rest: x < 1000 | x = 500-4000 ==> x = 1000-4000
            next[r->var].to = r->value - 1; // Next: x < 1000 | x = 500-4000 ==> x = 500-999
        }
        else
        {
            m[r->var].to = r->value;          // Rest: x > 1000 | x = 500-4000 ==> x = 500-1000
            next[r->var].from = r->value + 1; // Next: x > 1000 | x = 500-4000 ==> x = 1001-4000
        }
        return next;
    }
};

class AcceptingStep : public Step
{
public:
    AcceptingStep() { name = "A"; }

    bool isValidPart(MachinePart &m) override
    {
        return true;
    }

    uint64_t getPossibilities(MachinePartRange m) override
    {
        return m.getProd();
    }
};

class RejectingStep : public Step
{
public:
    RejectingStep() { name = "R"; }
};

/*
Used to build the tree structure of the Steps.

incase step's that are referenced by rules aren't created yet, rules can wait for said steps
and be notified once they are created. this way we can use references for out rules and steps.
probably still not that efficient...
*/
class StepFactory
{
public:
    StepFactory()
    {
        steps["A"] = std::make_shared<AcceptingStep>();
        steps["R"] = std::make_shared<RejectingStep>();
    }

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
        if (waiting.count(step->name) == 0)
            return;

        for (auto &&r : waiting[step->name])
            r->target = step;
        waiting.erase(step->name);
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
            step->addRule(makeRule(str));
    }
};

int main(int argc, char const *argv[])
{
    std::ifstream inputFile;
    inputFile.open(argv[1]);
    std::string line;
    uint64_t ans = 0;
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
            part[str[0]] = stoi(str.substr(str.find('=') + 1, str.size()));
        }
        if (sf.getFirstStep()->isValidPart(part))
            ans += part.getSum();
    }
    inputFile.close();

    std::cout << "p1: " << ans << "\n";

    MachinePartRange m;
    ans = sf.getFirstStep()->getPossibilities(m);
    std::cout << "p2: " << ans << "\n";
    return 0;
}
