//
// Created by manom on 2/03/2019.
//

#include "./NFA.h"

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
    if (!properlyInitialized) return false;
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

void NFA::convertToDot(std::string filename) {
    if (!properlyInitialized) return;
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

void NFA::sscHelper(DFA* d, SubState crState) {
    // add transitions
    for (char c:d->getAlphabet()) {

        SubState temp;
        if (crState.consistsof.size() == 3 and c == 'a') {
            std::cout << "hello";
        }
        // for every state that the current state consists of
        for (auto &state:crState.consistsof) {

            // add all states to where the states of the subset state go to to the new substate
            for (auto &transState:state->transition[c]) {

                // check if the new subset doesnt already contain the element
                if (temp.findState(transState)) continue;
                temp.consistsof.emplace_back(transState);

            }
        }

        // check if the state is a dead state
        if (temp.consistsof.empty()) {

            // check if the dead state already exists
            bool check = false;
            for (auto &substate:subStates) {
                if (substate == temp) {
                    check = true;
                }

            }

            if (!check) {
                // create dead state
                DFA::State* tempDfaState = new DFA::State("\"{}\"");
                d->addState(tempDfaState);
                temp.dfastate = tempDfaState;
                tempDfaState->final = false;
                subStates.emplace_back(temp);

                // create transitions for dead state
                for (char ca:d->getAlphabet()) {
                    tempDfaState->transition[ca] = tempDfaState;
                }
            }


        }
        // check if no new state exists already for the transition, if not add the new substate
        bool exists=false;
        for (auto &state:subStates) {
            if (temp == state) {
                // make the transition to that state
                crState.dfastate->transition[c] = state.dfastate;
                exists=true;
            }
        }
        if (exists) continue;

        // create a new dfa state for the substate
        std::string newName = "\"{";
        bool final = false;
        for (auto state:temp.consistsof) {
            newName += state->name;
            if (state->name != temp.consistsof.back()->name) newName += ",";
            if (state->final) final = true;
        }
        newName += "}\"";
        DFA::State* tempDfaState = new DFA::State(newName);

        d->addState(tempDfaState);
        temp.dfastate = tempDfaState;
        tempDfaState->final = final;

        // make the transition from the current state to this one
        crState.dfastate->transition[c] = tempDfaState;

        subStates.emplace_back(temp);
        sscHelper(d, temp);

    }
}

DFA* NFA::convertToDfa(std::string filename) {

   DFA* d = new DFA;
   d->setAlphabet(alphabet);
   std::vector<DFA::State*> dfaStates;

    // start state
    SubState tempState;
    tempState.consistsof.emplace_back(startState[0]);
    // make a dfa state for the new startstate
    std::string newName = "\"{" + startState[0]->name + "}\"";
    DFA::State* tempDfaState = new DFA::State(newName);
    d->addState(tempDfaState);
    tempState.dfastate = tempDfaState;
    tempDfaState->final = startState[0]->final;
    subStates.emplace_back(tempState);

    // do subset construction + lazy evaluation
    sscHelper(d, tempState);

    // set start and final states for the dfa
    d->setStartState(d->getStates()[0]);
    for (auto &state:d->getStates()) {
        if (state->final) d->addEndState(state);
    }

    d->setProperlyInitialized(true);
    return d;
}

NFA::NFA(std::string filename) {
    std::ifstream configDoc(filename, std::ifstream::binary);
    Json::Value root;
    Json::Reader reader;
    reader.parse(configDoc, root, false);
    std::string type = root["type"].asString();
    if (type != "NFA") {
        std::cerr <<  "failed to construct nfa: type is not NFA" << std::endl;
        return;
    }

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
            temp->final=true;
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
    properlyInitialized = true;
}
