#ifndef __COORDINATE_TOOLS__CONVERTER_CPP__
#define __COORDINATE_TOOLS__CONVERTER_CPP__

#include "converter.h"

namespace coordinate_tools {

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

    // opposite value
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

    int newDeg = std::abs(axis.getDeg()),
        initSign = axis.getSign(),
        maxAbsDegree;

    // normalize 'maxDegreeFlag'
    setMaxAbsoluteDegree(maxAbsDegree, maxDegreeFlag);

    // less degree don't need to process
    if (lessThanDMSAngle(axis, maxAbsDegree)) return;

    // default for positive
    std::vector<int> qrSigns, qrAdjustments;

    // 'qr' for 'quadrant'
    setQuadrantDeterminators(
      qrSigns, qrAdjustments,
      axis.getSign(), maxDegreeFlag, maxAbsDegree
    );

    // process with remainder
    int qrIndex = (newDeg / maxAbsDegree - 1) % qrSigns.size();
    newDeg %= maxAbsDegree;

    // assign to new value
    axis.setDeg(qrAdjustments[qrIndex] + qrSigns[qrIndex] * newDeg);

    // fraction value
    if (axis.getMin() > 0 || axis.getSec() > 0) {

      double frac = double(axis.getMin()) / 60 - axis.getSec() / 3600;
      double min_d;

      if (qrAdjustments[qrIndex] != 0) {
        min_d = frac * 60;

        if (qrSigns[qrIndex] == -1) {
          axis.setDeg(axis.getDeg() - 1);
        }
        else axis.setDeg(axis.getDeg() + 1);
      }
      else {
        frac = 1 - frac;
        min_d = frac * 60;
      }

      // determining the zero degree sign
      if (axis.getDeg() == 0 && qrAdjustments[qrIndex] == 0) {
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