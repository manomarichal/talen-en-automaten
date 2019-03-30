#include <iostream>
#include <vector>
#include <fstream>
#include "./NFA.h"
#include "./DFA.h"
#include "./ENFA.h"



int main(int argc, char* argv[]) {

    std::string alg = argv[1];

    if (alg == "tfa") {
        std::string input = argv[2];
        std::string dot = argv[3];
        std::string output = argv[4];
        DFA k(input);
        k.minimizeDfa(output);
        k.convertToDot(dot);
        //k.convertToJson("output");
        std::string dotstring = "dot -Tpng ../output/" + dot + " -o ../output/" + dot;
        //system(dotstring);
    }

    else if (alg == "tfaeq") {
        std::string input = argv[2];
        std::string input2 = argv[3];
        std::string output = argv[4];
        DFA n(input);
        DFA m(input2);
        n.isEquivalentTo(m, output);
    }

    else if (alg == "ssc") {
        std::string input = argv[2];
        std::string dot = argv[3];
        NFA a(input);
        DFA *converted = a.convertToDfa();
        converted->convertToDot(dot);
        //converted->convertToJson("dfa_converted.json");
        std::string dotstring = "dot -Tpng ../output/" + dot + " -o ../output/" + dot;
        //system(dotstring);
    }

    else if (alg == "mssc") {
        std::string input = argv[2];
        std::string dot = argv[3];
        ENFA a(input);
        DFA *converted = a.convertToDfa();
        converted->convertToDot(dot);
        //converted->convertToJson("dfa_converted.json");
        std::string dotstring = "dot -Tpng ../output/" + dot + " -o ../output/" + dot;
        //system(dotstring);
    }
}

