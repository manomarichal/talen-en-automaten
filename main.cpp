#include <iostream>
#include <vector>
#include <fstream>
#include "NFA.h"
#include "json.h"



int main() {
    NFA k("../test1.json");
    std::cout << k.inputString("100001000010");
    k.printNFA("../nfa.dot");
    system("dot -Tpng ../nfa.dot -o ../nfa.png");
}