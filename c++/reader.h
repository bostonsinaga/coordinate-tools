#ifndef __COORDINATE_TOOLS__READER_H__
#define __COORDINATE_TOOLS__READER_H__

#include <ctype.h>
#include <vector>
#include <string>
#include "point.h"

namespace coordinate_tools {
  class Reader {
  private:
    std::vector<Point> points;

  public:
    Reader() {}

    // ex: [ -7.123, 110.123 ]
    bool testDecimal(std::string text);

    // ex: [ 7°7'22.80"S, 110°7'22.80"E ]
    bool testDMS() {
      return true;
    }

    // returns the first vector
    Point getSinglePoint() {
      if (points.size() > 0) {
        return points.front();
      }
      else return Point();
    }

    std::vector<Point> getAllPoints() { return points; }
  };
}

#endif // __COORDINATE_TOOLS__READER_H__