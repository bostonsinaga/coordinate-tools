#ifndef __COORDINATE_TOOLS__CONVERTER_H__
#define __COORDINATE_TOOLS__CONVERTER_H__

#include <vector>
#include "point.h"

namespace coordinate_tools {
  class Converter {
  private:
    enum {qr_signs, qr_adjustments};

    static void setMaxAbsoluteDegree(
      int &maxAbsDegree,
      int &maxDegreeFlag
    );

    static void setQuadrantDeterminators(
      std::vector<int> &qrSigns,
      std::vector<int> &qrAdjustments,
      int valSign, int maxDegreeFlag, int maxAbsDegree
    );

  public:
    enum {MAX_DEG_90, MAX_DEG_180};

    static void normalizeDecimalAngle(double &axis, int maxDegreeFlag = MAX_DEG_90);
    static void normalizeDMSAngle(DMSAxis &axis, int maxDegreeFlag = MAX_DEG_90);

    static void switchDecimalAxis(DecimalPoint &pt);
    static void switchDMSAxis(DMSPoint &pt);
  };
}

#endif // __COORDINATE_TOOLS__CONVERTER_H__