#include <iostream>
#include <vector>
#include <fstream>
#include "./src/NFA.h"
#include "./src/DFA.h"



int main() {
    // using TF-algorithm for dfa minimazation
    DFA k("../input/tfa-min/input.dfa.json");
    k.minimizeDfa();
    k.convertToDot("dfa_minimized.dot");
    k.convertToJson("dfa_minimized.json");
    system("dot -Tpng ../output/dfa_minimized.dot -o ../output/dfa_minimized.png");

    // using TF-algorithm for checking for dfa equivalence
    DFA n("../input/tfa-eq/input1.dfa.json");
    DFA m("../input/tfa-eq/input2.dfa.json");
    n.isEquivalentTo(m);

    // subset construction
    NFA a("../input/ssc/input.nfa.json");
    DFA* converted = a.convertToDfa("../output/dfa_converted.dot");
    converted->convertToDot("dfa_converted.dot");
    converted->convertToJson("dfa_converted.json");
    system("dot -Tpng ../output/dfa_converted.dot -o ../output/dfa_converted.png");

}

