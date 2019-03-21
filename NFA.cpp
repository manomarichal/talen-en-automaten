//
// Created by manom on 2/03/2019.
//

#include "NFA.h"

void NFA::transition(char c) {
    std::vector<State*> next;
    for (auto s: currentState) {
        for (auto n: s->transition[c]) {
            next.push_back(n);
        }
    }
    currentState = next;
}

bool NFA::inputString(std::string input) {
    currentState = {startState};
    for (char c: input) {
        if (std::find(alphabet.begin(), alphabet.end(), c) == alphabet.end()) {
            std::cerr << "input string contains an symbol that is not in the alphabet: " << c << std::endl;
            return false;
        }
        transition(c);
    }
    for (auto state: currentState) {
        if (std::find(endStates.begin(), endStates.end(), state) != endStates.end()) return true;
    }
    return false;
}

void NFA::printNFA(std::string filename) {
    std::ofstream outputFile("../output/" + filename);
    std::stringstream s;
    // states
    for (auto state:endStates) {
        s << "  " << state->name << "[peripheries=2]" << std::endl;
    }
    // edges
    for (auto state:states) {
        for (auto symbol:state->transition) {
            for (auto edge:symbol.second)
            s << "  " << state->name << "->" << edge->name << "[label=\"" << symbol.first << "\"];" << std::endl;
        }
    }

    // start state
    s << "  " << "head [style=invis]\n   head->" << startState[0]->name << std::endl;
    // end states

    outputFile << "digraph {\n" << s.str() << "}";
    outputFile.close();
}

NFA::NFA(std::string filename) {
    std::ifstream configDoc(filename, std::ifstream::binary);
    Json::Value root;
    Json::Reader reader;
    reader.parse(configDoc, root, false);
    std::string type = root["type"].asString();

    // read alphabet in
    const Json::Value &alphabetJson = root["alphabet"];
    for (int index = 0; index < alphabetJson.size(); ++index) {
        if (alphabetJson[index].asString().length() != 1) {
            std::cerr << "alphabet elements can only be strings of lenght 1" << std::endl;
            return;
        }
        alphabet.push_back(alphabetJson[index].asString()[0]);
    }

    // read states in
    const Json::Value &statesJson = root["states"];
    bool startStateCheck = false;
    for (int index = 0; index < statesJson.size(); ++index) {
        // check if a state with the same name already exists
        for (auto state: states) {
            if (state->name == statesJson[index]["name"].asString()) {
                std::cerr << "there can't be 2 or more states with the same name" << std::endl;
                return;
            }
        }
        State* temp = new State(statesJson[index]["name"].asString());
        if (statesJson[index]["starting"].asBool()) {
            if (startStateCheck) std::cerr << "there can only be one starting state" << std::endl;
            startState.push_back(temp);
            startStateCheck = true;
        }
        if (statesJson[index]["accepting"].asBool()) {
            endStates.push_back(temp);
        }
        states.push_back(temp);
    }
    // read in transitions
    for (auto state: states) {
        for (char c:alphabet) {
            state->transition[c] = {};
        }
    }
    const Json::Value &transitionsJson = root["transitions"];
    for (int index = 0; index < transitionsJson.size(); ++index) {
        for (auto state: states) {
            if (state->name == transitionsJson[index]["from"].asString()) {
                // check if the input is 1 symbol
                if (transitionsJson[index]["input"].asString().length() != 1) {
                    std::cerr << "input symbols can only be characters" << std::endl;
                    return;
                }
                // check if the alphabet contains the character
                if (std::find(alphabet.begin(), alphabet.end(), transitionsJson[index]["input"].asString()[0]) == alphabet.end()) {
                    std::cerr << "input symbols not found in alphabet" << std::endl;
                    return;
                }
                for (auto s: states) {
                    if (s->name == transitionsJson[index]["to"].asString()) {
                        state->transition[transitionsJson[index]["input"].asString()[0]].emplace_back(s);
                    }
                }

            }
        }
    }
}
