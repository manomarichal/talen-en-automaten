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

void DFA::convertToDot(std::string filename) {
    if (!properlyInitialized) return;
    std::ofstream outputFile("../output/" + filename);
    std::stringstream s;
    // states
    s << "nodesep=1;\n";
    s << "overlap=false;\n";



    for (auto state:endStates) {
        s << "  " << "\"" + state->name + "\""  << "[peripheries=2]" << std::endl;
    }
    // edges
    for (auto state:states) {
        for (auto symbol:state->transition) {
            s << "  " << "\"" + state->name + "\""  << "->" << "\"" + symbol.second->name + "\"" <<  "[label=\"" << symbol.first << "\"];" << std::endl;
        }
    }

    // start state
    s << "  " << "head [style=invis]\n  head->" << "\"" + startState->name + "\"" << std::endl;
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

    // create html table
    std::ofstream outputFile("../output/table-min.html");
    std::stringstream output;

    output <<
           "<html>\n"
           "<head>\n"
           "<style>\n"
           "table, th, td {\n"
           "       border: 1px solid black;\n"
           "       font-family:\"Courier New\", Courier, monospace;\n"
           "       font-size:150%\n"
           "}\n"
           "table {\n"
           "       border-collapse: collapse;\n"
           "}\n"
           "td,th {\n"
           "       height: 50px;\n"
           "       width: 50;\n"
           "       text-align: center;\n"
           "}\n"
           "tr:nth-child(even) {background-color: #f2f2f2;}\n"
           "th {\n"
           "       background-color: #4CAF50;\n"
           "       color: white;\n"
           "}\n"
           "</style>\n"
           "</head>\n"
           "<body>\n"
           "<table>\n";

    for (int column=1;column<states.size();column++) {
        output << "  <tr>\n"
                  "     <th>" + states[column]->name + "</th>\n";

        for (auto eqClass:eqClasses) {
            if (std::find(eqClass.begin(), eqClass.end(), states[column]) != eqClass.end()) {

                // we found the equivalence class
                for (int row=0; row<column;row++) {

                    // look for the state in the equivalence class
                    bool found = false;

                    for (auto state:eqClass) {
                        if (state->name == states[row]->name) found = true;
                    }

                    if (!found) {
                        output << "     <td>X</td>\n";
                    }
                    else output << "     <td></td>\n";
                }
                output << "  </tr>\n";
            }
        }
    }
    output << "  <tr>\n";
    output << "     <th></th>\n";
    for (int column=0;column<states.size()-1;column++) {
        output << "     <th>" + states[column]->name + "</th>\n";
    }
    output << "  </tr>\n";


    output << "</body>\n" << "</html>";
    outputFile << output.str();
    outputFile.close();

    // create new states and delete old ones (wordt nog niet gedaan)
    states.clear();
    endStates.clear();
    for (const auto &eqClass:eqClasses) {
        std::string newName = "{";
        // create new name
        for (auto &state:eqClass) {
            newName += state->name;
            if (state->name != eqClass.back()->name) newName += ",";
        }
        newName += "}";
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

void DFA::convertToJson(std::string filename) {
    if (!properlyInitialized) return;
    std::ofstream outputFile("../output/" + filename);
    Json::Value output;

    output["type"] = "DFA";

    // alphabet
    Json::Value tempAlphabet(Json::arrayValue);
    for (char c:alphabet) {
        tempAlphabet.append(Json::Value(std::string(1,c)));
    }
    output["alphabet"] = tempAlphabet;

    // states
    Json::Value tempStates(Json::arrayValue);
    for (auto &state:states) {
        Json::Value tempState;
        tempState["name"] = state->name;
        if (state == startState) tempState["starting"] = true;
        else tempState["starting"] = false;
        tempState["accepting"] = state->final;
        tempStates.append(tempState);
    }
    output["states"] = tempStates;


    // transitions
    Json::Value tempTransitions(Json::arrayValue);
    for (auto &state:states) {
        for (auto &transition:state->transition) {
            Json::Value tempTransition;
            tempTransition["from"] = state->name;
            tempTransition["to"] = transition.second->name;
            tempTransition["input"] = std::string(1, transition.first);
            tempTransitions.append(tempTransition);
        }
    }
    output["transitions"] = tempTransitions;
    outputFile << output;
    outputFile.close();
}

bool DFA::isEquivalentTo(const DFA &dfa) {

    std::vector<State*> unie;
    for (State* state:states) unie.emplace_back(state);
    for (State* state:dfa.getStates()) unie.emplace_back(state);

    // deze map stelt de tabel voor met de paren, de bool geeft aan of het wel of niet onderscheidbaar is
    std::map<std::pair<State*, State*>, bool> checkAccesible;
    // add pairs to table
    for (int x=0; x < unie.size(); x++) {
        for (int y=x+1; y < unie.size(); y++) {
            checkAccesible[{unie[x], unie[y]}] = false;
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
    for (auto &state:unie) {

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

    // create html table
    bool result = true;
    if (checkAccesible.count({startState, dfa.getStartState()})) {
        if (checkAccesible[{startState, dfa.getStartState()}]) result = false;
    }
    else if (checkAccesible.count({dfa.getStartState(), startState}))  {
        if (checkAccesible[{dfa.getStartState(), startState}]) result = false;
    }
    else {
        std::cerr << "couldnt find a pair with both startstates";
        return false;
    }

    std::ofstream outputFile("../output/table-eq.html");
    std::stringstream output;

    output <<
           "<html>\n"
           "<head>\n"
           "<style>\n"
           "table, th, td {\n"
           "       border: 1px solid black;\n"
           "       font-family:\"Courier New\", Courier, monospace;\n"
           "       font-size:150%\n"
           "}\n"
           "table {\n"
           "       border-collapse: collapse;\n"
           "}\n"
           "td,th {\n"
           "       height: 50px;\n"
           "       width: 50;\n"
           "       text-align: center;\n"
           "}\n"
           "tr:nth-child(even) {background-color: #f2f2f2;}\n"
           "th {\n"
           "       background-color: #4CAF50;\n"
           "       color: white;\n"
           "}\n"
           "</style>\n"
           "</head>\n"
           "<body>\n";
    if (result) output << "     Equivalent = true";
    else output << "     Equivalent = false";

    output << "<table>\n";
    for (int column=1;column<unie.size();column++) {
        output << "  <tr>\n"
                  "     <th>" + unie[column]->name + "</th>\n";

        for (auto eqClass:eqClasses) {
            if (std::find(eqClass.begin(), eqClass.end(), unie[column]) != eqClass.end()) {

                // we found the equivalence class
                for (int row=0; row<column;row++) {

                    // look for the state in the equivalence class
                    bool found = false;

                    for (auto state:eqClass) {
                        if (state->name == unie[row]->name) found = true;
                    }
                    if (!found) {
                        output << "     <td>X</td>\n";
                    }
                    else output << "     <td></td>\n";
                }
                output << "  </tr>\n";
            }
        }
    }
    output << "  <tr>\n";
    output << "     <th></th>\n";
    for (int column=0;column<unie.size()-1;column++) {
        output << "     <th>" + unie[column]->name + "</th>\n";
    }
    output << "  </tr>\n";


    output << "</body>\n" << "</html>";
    outputFile << output.str();
    outputFile.close();

    return result;
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

const std::vector<DFA::State *> &DFA::getStates() const {
    return states;
}

DFA::State *DFA::getStartState() const {
    return startState;
}

const std::vector<DFA::State *> &DFA::getEndStates() const {
    return endStates;
}

bool DFA::isProperlyInitialized() const {
    return properlyInitialized;
}

void DFA::setProperlyInitialized(bool properlyInitialized) {
    DFA::properlyInitialized = properlyInitialized;
}

const std::vector<char> &DFA::getAlphabet() const {
    return alphabet;
}

void DFA::setAlphabet(const std::vector<char> &alphabet) {
    DFA::alphabet = alphabet;
}

void DFA::setStates(const std::vector<DFA::State *> &states) {
    DFA::states = states;
}

void DFA::setStartState(DFA::State *startState) {
    DFA::startState = startState;
}

void DFA::setEndStates(const std::vector<DFA::State *> &endStates) {
    DFA::endStates = endStates;
}

void DFA::addState(DFA::State *state) {
    states.emplace_back(state);
}

void DFA::addEndState(DFA::State *state) {
    endStates.emplace_back(state);
}