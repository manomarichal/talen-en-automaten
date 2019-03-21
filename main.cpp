#include <iostream>
#include <vector>
#include <fstream>
#include "./src/NFA.h"
#include "./src/DFA.h"



int main() {
    DFA k("../input/tfa-min/input.dfa.json");
    k.printDFA("../output/dfa_not_minimized.dot");
    k.minimizeDfa();
    k.printDFA("../output/dfa_minimized.dot");
    system("dot -Tpng ../output/dfa_minimized.dot -o ../output/dfa_minimized.png");
    system("dot -Tpng ../output/dfa_not_minimized.dot -o ../output/dfa_not_minimized.png");
}