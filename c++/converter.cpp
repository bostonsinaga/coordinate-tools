#ifndef __COORDINATE_TOOLS__CONVERTER_CPP__
#define __COORDINATE_TOOLS__CONVERTER_CPP__

#include <iostream>
#include "converter.h"

namespace coordinate_tools {

  DecimalPoint Converter::dmsToDd(DMSPoint &before) {

    double after_d[2];
    DMSAxis before_d[2] = {before.lat, before.lng};

    for (int i = 0; i < 2; i++) {

      int curDeg = before_d[i].getDeg(),
          absCurDeg = std::abs(curDeg);

      after_d[i] = curDeg / absCurDeg * (
        absCurDeg +
        double(before_d[i].getMin()) / 60 +
        before_d[i].getSec() / 3600
      );
    }

    return DecimalPoint(after_d[0], after_d[1]);
  }

  DMSPoint Converter::ddToDms(DecimalPoint &before) {

    DMSAxis after_d[2];
    double before_d[2] = {before.lat, before.lng};

    for (int i = 0; i < 2; i++) {

      int deg_i = int(before_d[i]);
      double min_d = std::abs(before_d[i] - deg_i) * 60;
      int min_i = int(min_d);

      after_d[i].setDeg(deg_i);
      after_d[i].setSec((min_d - min_i) * 60);
      after_d[i].setMin(min_i);
    }

    return DMSPoint(after_d[0], after_d[1]);
  }

  void Converter::setMaxAbsoluteDegree(
    int &maxAbsDegree,
    int &maxDegreeFlag
  ) {
    // set 'maxDegreeFlag' default to 'MAX_DEG_90'
    if (maxDegreeFlag != MAX_DEG_90 && maxDegreeFlag != MAX_DEG_180) {
      maxDegreeFlag = MAX_DEG_90;
    }

    // determine the degree limit
    if (maxDegreeFlag == MAX_DEG_90) maxAbsDegree = 90;
    else maxAbsDegree = 180;
  }

  void Converter::setQuadrantDeterminators(
    std::vector<int> &qrSigns,
    std::vector<int> &qrAdjustments,
    int valSign, int maxDegreeFlag, int maxAbsDegree
  ) {
    // 90°
    if (maxDegreeFlag == MAX_DEG_90) {
      qrSigns = {-1, -1, 1, 1};
      qrAdjustments = {maxAbsDegree, 0, -maxAbsDegree, 0};
    }
    else { // 180°
      qrSigns = {1, 1};
      qrAdjustments = {-maxAbsDegree, 0};
    }

    // inverse for negative
    if (valSign == -1) {
      for (int i = 0; i < qrSigns.size(); i++) {
        qrSigns[i] *= -1;
        qrAdjustments[i] *= -1;
      }
    }
  }

  // ex: [ -7.123, 365.123 ] to [ -7.123, xxx.yyy ]
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

  // ex: [ 7°7'22.80"S, 365'22.80"E ] to [ 7°7'22.80"S, xxx°xx'yy.yy"* ]
  void Converter::normalizeDMSAngle(DMSAxis &axis, int maxDegreeFlag) {

    int newAxis = std::abs(axis.getDeg()),
        maxAbsDegree;

    // normalize 'maxDegreeFlag'
    setMaxAbsoluteDegree(maxAbsDegree, maxDegreeFlag);

    // less degree don't need to process
    if (newAxis <= maxAbsDegree) return;

    // default for positive
    std::vector<int> qrSigns, qrAdjustments;

    // 'qr' for 'quadrant'
    setQuadrantDeterminators(
      qrSigns, qrAdjustments,
      axis.getDeg() / newAxis, maxDegreeFlag, maxAbsDegree
    );

    // process with remainder
    int qrIndex = (newAxis / maxAbsDegree - 1) % qrSigns.size();
    newAxis %= maxAbsDegree;

    // assign to new value
    axis.setDeg(qrAdjustments[qrIndex] + qrSigns[qrIndex] * newAxis);

    // fraction value
    if (axis.getMin() > 0 || axis.getSec() > 0) {
      axis.setDeg(axis.getDeg() - 1);

      // version 2
      double newFrac = 1 - axis.getMin() / 60 - axis.getSec() / 3600;
      double newMin = newFrac * 60;
      axis.setMin(int(newMin));
      axis.setSec((newMin - int(newMin)) * 60);
    }
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