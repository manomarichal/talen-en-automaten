#include <iostream>
#include <vector>
#include <fstream>
#include "./src/NFA.h"
#include "./src/DFA.h"



int main() {
    DFA k("../input/test1.json");
    std::cout << k.inputString("100001000010");
    k.printNFA("nfa.dot");
    system("dot -Tpng ../output/nfa.dot -o ../output/nfa.png");
}