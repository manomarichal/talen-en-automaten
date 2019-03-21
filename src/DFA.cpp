//
// Created by manom on 2/03/2019.
//

#include "./DFA.h"
bool DFA::checkInEqClass(const std::vector<std::vector<State*>> &eqClasses, State* stateToCheck) {
    for (const auto &eqClass: eqClasses) {
        for (const auto &state: eqClass) {
            if (state == stateToCheck) return true;
        }
    }
    return false;
}

void DFA::transition(char c) {
    currentState = currentState->transition[c];
}

bool DFA::inputString(std::string input) {
    if (!properlyInitialized) return false;
    currentState = {startState};
    for (char c: input) {
        if (std::find(alphabet.begin(), alphabet.end(), c) == alphabet.end()) {
            std::cerr << "input string contains an symbol that is not in the alphabet: " << c << std::endl;
            return false;
        }
        transition(c);
    }
    if (std::find(endStates.begin(), endStates.end(), currentState) != endStates.end()) return true;
    return false;
}

void DFA::printDFA(std::string filename) {
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
            s << "  " << state->name << "->" << symbol.second->name<< "[label=\"" << symbol.first << "\"];" << std::endl;
        }
    }

    // start state
    s << "  " << "head [style=invis]\n   head->" << startState->name << std::endl;
    // end states

    outputFile << "digraph {\n" << s.str() << "}";
    outputFile.close();
}

void DFA::minimizeDfa() {
    if (!properlyInitialized) return;
    // deze map stelt de tabel voor met de paren, de bool geeft aan of het wel of niet onderscheidbaar is
    std::map<std::pair<State*, State*>, bool> checkAccesible;
    // add pairs to table
    for (int x=0; x < states.size(); x++) {
        for (int y=x+1; y < states.size(); y++) {
            checkAccesible[{states[x], states[y]}] = false;
        }
    }
    // base case, zet paren die uit 1 final state staan op onderscheidbaar
    for (const auto &pair:checkAccesible) {
        if ((pair.first.first->final and !pair.first.second->final) or
                (!pair.first.first->final and pair.first.second->final)) {
            checkAccesible[pair.first] = true;
        }
    }

    // inductive step
    while (true) {
        int pos=0; // used to check if we looped over all pairs without being able to mark one, if so we can end the inductive step
        for (const auto &pair:checkAccesible) {
            if (pair.second)  {
                pos++;
                continue;
            }
            bool startOver = false; // we start over the algorithm if we find a pair that we can mark
            for (auto c:alphabet) {

                // check if the pair exists within the table
                if (checkAccesible.count({pair.first.second->transition[c], pair.first.first->transition[c]})) {

                    // check if the pair is marked
                    if (checkAccesible[{pair.first.second->transition[c], pair.first.first->transition[c]}]) {

                        //mark the pair
                        checkAccesible[{pair.first.second->transition[c], pair.first.first->transition[c]}] = true;
                        startOver = true;
                        break;
                    }
                }
                // same for reversed order
                else if (checkAccesible.count({pair.first.first->transition[c], pair.first.second->transition[c]})) {

                    // check if the pair is marked
                    if (checkAccesible[{pair.first.first->transition[c], pair.first.second->transition[c]}]) {

                        //mark the pair
                        checkAccesible[{pair.first.first->transition[c], pair.first.second->transition[c]}] = true;
                        startOver = true;
                        break;
                    }
                }
            }
            pos++;
            if (startOver) break;
        }
        if (pos == checkAccesible.size()) break;
    }

    std::vector<std::vector<State*>> eqClasses;
    // rekening houden met memloss (oude states deleten)
    // replace each state with it's equivalence class
    for (auto &state:states) {

        // first we check if the state isnt already in an equivalence class
        if (checkInEqClass(eqClasses, state)) continue;

        // make a new equivalence class with the state in
        std::vector<State*> eqClass = {state};

        // check if any other states belong to the equivalence class
        for (const auto &pair:checkAccesible) {

            if (pair.second) continue;
            if (pair.first.first == state) {
                eqClass.emplace_back(pair.first.second);
            }

            else if (pair.first.second == state) {
                eqClass.emplace_back(pair.first.first);
            }
        }
        eqClasses.emplace_back(eqClass);
    }

    // create new states and delete old ones (wordt nog niet gedaan)
    states.clear();
    endStates.clear();
    for (const auto &eqClass:eqClasses) {
        std::string newName;
        // create new name
        for (auto &state:eqClass) {
            newName += state->name;
        }
        State* newState = new State(newName);
        // check if it is a final state or start state
        for (auto &state:eqClass) {
            if (state->final) {
                newState->final = true;
                endStates.emplace_back(newState);
            }
            if (startState == state) startState = newState;
        }
        states.emplace_back(newState);


    }
    if (states.size() != eqClasses.size()) std::cerr << "something went wrong while converting the equivalence classes to states\n";
    // create new transition functions
    for (int n=0;n<states.size();n++) {
        for (char c:alphabet) {
            std::string trans;
            trans = eqClasses[n][0]->transition[c]->name;

            for (auto state:states) {
                if (state->name.find(trans) != std::string::npos) {
                    states[n]->transition[c] = state;
                }
            }
        }
    }



}
DFA::DFA(std::string filename) {
    std::ifstream configDoc(filename, std::ifstream::binary);
    Json::Value root;
    Json::Reader reader;
    reader.parse(configDoc, root, false);
    std::string type = root["type"].asString();
    if (type != "DFA") {
        std::cerr <<  "failed to construct dfa: type is not DFA" << std::endl;
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
            startState = temp;
            startStateCheck = true;
        }
        if (statesJson[index]["accepting"].asBool()) {
            endStates.push_back(temp);
            temp->final = true;
        }
        states.push_back(temp);
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
                        state->transition[transitionsJson[index]["input"].asString()[0]] = s;
                    }
                }

            }
        }
    }
    properlyInitialized = true;
}
