#include <iostream>
#include <vector>
#include <fstream>
#include "./src/NFA.h"
#include "./src/DFA.h"



int main() {
    // using TF-algorithm for dfa minimazation
    DFA k("../input/tfa-min/input.dfa.json");
    k.convertToDot("dfa_not_minimized.dot");
    k.minimizeDfa();
    k.convertToDot("dfa_minimized.dot");
    k.convertToJson("dfa_k.json");
    system("dot -Tpng ../output/dfa_minimized.dot -o ../output/dfa_minimized.png");
    system("dot -Tpng ../output/dfa_not_minimized.dot -o ../output/dfa_not_minimized.png");

    // using TF-algorithm for checking for dfa equivalence
    DFA n("../input/tfa-eq/input1.dfa.json"); n.convertToDot("dfa_n.dot");
    DFA m("../input/tfa-eq/input2.dfa.json"); m.convertToDot("dfa_m.dot");
    if (n.isEquivalentTo(m, false)) {
        std::cout << "n and m are equivalent\n";
    }
    else std::cout << "n and m are not equivalent\n";

    // subset construction
    NFA a("../input/ssc/input.nfa.json");
    a.convertToDot("nfa_a.dot");
    DFA* converted = a.convertToDfa("../output/dfa_converted.dot");
    converted->convertToDot("dfa_converted.dot");
    system("dot -Tpng ../output/nfa_a.dot -o ../output/nfa_a.png");
    system("dot -Tpng ../output/dfa_converted.dot -o ../output/dfa_converted.png");
}

