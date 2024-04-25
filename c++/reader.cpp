#ifndef __COORDINATE_TOOLS__READER_CPP__
#define __COORDINATE_TOOLS__READER_CPP__

#include "reader.h"

namespace coordinate_tools {
  bool Reader::testDecimal(std::string text) {

    points = {};
    std::string latStr, lngStr;
    int dotCtr = 0;

    enum {latPart, lngPart};
    bool axisPart = latPart;

    enum {othLast, numLast, alpLast};
    int chLast = othLast;

    bool keepAdd = false,
         anySucceed = false;

    auto isStringsContain = [&]()->bool {
      return latStr.length() > 0 && lngStr.length() > 0;
    };

    for (int i = 0; i < text.length(); i++) {

      bool isCurNum = false,
           pairNeedTest = false;

      // negative sign
      if (text[i] == '-' &&
        ((axisPart == latPart && latStr.length() == 0) ||
        (axisPart == lngPart && lngStr.length() == 0))
      ) {
        keepAdd = true;
      }
      // number
      else if (std::isdigit(text[i])) {
        keepAdd = true;
        chLast = numLast;
        isCurNum = true;
      }
      // dot
      else if (text[i] == '.' && dotCtr == 0) {
        dotCtr++;
        keepAdd = true;
      }
      // separator
      else if (text[i] == ',' || text[i] == ' ') {
        dotCtr = 0;
        keepAdd = false;

        if (chLast == numLast) {
          if (isStringsContain()) pairNeedTest = true;
          axisPart = !axisPart;
        }
      }
      else if (std::isalpha(text[i])) {
        chLast = alpLast;
        pairNeedTest = true;
      }

      if (i == text.length() - 1) pairNeedTest = true;
      if (!isCurNum && chLast != alpLast) chLast = othLast;

      if (keepAdd) {
        if (axisPart == latPart) latStr += text[i];
        else if (axisPart == lngPart) lngStr += text[i];
      }

      if (pairNeedTest) {
        double lat = 0, lng = 0;

        try { lat = std::stod(latStr); }
        catch(...) {}

        try { lng = std::stod(lngStr); }
        catch(...) {}

        bool isStringsContainVar = isStringsContain();

        if (isStringsContainVar || chLast == alpLast) {
          axisPart = latPart;
          latStr = "";
          lngStr = "";

          if (isStringsContainVar) {
            points.push_back(Point(lat, lng));
            anySucceed = true;
          }
        }
      }
    }

    return anySucceed;
  }
}

#endif // __COORDINATE_TOOLS__READER_CPP__