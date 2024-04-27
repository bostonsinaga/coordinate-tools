#ifndef __COORDINATE_TOOLS__PARSER_H__
#define __COORDINATE_TOOLS__PARSER_H__

#include <ctype.h>
#include <vector>
#include <string>
#include "point.h"

namespace coordinate_tools {
  class AxisString {
  public:
    std::string latStr, lngStr;

    enum {latPart, lngPart};
    bool axisPart = latPart;

    enum {incLast, numLast, othLast};
    int chLast = incLast;

    bool anySeparator = false,
         anySucceed = false;

    bool isStringsContain() {
      return latStr.length() > 0 && lngStr.length() > 0;
    }

    void switchAxis(bool &pairNeedTest) {
      if (chLast == numLast) {
        if (!anySeparator) anySeparator = true;
        else if (anySeparator) {
          if (isStringsContain()) pairNeedTest = true;
          axisPart = !axisPart;
          anySeparator = false;
        }
      }
    }

    void addToString(bool keepAdd, char ch, bool reset = false) {
      if (keepAdd) {
        if (axisPart == latPart) latStr = reset ? "" : latStr + ch;
        else if (axisPart == lngPart) lngStr = reset ? "" : lngStr + ch;
      }
    }
  };

  class Parser {
  private:
    std::vector<Point> points;

  public:
    // ex: [ -7.123, 110.123 ]
    bool testDecimal(std::string &text, bool reset = false);

    bool testDecimal(const std::string &text, bool reset = false) {
      testDecimal(text);
    }

    // ex: [ 7°7'22.80"S, 110°7'22.80"E ]
    bool testDMS(std::string &text, bool reset = false);

    bool testDMS(const std::string &text, bool reset = false) {
      testDMS(text);
    }

    // returns the first vector
    Point getSinglePoint() {
      if (points.size() > 0) {
        return points.front();
      }
      else return Point();
    }

    std::vector<Point> getAllPoints() { return points; }
    void reset() { points = {}; }
  };
}

#endif // __COORDINATE_TOOLS__PARSER_H__