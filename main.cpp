#include <iostream>
#include <vector>
#include <fstream>
#include "NFA.h"
#include "json.h"



int main() {
    NFA k("../input/test1.json");
    std::cout << k.inputString("100001000010");
    k.printNFA("nfa.dot");
    system("dot -Tpng ../output/nfa.dot -o ../output/nfa.png");
}