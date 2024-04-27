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

    void addToString(bool keepAdd, char ch, bool needReset = false) {
      if (keepAdd) {
        if (axisPart == latPart) latStr = needReset ? "" : latStr + ch;
        else if (axisPart == lngPart) lngStr = needReset ? "" : lngStr + ch;
      }
    }
  };

  class Parser {
  private:
    std::vector<Point> points;

  public:
    // ex: [ -7.123, 110.123 ]
    bool testDecimal(std::string text, bool needReset = false);

    // ex: [ 7°7'22.80"S, 110°7'22.80"E ]
    bool testDMS(std::string text, bool needReset = false);

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