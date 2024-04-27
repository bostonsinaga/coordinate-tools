#include <iostream>
#include "coordinate-tools.h"

using namespace coordinate_tools;

int main(int argc, char *argv[]) {    

    Parser parser;
    parser.reset();

    if (parser.testDecimal(argv[1])) {
        for (auto &pt : parser.getAllPoints()) {
            std::cout << "Lat: " << pt.lat << ", ";
            std::cout << "Lng: " << pt.lng << std::endl;
        }
    }

    return 0;
}

