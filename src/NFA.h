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
#include "DFA.h"

class NFA {
public:

    struct State {
        std::map<char, std::vector<State*>> transition;
        std::string name;
        bool final=false;
        State(std::string stateName) {name = stateName;};
        bool operator==(const State &s) {
            if (name == s.name) return true;
            return false;
        }
    };

    NFA(std::string filename);

    bool inputString(std::string s);

    void convertToDot(std::string filename);

    DFA* convertToDfa(std::string filename);

private:

    std::vector<char> alphabet;
    std::vector<State*> states;
    std::vector<State*> startState;
    std::vector<State*> endStates;
    std::vector<State*> currentState;
    void transition(char c);
    bool properlyInitialized = false;

    // helpers used by subsetconstruction
    struct SubState {
        DFA::State* dfastate;
        std::vector<State*> consistsof;

        bool operator==(const SubState &right) {
            if (this->consistsof.size() != right.consistsof.size()) return false;
            for (auto &state:this->consistsof) {
                bool isIn = false;
                for (auto &rightState:right.consistsof) {
                    if (state->name == rightState->name) isIn = true;
                }
                if (!isIn) return false;
            }
            return true;
        }

        bool findState(State* state) {
            bool check = false;
            for (auto sstate:consistsof) {
                if (state->name == sstate->name) check = true;
            }
            return check;
        }
    };

    std::vector<SubState> subStates;

    void sscHelper(DFA* d, SubState crState);



};


#endif //DFADOT_NFA_H
