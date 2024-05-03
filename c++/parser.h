#ifndef __COORDINATE_TOOLS__PARSER_H__
#define __COORDINATE_TOOLS__PARSER_H__

#include <ctype.h>
#include <vector>
#include <string>
#include "point.h"

namespace coordinate_tools {
  class Parser {
  private:
    std::vector<DecimalPoint> decPoints;
    std::vector<DMSPoint> dmsPoints;

  public:
    // ex: [ -7.123, 110.123 ]
    bool testDecimal(std::string text, bool needReset = false);

    // ex: [ 7°7'22.80"S, 110°7'22.80"E ]
    bool testDMS(std::string text, bool needReset = false);

    // returns the first vector
    DecimalPoint getSingleDecimalPoint() {
      if (decPoints.size() > 0) return decPoints.front();
      return DecimalPoint();
    }

    // returns the first vector
    DMSPoint getSingleDMSPoint() {
      if (dmsPoints.size() > 0) return dmsPoints.front();
      return DMSPoint();
    }

    std::vector<DecimalPoint> getAllDecimalPoints() { return decPoints; }
    std::vector<DMSPoint> getAllDMSPoints() { return dmsPoints; }

    void reset(int pointType = ALL_POINT_TYPE) {
      if (pointType == DECIMAL_POINT_TYPE) decPoints = {};
      else if (pointType == DMS_POINT_TYPE) dmsPoints = {};
      else if (pointType == ALL_POINT_TYPE) {
        decPoints = {};
        dmsPoints = {};
      }
    }
  };
}

#endif // __COORDINATE_TOOLS__PARSER_H__