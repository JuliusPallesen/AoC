#include <algorithm>
#include <assert.h>
#include <fstream>
#include <iostream>
#include <sstream>
#include <unordered_map>
#include <vector>

static std::vector<char> cardValues({ 'A', 'K', 'Q', 'J', 'T', '9', '8', '7', '6', '5', '4', '3', '2' });

struct CardCount {
    char amount;
    char card;
};

struct CamelHand {
    std::string str;
    std::vector<CardCount> hand;
    int bid;
};

bool cmpCardGreater(const char c1, const char c2)
{
    return std::find(cardValues.begin(), cardValues.end(), c1) < std::find(cardValues.begin(), cardValues.end(), c2);
}

std::vector<CardCount> getHand(const std::string& hand)
{
    std::vector<CardCount> ret;
    std::unordered_map<char, char> map;
    for (char c : hand) {
        map[c]++;
    }

    for (auto& p : map) {
        ret.push_back({ p.second, p.first });
    }

    std::sort(ret.begin(), ret.end(), [](const auto& p1, const auto& p2) { return p1.amount > p2.amount; });
    return ret;
}

int main(int argc, char const* argv[])
{
    std::ifstream inputFile;
    inputFile.open(argv[1]);
    std::string line;
    uint64_t ans = 0;
    std::vector<CamelHand> hands;

    while (std::getline(inputFile, line)) {
        CamelHand hand;
        hand.str = line.substr(0, line.find(' '));
        hand.hand = getHand(hand.str);
        hand.bid = stoi(line.substr(line.find(' ') + 1, line.size()));
        hands.push_back(hand);
    }

    auto const handCmpr = [](const CamelHand& h1, const CamelHand& h2) {
        for (size_t i = 0; i < std::min(h1.hand.size(), h2.hand.size()); ++i) {
            if (h1.hand[i].amount < h2.hand[i].amount) {
                return true;
            }
            if (h1.hand[i].amount > h2.hand[i].amount) {
                return false;
            }
        }
    
        int i = 0;
        while (h1.str[i] == h2.str[i] && i < 5) {
            ++i;
        }
        return !cmpCardGreater(h1.str[i], h2.str[i]);
    };

    std::sort(hands.begin(), hands.end(), handCmpr);

    for (size_t i = 0; i < hands.size(); i++) {
        ans += (i + 1) * (uint64_t)hands[i].bid;
    }
    inputFile.close();
    std::cout << ans;
    return 0;
}
