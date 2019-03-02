//
// Created by manom on 2/03/2019.
//

#ifndef DFADOT_NFA_H
#define DFADOT_NFA_H

#include <vector>
#include <map>
#include <algorithm>
struct State {
    std::map<char, std::vector<State>> transition;
    std::string name;

    bool operator==(const State &s) {
        if (name == s.name) return true;
        return false;
    }
};

class NFA {
private:
    std::vector<char> alphabet;
    std::vector<State> states;
    State startState;
    std::vector<State> endStates;
    std::vector<State> currentState;
    void transition(char c);
public:
    const std::vector<char> &getAlphabet() const;

    void setAlphabet(const std::vector<char> &alphabet);

    const std::vector<State> &getStates() const;

    void setStates(const std::vector<State> &states);

    const State &getStartState() const;

    void setStartState(const State &startState);

    const std::vector<State> &getEndStates() const;

    void setEndStates(const std::vector<State> &endStates);

    bool input(std::string s);
};


#endif //DFADOT_NFA_H
