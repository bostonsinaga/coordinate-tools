#ifndef __COORDINATE_TOOLS__READER_CPP__
#define __COORDINATE_TOOLS__READER_CPP__

#include "reader.h"

namespace coordinate_tools {
  bool Reader::testDecimal(std::string text) {

    points = {};
    std::string latStr, lngStr;

    enum {latPart, lngPart};
    bool axisPart = latPart;

    enum {incLast, numLast, othLast};
    int chLast = incLast;

    enum {noNumSet, evenNumSet, unevenNumSet};
    int anyNumber = noNumSet;

    bool keepAdd = false,
         anySucceed = false;

    auto isStringsContain = [&]()->bool {
      return latStr.length() > 0 && lngStr.length() > 0;
    };

    for (int i = 0; i < text.length(); i++) {

      bool isCurNum = false,
           pairNeedTest = false,
           branchStop = false;

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
      else if (text[i] == '.') {
        keepAdd = true;
      }
      // separator
      if (text[i] == ',' || text[i] == ' ') {
        keepAdd = false;

        if (chLast == numLast) {
          if (isStringsContain()) pairNeedTest = true;
          axisPart = !axisPart;
          anyNumber = false;
        }
      }
      // not numeric character
      else {
        chLast = othLast;
        pairNeedTest = true;
      }

      if (i == text.length() - 1) pairNeedTest = true;
      if (!isCurNum && chLast != othLast) chLast = incLast;

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

        if (isStringsContainVar || chLast == othLast) {
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