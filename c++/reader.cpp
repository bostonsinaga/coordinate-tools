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

    bool anySeparator = false,
         anySucceed = false;

    auto isStringsContain = [&]()->bool {
      return latStr.length() > 0 && lngStr.length() > 0;
    };

    auto switchAxis = [&](bool &pairNeedTest) {
      if (chLast == numLast) {
        if (!anySeparator) anySeparator = true;
        else if (anySeparator) {
          if (isStringsContain()) pairNeedTest = true;
          axisPart = !axisPart;
          anySeparator = false;
        }
      }
    };

    auto addToString = [&](bool keepAdd, char ch, bool reset = false) {
      if (keepAdd) {
        if (axisPart == latPart) latStr = reset ? "" : latStr + ch;
        else if (axisPart == lngPart) lngStr = reset ? "" : lngStr + ch;
      }
    };

    for (int i = 0; i < text.length(); i++) {

      bool keepAdd = false,
           isCurNum = false,
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
      // dot or separator
      else if (text[i] == '.') {
        if (!anySeparator) {
          keepAdd = true;
          anySeparator = true;
        }
        else switchAxis(pairNeedTest);
      }
      // primary separator
      else if (text[i] == ',') {
        if (!anySeparator) addToString(true, '.');
        switchAxis(pairNeedTest);
      }
      // secondary separator
      else if (text[i] == ' ') {
        if (anySeparator) switchAxis(pairNeedTest);
        else if (!anySeparator && chLast == numLast) {
          addToString(true, '\0', true);
        }
      }
      // not numeric or a wild dash character
      else {
        chLast = othLast;
        pairNeedTest = true;
      }

      if (i == text.length() - 1) pairNeedTest = true;
      if (!isCurNum && chLast != othLast) chLast = incLast;

      addToString(keepAdd, text[i]);

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