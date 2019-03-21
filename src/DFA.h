//
// Created by manom on 2/03/2019.
//

#ifndef DFADOT_DFA_H
#define DFADOT_DFA_H

#include <vector>
#include <map>
#include <algorithm>
#include <fstream>
#include <assert.h>
#include <iostream>
#include <typeinfo>
#include "./json.h"

class DFA {
private:

    struct State {
        std::map<char, State*> transition;
        std::string name;

        State(std::string stateName) {name = stateName;};
        bool operator==(const State &s) {
            if (name == s.name) return true;
            return false;
        }
    };

    std::vector<char> alphabet;
    std::vector<State*> states;
    State* startState;
    State* currentState;
    std::vector<State*> endStates;
    void transition(char c);
    bool properlyInitialized;

public:
    DFA(std::string filename);

    bool inputString(std::string s);

    void printNFA(std::string filename);
};


#endif //DFADOT_DFA_H
