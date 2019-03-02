//
// Created by manom on 2/03/2019.
//

#include "NFA.h"
void NFA::transition(char c) {
    std::vector<State> next, temp;
    for(auto s: currentState) {
        temp = s.transition.find(c)->second;
        for (auto n: temp) {
            next.push_back(n);
        }
    }
    currentState = next;
}

bool NFA::input(std::string s) {
    currentState = {startState};
    for (char c: s) {
        if (std::find(alphabet.begin(), alphabet.end(), c) != alphabet.end()) return false;
        transition(c);
    }
    for (State state: currentState) {
        if (std::find(endStates.begin(), endStates.end(), state) != endStates.end()) return true;
    }
    return false;
}

const std::vector<char> &NFA::getAlphabet() const {
    return alphabet;
}

void NFA::setAlphabet(const std::vector<char> &alphabet) {
    NFA::alphabet = alphabet;
}

const std::vector<State> &NFA::getStates() const {
    return states;
}

void NFA::setStates(const std::vector<State> &states) {
    NFA::states = states;
}

const State &NFA::getStartState() const {
    return startState;
}

void NFA::setStartState(const State &startState) {
    NFA::startState = startState;
}

const std::vector<State> &NFA::getEndStates() const {
    return endStates;
}

void NFA::setEndStates(const std::vector<State> &endStates) {
    NFA::endStates = endStates;
}
