#ifndef __COORDINATE_TOOLS__PARSER_CPP__
#define __COORDINATE_TOOLS__PARSER_CPP__

#include "parser.h"

namespace coordinate_tools {
  bool Parser::testDecimal(std::string text, bool needReset) {

    if (needReset) reset();
    AxisString axStr;

    for (int i = 0; i < text.length(); i++) {

      bool keepAdd = false,
           isCurNum = false,
           pairNeedTest = false;

      // negative sign
      if (text[i] == '-' &&
        ((axStr.axisPart == axStr.latPart && axStr.latStr.length() == 0) ||
        (axStr.axisPart == axStr.lngPart && axStr.lngStr.length() == 0))
      ) {
        keepAdd = true;
      }
      // number
      else if (std::isdigit(text[i])) {
        keepAdd = true;
        axStr.chLast = axStr.numLast;
        isCurNum = true;
      }
      // dot or separator
      else if (text[i] == '.') {
        if (!axStr.anySeparator) {
          keepAdd = true;
          axStr.anySeparator = true;
        }
        else axStr.switchAxis(pairNeedTest);
      }
      // primary separator
      else if (text[i] == ',') {
        if (!axStr.anySeparator) axStr.addToString(true, '.');
        axStr.switchAxis(pairNeedTest);
      }
      // secondary separator
      else if (text[i] == ' ') {
        if (axStr.anySeparator) axStr.switchAxis(pairNeedTest);
        else if (!axStr.anySeparator && axStr.chLast == axStr.numLast) {
          axStr.addToString(true, '\0', true);
        }
      }
      // not numeric or a wild dash character
      else {
        axStr.chLast = axStr.othLast;
        pairNeedTest = true;
      }

      if (i == text.length() - 1) pairNeedTest = true;

      if (!isCurNum && axStr.chLast != axStr.othLast) {
        axStr.chLast = axStr.incLast;
      }

      axStr.addToString(keepAdd, text[i]);

      if (pairNeedTest) {
        double lat = 0, lng = 0;

        try { lat = std::stod(axStr.latStr); }
        catch(...) {}

        try { lng = std::stod(axStr.lngStr); }
        catch(...) {}

        bool isStringsContainVar = axStr.isStringsContain();

        if (isStringsContainVar || axStr.chLast == axStr.othLast) {
          axStr.axisPart = axStr.latPart;
          axStr.latStr = "";
          axStr.lngStr = "";

          if (isStringsContainVar) {
            points.push_back(Point(lat, lng));
            axStr.anySucceed = true;
          }
        }
      }
    }

    return axStr.anySucceed;
  }

  bool Parser::testDMS(std::string text, bool needReset) {
    return true;
  }
}

#endif // __COORDINATE_TOOLS__PARSER_CPP__