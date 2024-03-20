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

/*
modules
% flipflop
has state
ignores high pulse
on low pulse invert state and send signal


& conjunction
remembers last received input for all incoming signals
when reveiving input
    -> update remembered signal in slot
    -> if all are high emit low
    -> else emit high

broadcaster
propagate signal


fifo queue for signals
*/

constexpr bool HIGH = true;
constexpr bool LOW = false;

using State = bool;

class ModuleEnvironment;

struct Signal
{
    Signal(std::string s, std::string d, State b) : source(s), destination(d), state(b) {}
    std::string source, destination;
    bool state;
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
    Broadcaster(std::vector<std::string> destinations) : Module("broadcaster", destinations) {}
    std::vector<Signal> processSignal(const Signal &s) override
    {
        return generateOutgoingSignals(s.state);
    }
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
                // makeModules<Broadcaster>(input);
            }
        }
    }

private:
    std::unordered_map<std::string, std::vector<std::string>> waiting;

    template <typename T>
    void makeModules(const std::string &input)
    {
        const std::string name = getName(input);
        modules[name] = std::make_unique<T>(name, getDestinations(input));
        notifyDestinations(modules[name]);
    }
    /*
    void makeModules<Broadcaster>(const std::string &input)
    {
        modules["broadcaster"] = std::make_unique<Broadcaster>(getDestinations(input));
        notifyDestinations(modules["broadcaster"]);
    }
    */

    std::vector<std::string> getDestinations(const std::string &input)
    {
        std::vector<std::string> ret;
        std::string dests = input.substr(input.find_last_of(" ") + 1);
        std::stringstream ss(dests);
        std::string d;
        while (std::getline(ss, d, ','))
        {
            ret.push_back(d);
        }
        return ret;
    }

    std::string getName(const std::string &input)
    {
        return input.substr(1, input.find(' ') - 1);
    }

    void notifyDestinations(const std::unique_ptr<Module> &ptr)
    {
        for (auto &&d : ptr->m_destinations)
        {
            (modules.count(d)) ? modules[d]->addIncomingConnection(ptr->m_name)
                               : waiting[d].push_back(ptr->m_name);
        }
    }
};

int main(int argc, char const *argv[])
{
    std::ifstream inputFile;
    inputFile.open(argv[1]);
    std::string line;
    int ans;
    inputFile.close();
    std::cout << ans;
    return 0;
}