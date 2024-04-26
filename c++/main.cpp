#include <iostream>
#include "coordinate-tools.h"

using namespace coordinate_tools;

int main(int argc, char *argv[]) {    
    Parser reader = Parser();

    if (reader.testDecimal(argv[1])) {
        for (auto &pt : reader.getAllPoints()) {
            std::cout << "Lat: " << pt.lat << ", ";
            std::cout << "Lng: " << pt.lng << std::endl;
        }
    }

    return 0;
}

