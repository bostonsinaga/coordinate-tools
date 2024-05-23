#ifndef __COORDINATE_TOOLS__POINT_CPP__
#define __COORDINATE_TOOLS__POINT_CPP__

#include "point.h"

namespace coordinate_tools {

  /** Quadrant */

  void Quadrant::setMaxAbsoluteDegree(
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

  int Quadrant::setDeterminators(
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

  /** Decimal Point */

  std::string DecimalPoint::stringify(std::string separator, bool swapped) {
    double toStrLat = lat, toStrLng = lng;

    if (swapped) {
      toStrLat = lng;
      toStrLng = lat;
    }

    return std::to_string(toStrLat) + separator + std::to_string(toStrLng);
  }

  // ex: [ -7.123, 365.123 ] to [ -7.123, 5.123 ]
  void DecimalPoint::normalizeAngle(double &axis, int maxDegreeFlag) {

    int initIntAxis = int(axis),
        initAbsIntAxis = std::abs(initIntAxis),
        newAxis = initAbsIntAxis,
        maxAbsDegree;

    Quadrant::setMaxAbsoluteDegree(maxAbsDegree, maxDegreeFlag);

    // less degree don't need to process
    if (newAxis <= maxAbsDegree) return;

    // default positive
    std::vector<int> qrSigns, qrAdjustments;

    // 'qr' for 'quadrant'
    int qrIndex = Quadrant::setDeterminators(
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

  void DecimalPoint::normalizePoint(DecimalPoint *pt) {
    // exceeded
    if (std::abs(pt->lat) > 90) {
      // inverted
      if (std::abs(pt->lng) <= 90) {
        normalizeAngle(pt->lat, Quadrant::MAX_DEG_180);
        switchAxis(pt);
      }
      // normal
      else {
        normalizeAngle(pt->lat, Quadrant::MAX_DEG_90);
        normalizeAngle(pt->lng, Quadrant::MAX_DEG_180);
      }
    }
    // normal
    else normalizeAngle(pt->lng, Quadrant::MAX_DEG_180);
  }

  /** DMS Point */

  std::string DMSPoint::getLetter(DMSAxis *axis, int &axisPart) {
    // positive
    if (axis->getSign() == 1) {
      if (axisPart == LAT_PART) return "N";
      else if (axisPart == LNG_PART) return "E";
    }
    // negative
    else {
      if (axisPart == LAT_PART) return "S";
      else if (axisPart == LNG_PART) return "W";
    }
    return "";
  }

  DMSPoint::DMSPoint(
    int lat_deg, int lat_min, double lat_sec,
    int lng_deg, int lng_min, double lng_sec
  ) {
    lat.setDeg(lat_deg);
    lat.setMin(lat_min);
    lat.setSec(lat_sec);
    lng.setDeg(lng_deg);
    lng.setMin(lng_min);
    lng.setSec(lng_sec);
    normalizePoint(this);
  }

  void DMSPoint::empty() {
    lat.setDeg(0);
    lat.setMin(0);
    lat.setSec(0);
    lng.setDeg(0);
    lng.setMin(0);
    lng.setSec(0);
  }

  std::string DMSPoint::stringifySingle(int axisPart) {
    DMSAxis *usedPart;

    if (axisPart == LAT_PART) usedPart = &lat;
    else if (axisPart == LNG_PART) usedPart = &lng;
    else return "";

    std::stringstream secStr;
    secStr << std::fixed << std::setprecision(2) << usedPart->getSec();

    std::string minStr = std::to_string(usedPart->getMin()) + "'";
    if (usedPart->getMin() < 10) minStr = "0" + minStr;

    return (
      std::to_string(usedPart->getDeg()) + "\370" + 
      minStr + secStr.str() + "\"" +
      getLetter(usedPart, axisPart)
    );
  }

  std::string DMSPoint::stringifyPair(std::string separator, bool swapped) {
    if (swapped) return (
      stringifySingle(LNG_PART) + separator +
      stringifySingle(LAT_PART)
    );
    else return (
      stringifySingle(LAT_PART) + separator +
      stringifySingle(LNG_PART)
    );
  }

  // ex: [ 7°7'22.80"S, 270°7'22.80"E ] to [ 7°07'22.80"S, 89°52'37.20"W ]
  void DMSPoint::normalizeAngle(DMSAxis &axis, int maxDegreeFlag) {

    int newDeg = std::abs(axis.getDeg()),
        initSign = axis.getSign(),
        maxAbsDegree;

    Quadrant::setMaxAbsoluteDegree(maxAbsDegree, maxDegreeFlag);

    // less degree don't need to process
    if (lessThanAngle(axis, maxAbsDegree)) return;

    // default for positive
    std::vector<int> qrSigns, qrAdjustments;

    // 'qr' for 'quadrant'
    int qrIndex = Quadrant::setDeterminators(
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
      // occurs when transition from 180°E to 180°W or near polar latitude rebound
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

  void DMSPoint::normalizePoint(DMSPoint *pt) {
    // exceeded
    if (!lessThanAngle(pt->lat, 90)) {
      // inverted
      if (lessThanAngle(pt->lng, 90)) {
        normalizeAngle(pt->lat, Quadrant::MAX_DEG_180);
        switchAxis(pt);
      }
      // normal
      else {
        normalizeAngle(pt->lat, Quadrant::MAX_DEG_90);
        normalizeAngle(pt->lng, Quadrant::MAX_DEG_180);
      }
    }
    // normal
    else normalizeAngle(pt->lng, Quadrant::MAX_DEG_180);
  }

  bool DMSPoint::lessThanAngle(DMSAxis &axis, int maxAbsAngle) {

    int absDeg = std::abs(axis.getDeg());
    bool hasFraction = axis.getMin() > 0 || axis.getSec() > 0;

    if (absDeg < 90 || (absDeg == maxAbsAngle && !hasFraction)) {
      return true;
    }
    return false;
  }
}

#endif // __COORDINATE_TOOLS__POINT_CPP__