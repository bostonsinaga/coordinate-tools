#ifndef __COORDINATE_TOOLS__CONVERTER_CPP__
#define __COORDINATE_TOOLS__CONVERTER_CPP__

#include "converter.h"

namespace coordinate_tools {

  // DMS to Decimal Degree
  DecimalPoint Converter::dmsToDd(DMSPoint &before) {

    double after_arr[2];
    DMSAxis before_arr[2] = {before.lat, before.lng};

    for (int i = 0; i < 2; i++) {
      after_arr[i] = before_arr[i].getSign() * (
        before_arr[i].getDeg() +
        double(before_arr[i].getMin()) / 60 +
        before_arr[i].getSec() / 3600
      );
    }

    return DecimalPoint(after_arr[0], after_arr[1]);
  }

  // Decimal Degree to DMS
  DMSPoint Converter::ddToDms(DecimalPoint &before) {

    DMSAxis after_arr[2];
    double before_arr[2] = {before.lat, before.lng};

    for (int i = 0; i < 2; i++) {

      int deg_i = int(before_arr[i]);
      double min_d = std::abs(before_arr[i] - deg_i) * 60;
      int min_i = int(min_d);

      after_arr[i].setDeg(deg_i);
      after_arr[i].setSec((min_d - min_i) * 60);
      after_arr[i].setMin(min_i);
    }

    return DMSPoint(after_arr[0], after_arr[1]);
  }

  // normalize 'maxDegreeFlag'
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

  // return 'qrIndex'
  int Converter::setQuadrantDeterminators(
    std::vector<int> &qrSigns,
    std::vector<int> &qrAdjustments,
    int valSign, int &updateDegree,
    int &maxDegreeFlag, int &maxAbsDegree
  ) {
    // 90°N
    if (maxDegreeFlag == MAX_DEG_90) {
      qrSigns = {-1, -1, 1, 1};
      qrAdjustments = {maxAbsDegree, 0, -maxAbsDegree, 0};
    }
    else { // 180°E
      qrSigns = {1, 1};
      qrAdjustments = {-maxAbsDegree, 0};
    }

    // opposite value
    if (valSign == -1) {
      for (int i = 0; i < qrSigns.size(); i++) {
        qrSigns[i] *= -1;
        qrAdjustments[i] *= -1;
      }
    }

    // process with remainder
    int qrIndex = (updateDegree / maxAbsDegree - 1) % qrSigns.size();
    updateDegree %= maxAbsDegree;
    return qrIndex;
  }

  // ex: [ -7.123, 365.123 ] to [ -7.123, xxx.yyy ]
  void Converter::normalizeDecimalAngle(double &axis, int maxDegreeFlag) {

    int initIntAxis = int(axis),
        initAbsIntAxis = std::abs(initIntAxis),
        newAxis = initAbsIntAxis,
        maxAbsDegree;

    setMaxAbsoluteDegree(maxAbsDegree, maxDegreeFlag);

    // less degree don't need to process
    if (newAxis <= maxAbsDegree) return;

    // default positive
    std::vector<int> qrSigns, qrAdjustments;

    // 'qr' for 'quadrant'
    int qrIndex = setQuadrantDeterminators(
      qrSigns, qrAdjustments,
      initIntAxis / newAxis, newAxis,
      maxDegreeFlag, maxAbsDegree
    );

    // result with fraction
    double realAxis = (
      qrAdjustments[qrIndex] + qrSigns[qrIndex] *
      (newAxis + std::abs(axis) - initAbsIntAxis)
    );

    axis = realAxis;
  }

  // ex: [ 7°7'22.80"S, 270°7'22.80"E ] to [ 7°07'22.80"S, 91°06'37.20"E ] ??
  void Converter::normalizeDMSAngle(DMSAxis &axis, int maxDegreeFlag) {

    int newDeg = std::abs(axis.getDeg()),
        initSign = axis.getSign(),
        maxAbsDegree;

    setMaxAbsoluteDegree(maxAbsDegree, maxDegreeFlag);

    // less degree don't need to process
    if (lessThanDMSAngle(axis, maxAbsDegree)) return;

    // default for positive
    std::vector<int> qrSigns, qrAdjustments;

    // 'qr' for 'quadrant'
    int qrIndex = setQuadrantDeterminators(
      qrSigns, qrAdjustments,
      axis.getSign(), newDeg,
      maxDegreeFlag, maxAbsDegree
    );

    // assign to new value
    axis.setDeg(qrAdjustments[qrIndex] + qrSigns[qrIndex] * newDeg);

    // fraction value
    if (axis.getMin() > 0 || axis.getSec() > 0) {

      double frac = double(axis.getMin()) / 60 + axis.getSec() / 3600;

      // determining the zero degree sign
      if (qrAdjustments[qrIndex] == 0) {
        if (axis.getDeg() == 0) {
          // negative direction
          if (initSign == -1) {
            if (qrSigns[qrIndex] == 1) axis.setSign(1);
            else if (qrSigns[qrIndex] == -1) axis.setSign(-1);
          }
          // positive direction
          else {
            if (qrSigns[qrIndex] == -1) axis.setSign(-1);
            else if (qrSigns[qrIndex] == 1) axis.setSign(1);
          }
        }
      }
      // occurs when transition from 180'E to 180'W or near polar latitude rebound
      else {
        frac = 1 - frac;
        axis.setDeg(axis.getDeg() - 1, true);
      }

      // fraction result
      double min_d = frac * 60;
      axis.setMin(int(min_d));
      axis.setSec((min_d - int(min_d)) * 60);
    }
  }

  bool Converter::lessThanDMSAngle(DMSAxis &axis, int maxAbsAngle) {

    int absDeg = std::abs(axis.getDeg());
    bool hasFraction = axis.getMin() > 0 || axis.getSec() > 0;

    if (absDeg < 90 || (absDeg == maxAbsAngle && !hasFraction)) {
      return true;
    }
    return false;
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