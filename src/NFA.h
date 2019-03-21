//
// Created by manom on 2/03/2019.
//

#ifndef DFADOT_NFA_H
#define DFADOT_NFA_H

#include <vector>
#include <map>
#include <algorithm>
#include <fstream>
#include <assert.h>
#include <iostream>
#include <typeinfo>
#include "./json.h"


class NFA {
private:

    struct State {
        std::map<char, std::vector<State*>> transition;
        std::string name;

        State(std::string stateName) {name = stateName;};
        bool operator==(const State &s) {
            if (name == s.name) return true;
            return false;
        }
    };

    std::vector<char> alphabet;
    std::vector<State*> states;
    std::vector<State*> startState;
    std::vector<State*> endStates;
    std::vector<State*> currentState;
    void transition(char c);
    bool properlyInitialized = false;
public:
    NFA(std::string filename);

    bool inputString(std::string s);

    void printNFA(std::string filename);
};


#endif //DFADOT_NFA_H
