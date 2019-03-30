//
// Created by mano on 30.03.19.
//

#ifndef DFADOT_ENFA_H
#define DFADOT_ENFA_H

#include <vector>
#include <map>
#include <algorithm>
#include <fstream>
#include <assert.h>
#include <iostream>
#include <typeinfo>
#include "./json.h"
#include "DFA.h"


class ENFA {
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

        void eclose(char epsilon) {
            for (auto state:this->consistsof) {
                for (auto trans:state->transition[epsilon]) {
                    if (this->findState(trans)) continue;
                    this->consistsof.emplace_back(trans);
                }
            }
        }
    };

    ENFA(std::string filename);

    bool inputString(std::string s);

    void convertToDot(std::string filename);

    DFA* convertToDfa();

private:

    char eps;

    std::vector<char> alphabet;
    std::vector<State*> states;
    std::vector<State*> startState;
    std::vector<State*> endStates;
    std::vector<State*> currentState;
    void transition(char c);
    bool properlyInitialized = false;

    std::vector<SubState> subStates;

    void sscHelper(DFA* d, SubState crState);

};


#endif //DFADOT_ENFA_H
