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
public:

    struct State {
        std::map<char, State*> transition;
        std::string name;
        bool final = false;

        State(std::string stateName) {name = stateName;};
        bool operator==(const State &s) {
            if (name == s.name) return true;
            return false;
        }
    };

    DFA()= default;

    DFA(std::string filename);

    bool inputString(std::string s);

    void convertToDot(std::string filename);

    void minimizeDfa(std::string outputname);

    void convertToJson(std::string filename);

    bool isEquivalentTo(const DFA &dfa, std::string outputname);

    // getters and setters
    const std::vector<State *> &getStates() const;

    const std::vector<char> &getAlphabet() const;

    void setAlphabet(const std::vector<char> &alphabet);

    void setStates(const std::vector<State *> &states);

    void setStartState(State *startState);

    void setEndStates(const std::vector<State *> &endStates);

    State *getStartState() const;

    const std::vector<State *> &getEndStates() const;

    bool isProperlyInitialized() const;

    void setProperlyInitialized(bool properlyInitialized);

    void addState(State* state);

    void addEndState(State* state);


private:
    std::vector<char> alphabet;
    std::vector<State*> states;
    State* startState;
    State* currentState;
    std::vector<State*> endStates;
    void transition(char c);
    bool checkInEqClass(const std::vector<std::vector<State*>> &eqClasses, State* stateToCheck);
    bool properlyInitialized;

};


#endif //DFADOT_DFA_H
