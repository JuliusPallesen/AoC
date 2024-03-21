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
#include <optional>
#include <numeric>

constexpr bool HIGH = true;
constexpr bool LOW = false;

using State = bool;

class ModuleEnvironment;

struct Signal
{
    std::string source, destination;
    State state;

    Signal(std::string s, std::string d, State b) : source(s), destination(d), state(b) {}
};

class Module
{
public:
    friend ModuleEnvironment;
    virtual std::vector<Signal> processSignal(const Signal &s) = 0;
    virtual void addIncomingConnection(const std::string &module){};
    virtual ~Module() {}

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

private:
    bool m_state;
};

class Conjunction : public Module
{
public:
    friend ModuleEnvironment;
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

private:
    std::unordered_map<std::string, State> states;

    State getState()
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
        m_lastState = s.state;
        return generateOutgoingSignals(s.state);
    }

private:
    State m_lastState;
};

class ModuleEnvironment
{
public:
    uint64_t buttonPresses = 0;

    void addModule(const std::string &input)
    {
        if (input[0] == '%')
        {
            makeModules<FlipFlop>(input);
        }
        else
        {
            (input[0] == '&') ? makeModules<Conjunction>(input) : makeModules<Broadcaster>(input);
        }
    }

    void pushButton()
    {
        if (buttonPresses == 0)
            setUprxInputCycles();

        buttonPresses++;
        signalQueue.emplace("button", "broadcaster", LOW);
        evaluateSignals();
    }

    uint64_t getResult()
    {
        return highCount * lowCount;
    }

    /*
    Check if all rx inputs pulsed at least once.
    */
    bool allCyclesInstanciated()
    {
        return (buttonPresses > 0 && std::all_of(rxInputCycles.begin(), rxInputCycles.end(), [](const auto &p)
                                                 { return p.second > 0; }));
    }

    /*
    We can calculate the first pulse of the rx module based on the assumption that the connected
    Conjunction Modules connected to rx pulse in cycles and calculating the lowest common multiple of said cycles.
    */
    uint64_t calcFirstRXPulse()
    {
        return std::accumulate(rxInputCycles.begin(), rxInputCycles.end(), static_cast<uint64_t>(1),
                               [](uint64_t acc, const auto &val)
                               { return std::lcm(acc, val.second); });
    }

private:
    std::unordered_map<std::string, std::vector<std::string>> waiting;
    std::unordered_map<std::string, uint64_t> rxInputCycles;
    std::string rxInput;
    uint64_t highCount = 0;
    uint64_t lowCount = 0;
    std::unordered_map<std::string, std::unique_ptr<Module>> modules;
    std::queue<Signal> signalQueue;

    void evaluateSignals()
    {
        while (!signalQueue.empty())
        {
            Signal s = signalQueue.front();
            signalQueue.pop();
            s.state ? ++highCount : ++lowCount;
            if (modules.count(s.destination))
            {
                if (rxInputCycles.count(s.source) && s.state == HIGH && rxInputCycles[s.source] == 0) 
                    rxInputCycles[s.source] = buttonPresses; // adding the button pushes it took for the rxInput to pulse (cycle)

                const auto & out = modules[s.destination]->processSignal(s);
                std::for_each(out.begin(),out.end(),[&](const Signal & s){signalQueue.push(s);});
            }
        }
    }

    void setUprxInputCycles()
    {
        const auto conPtr = dynamic_cast<Conjunction *>(modules[rxInput].get()); // not pretty, i know...
        std::transform(conPtr->states.begin(), conPtr->states.end(), std::inserter(rxInputCycles, rxInputCycles.end()),
               [](const auto &input) { return std::make_pair(input.first, 0); });
    }

    template <typename T, typename std::enable_if<std::is_base_of<Module, T>::value>::type * = nullptr>
    void makeModules(const std::string &input)
    {
        const auto name = (std::is_same<T, Broadcaster>::value) ? "broadcaster" : getName(input); // meh
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
            if (d == "rx")
                rxInput = ptr->m_name;

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

    while (!modEnv.allCyclesInstanciated())
    {
        modEnv.pushButton();
        if (modEnv.buttonPresses == 1000)
            std::cout << "part1: " << modEnv.getResult() << "\n";
    }
    std::cout << "part2: " << modEnv.calcFirstRXPulse() << "\n";
    return 0;
}