#ifndef __COORDINATE_TOOLS__POINT_H__
#define __COORDINATE_TOOLS__POINT_H__

#include <iomanip>
#include <sstream>
#include "definitions.h"

namespace coordinate_tools {
  class DecimalPoint {
  public:
    DecimalPoint() {}

    DecimalPoint(DecimalPoint &pt_in) {
      lat = pt_in.lat;
      lng = pt_in.lng;
    }

    DecimalPoint(const DecimalPoint &pt_in) {
      lat = pt_in.lat;
      lng = pt_in.lng;
    }

    DecimalPoint(double lat_in, double lng_in) {
      lat = lat_in;
      lng = lng_in;
    }

    std::string stringify(std::string separator = ",", bool swapped = false) {
      double toStrLat = lat, toStrLng = lng;

      if (swapped) {
        toStrLat = lng;
        toStrLng = lat;
      }

      return std::to_string(toStrLat) + separator + std::to_string(toStrLng);
    }

    double lat = 0, lng = 0;
  };

  class DMSAxis {
  private:
    /**
     * The value sign is contained in 'deg'.
     * 'min' and 'sec' are absolute numbers.
     */
    int deg = 0, min = 0;
    double sec = 0.0;

  public:
    DMSAxis() {}

    DMSAxis(int deg_in, int min_in, double sec_in) {
      deg = deg_in;
      min = std::abs(min_in);
      sec = std::abs(sec_in);
    }

    int getDeg() { return deg; }
    int getMin() { return min; }
    double getSec() { return sec; }

    void setDeg(int deg_in) { deg = deg_in; }
    void setMin(int min_in) { min = min_in; }
    void setSec(double sec_in) { sec = sec_in; }
  };

  class DMSPoint {
  public:
    DMSAxis lat, lng;

  private:
    std::string getLetter(DMSAxis *axis, int &axisPart) {
      // positive
      if (axis->getDeg() >= 0) {
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

  public:
    DMSPoint() {}

    DMSPoint(DMSPoint &pt_in) {
      lat = pt_in.lat;
      lng = pt_in.lng;
    }

    DMSPoint(const DMSPoint &pt_in) {
      lat = pt_in.lat;
      lng = pt_in.lng;
    }

    DMSPoint(DMSAxis lat_in, DMSAxis lng_in) {
      lat = lat_in;
      lng = lng_in;
    }

    DMSPoint(
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

    void empty() {
      lat.setDeg(0);
      lat.setMin(0);
      lat.setSec(0);
      lng.setDeg(0);
      lng.setMin(0);
      lng.setSec(0);
    }

    std::string stringifySingle(int axisPart) {
      DMSAxis *usedPart;

      if (axisPart == LAT_PART) usedPart = &lat;
      else if (axisPart == LNG_PART) usedPart = &lng;
      else return "";

      std::stringstream secStr;
      secStr << std::fixed << std::setprecision(2) << usedPart->getSec();

      std::string minStr = std::to_string(usedPart->getMin()) + "'";
      if (usedPart->getMin() < 10) minStr = "0" + minStr;

      return (
        std::to_string(std::abs( usedPart->getDeg() )) + "\370" + 
        minStr + secStr.str() + "\"" +
        getLetter(usedPart, axisPart)
      );
    }

    std::string stringifyPair(std::string separator = ",", bool swapped = false) {
      if (swapped) return (
        stringifySingle(LNG_PART) + separator +
        stringifySingle(LAT_PART)
      );
      else return (
        stringifySingle(LAT_PART) + separator +
        stringifySingle(LNG_PART)
      );
    }
  };
}

#endif // __COORDINATE_TOOLS__POINT_H__