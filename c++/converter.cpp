#ifndef __COORDINATE_TOOLS__CONVERTER_CPP__
#define __COORDINATE_TOOLS__CONVERTER_CPP__

#include "converter.h"

namespace coordinate_tools {

  // ex: [ -7.123, 185.123 ] to [ -7.123, xxx.yyy ]
  void Converter::normalizeDecimalAngle(double &axis, int maxDegreeFlag) {

    int initIntAxis = int(axis),
        intAxis = std::abs(initIntAxis),
        maxAbsDegree;

    // set 'maxDegreeFlag' default to 'MAX_DEG_90'
    if (maxDegreeFlag != MAX_DEG_90 && maxDegreeFlag != MAX_DEG_180) {
      maxDegreeFlag = MAX_DEG_90;
    }

    // determine the degree limit
    if (maxDegreeFlag == MAX_DEG_90) maxAbsDegree = 90;
    else maxAbsDegree = 180;

    // less degree don't need to process
    if (intAxis <= maxAbsDegree) return;

    // default positive ('qr' for 'quadrant')
    std::vector<int> qrSigns, qrAdjustments;

    if (maxDegreeFlag == MAX_DEG_90) {
      qrSigns = {-1, -1, 1, 1};
      qrAdjustments = {maxAbsDegree, 0, -maxAbsDegree, 0};
    }
    else { // 180
      qrSigns = {1, 1};
      qrAdjustments = {-maxAbsDegree, 0};
    }

    double fracAxis = std::abs(axis) - intAxis;
    int qrCount = qrSigns.size();

    // inverse for negative
    if (initIntAxis / intAxis == -1) {
      for (int i = 0; i < qrCount; i++) {
        qrSigns[i] *= -1;
        qrAdjustments[i] *= -1;
      }
    }

    // process with remainder
    int qrIndex = (intAxis / maxAbsDegree - 1) % qrCount;
    intAxis %= maxAbsDegree;
    double realAxis = qrAdjustments[qrIndex] + qrSigns[qrIndex] * (intAxis + fracAxis);
    axis = realAxis;
  }

  void Converter::switchDecimalAxis(DecimalPoint &pt) {
    double buffer = pt.lat;
    pt.lat = pt.lng;
    pt.lng = buffer;
  }

  void Converter::switchDMSAxis(DMSPoint &pt) {
    DMSAxis buffer = pt.lat;
    pt.lat = pt.lng;
    pt.lng = buffer;
  }
}

#endif // __COORDINATE_TOOLS__CONVERTER_CPP__