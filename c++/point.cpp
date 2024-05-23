#ifndef __COORDINATE_TOOLS__POINT_CPP__
#define __COORDINATE_TOOLS__POINT_CPP__

#include "point.h"

namespace coordinate_tools {

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
}

#endif // __COORDINATE_TOOLS__POINT_CPP__