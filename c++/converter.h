#ifndef __COORDINATE_TOOLS__CONVERTER_H__
#define __COORDINATE_TOOLS__CONVERTER_H__

#include <vector>
#include "point.h"

namespace coordinate_tools {
  class Converter {
  public:
    enum {MAX_DEG_90, MAX_DEG_180};
    static void normalizeDecimalAngle(double &axis, int maxDegreeFlag = MAX_DEG_90);
    static void switchDecimalAxis(DecimalPoint &pt);
    static void switchDMSAxis(DMSPoint &pt);
  };
}

#endif // __COORDINATE_TOOLS__CONVERTER_H__