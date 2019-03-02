#include <iostream>
#include <vector>
#include <fstream>
#include "NFA.h"
#include "json.h"

NFA parse(std::string filename) {
    std::ifstream configDoc(filename, std::ifstream::binary);
    Json::Value root;
    configDoc >> root;

    std::string type = root.get("type", "DFA").asString();

}


int main() {

}