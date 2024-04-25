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

    enum {incLast, numLast, othLast};
    int chLast = incLast;

    enum {prevDot_nothing, prevDot_detected, prevDot_process};
    int prevDotSprt = prevDot_nothing;

    bool keepAdd = false,
         anySucceed = false;

    auto isStringsContain = [&]()->bool {
      return latStr.length() > 0 && lngStr.length() > 0;
    };

    char ch;
    bool chNeedUpdate = true;

    for (int i = 0; i < text.length(); i++) {

      bool isCurNum = false,
           pairNeedTest = false,
           branchStop = false;

      if (chNeedUpdate) {
        chNeedUpdate = false;
        ch = text[i];
      }

      // negative sign
      if (ch == '-' &&
        ((axisPart == latPart && latStr.length() == 0) ||
        (axisPart == lngPart && lngStr.length() == 0))
      ) {
        keepAdd = true;
        branchStop = true;
      }
      // number
      else if (std::isdigit(ch)) {
        keepAdd = true;
        chLast = numLast;
        isCurNum = true;
        branchStop = true;
      }
      // dot
      else if (ch == '.') {
        if (dotCtr == 0) {
          dotCtr++;
          keepAdd = true;
          branchStop = true;
        }
        else {
          ch = ',';
          prevDotSprt = prevDot_detected;
        }
      }

      if (!branchStop) {
        // separator
        if (ch == ',' || ch == ' ') {
          dotCtr = 0;
          keepAdd = false;

          if (prevDotSprt == prevDot_process) {
            i--;
            ch = '.';
            prevDotSprt = prevDot_nothing;
            continue;
          }
          else if (prevDotSprt == prevDot_detected) {
            prevDotSprt = prevDot_process;
          }

          if (chLast == numLast) {
            if (isStringsContain()) pairNeedTest = true;
            axisPart = !axisPart;
          }
        }
        // not numeric character
        else {
          chLast = othLast;
          pairNeedTest = true;
        }
      }

      chNeedUpdate = true;

      if (i == text.length() - 1) pairNeedTest = true;
      if (!isCurNum && chLast != othLast) chLast = incLast;

      if (keepAdd) {
        if (axisPart == latPart) latStr += ch;
        else if (axisPart == lngPart) lngStr += ch;
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