#include <iostream>
#include "coordinate-tools.h"

using namespace coordinate_tools;

int main(int argc, char *argv[]) {

    Parser parser;
    parser.reset();
    bool anySucceed = false;

    if (parser.testDMS(std::string(argv[1]))) {
        std::cout << "\nResult:\n\n";

        for (auto &pt : parser.getAllDMSPoints()) {
            anySucceed = true;
            std::cout << pt.stringifyPair() << std::endl;
        }
    }
    else if (parser.testDecimal(std::string(argv[1]))) {
        std::cout << "\nResult:\n\n";

        for (auto &pt : parser.getAllDecimalPoints()) {
            anySucceed = true;
            std::cout << pt.lat << ',' << pt.lng << std::endl;
        }
    }
    
    if (anySucceed) std::cout << "\nProcess end.\n";
    else std::cout << "\nProcess end. No coordinate.\n";

    return 0;
}

