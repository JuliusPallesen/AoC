#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <algorithm>
#include <queue>
#include <unordered_map>
#include <memory>
#include <sstream>
#include <string>
#include <type_traits>

constexpr bool HIGH = true;
constexpr bool LOW = false;

using State = bool;

class ModuleEnvironment;

struct Signal
{
    std::string source, destination;
    State state;

    Signal(std::string s, std::string d, State b) : source(s), destination(d), state(b) {}

    void print() const
    {
        std::cout << source << " -" << ((state) ? "high-> " : "low-> ") << destination << "\n";
    }
};

class Module
{
public:
    friend ModuleEnvironment;
    virtual std::vector<Signal> processSignal(const Signal &s) = 0;
    virtual void addIncomingConnection(const std::string &module){};
    virtual ~Module() {}
    virtual void print() {}

protected:
    std::string m_name;
    std::vector<std::string> m_destinations;

    Module(std::string name, std::vector<std::string> destinations) : m_name(std::move(name)),
                                                                      m_destinations(std::move(destinations)) {}

    std::vector<Signal> generateOutgoingSignals(State s)
    {
        std::vector<Signal> out;
        std::transform(m_destinations.begin(), m_destinations.end(), std::back_inserter(out), [&](const auto &d)
                       { return Signal(m_name, d, s); });
        return out;
    }
};

class FlipFlop : public Module
{
public:
    FlipFlop(std::string name, std::vector<std::string> destinations) : Module(name, destinations), m_state(LOW) {}

    std::vector<Signal> processSignal(const Signal &s) override
    {
        if (s.state == HIGH)
            return {};
        m_state = !m_state;
        return generateOutgoingSignals(m_state);
    }

    void print() override { std::cout << "FlipFlop: " << m_name << " state: " << (m_state ? "HIGH" : "LOW") << "\n"; }

private:
    bool m_state;
};

class Conjunction : public Module
{
public:
    Conjunction(std::string name, std::vector<std::string> destinations) : Module(name, destinations) {}
    void addIncomingConnection(const std::string &module) override
    {
        states[module] = LOW;
    }
    std::vector<Signal> processSignal(const Signal &s) override
    {
        states[s.source] = s.state;
        return generateOutgoingSignals(getState());
    }

    void print() override
    {
        std::cout << "Conjunction: " << m_name << "\n{";
        for (auto &&state : states)
        {
            std::cout << state.first << ": " << (state.second ? "High" : "Low") << ",";
        }
        std::cout << "}\n";
    }

private:
    std::unordered_map<std::string, State> states;
    bool getState()
    {
        for (auto &&slot : states)
        {
            if (slot.second == LOW)
                return HIGH;
        }
        return LOW;
    }
};

class Broadcaster : public Module
{
public:
    Broadcaster(std::string name, std::vector<std::string> destinations) : Module("broadcaster", destinations) {}
    std::vector<Signal> processSignal(const Signal &s) override
    {
        return generateOutgoingSignals(s.state);
    }

    void print() override { std::cout << "Broadcaster: " << m_name << "\n"; }
};

class ModuleEnvironment
{
public:
    std::unordered_map<std::string, std::unique_ptr<Module>> modules;
    std::queue<Signal> signalQueue;

    void addModule(const std::string &input)
    {
        if (input[0] == '%')
        {
            makeModules<FlipFlop>(input);
        }
        else
        {
            if (input[0] == '&')
            {
                makeModules<Conjunction>(input);
            }
            else
            {
                makeModules<Broadcaster>(input);
            }
        }
    }

    bool pushButton()
    {
        if (signalQueue.empty())
        {
            signalQueue.emplace("button", "broadcaster", LOW);
            return true;
        }
        return false;
    }

    void evaluateSignals()
    {
        while (!signalQueue.empty())
        {
            Signal s = signalQueue.front();
            signalQueue.pop();
            s.state ? ++highCount : ++lowCount;
            if (modules.count(s.destination))
            {
                const auto outSignals = modules[s.destination]->processSignal(s);
                for (const Signal &out : outSignals)
                {
                    signalQueue.push(out);
                }
            }
        }
    }

    uint64_t getResult()
    {
        std::cout << "High Signals: " << highCount << "\n";
        std::cout << "Low Signals: " << lowCount << "\n";
        return highCount * lowCount;
    }

    void printModules()
    {
        for (auto &&modPair : modules)
        {
            modPair.second->print();
        }
    }

private:
    std::unordered_map<std::string, std::vector<std::string>> waiting;
    uint64_t highCount = 0;
    uint64_t lowCount = 0;

    template <typename T, typename std::enable_if<std::is_base_of<Module, T>::value>::type * = nullptr>
    void makeModules(const std::string &input)
    {
        const auto name = (std::is_same<T, Broadcaster>::value) ? "broadcaster" : getName(input);
        modules[name] = std::make_unique<T>(name, getDestinations(input));
        notifyDestinations(modules[name]);
        notifyWaiters(modules[name]);
    }

    std::vector<std::string> getDestinations(const std::string &input)
    {
        std::vector<std::string> ret;
        std::string dests = input.substr(input.find(">") + 1);
        std::stringstream ss(dests);
        std::string d;
        while (std::getline(ss, d, ','))
        {
            ret.push_back(d.substr(1));
        }
        return ret;
    }

    std::string getName(const std::string &input)
    {
        return input.substr(1, input.find(' ') - 1);
    }

    void notifyDestinations(const std::unique_ptr<Module> &ptr)
    {
        for (const auto &d : ptr->m_destinations)
        {
            if (modules.count(d) > 0)
            {
                modules[d]->addIncomingConnection(ptr->m_name);
            }
            else
            {
                if (waiting.count(d) == 0)
                    waiting[d] = std::vector<std::string>();
                waiting[d].push_back(ptr->m_name);
            }
        }
    }

    void notifyWaiters(const std::unique_ptr<Module> &ptr)
    {
        if (waiting.count(ptr->m_name) == 0)
            return;
        for (const auto &waiter : waiting[ptr->m_name])
        {
            ptr->addIncomingConnection(waiter);
        }
        waiting.erase(ptr->m_name);
    }
};

int main(int argc, char const *argv[])
{
    std::ifstream inputFile;
    inputFile.open(argv[1]);
    std::string line;
    uint64_t ans = 0;
    ModuleEnvironment modEnv;
    while (std::getline(inputFile, line))
    {
        modEnv.addModule(line);
    }
    inputFile.close();

    for (size_t i = 0; i < 1000; i++)
    {
        modEnv.pushButton();
        modEnv.evaluateSignals();
    }
    std::cout << modEnv.getResult();
    return 0;
}