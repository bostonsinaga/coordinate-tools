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

    double lat = 0, lng = 0;
  };

  class DMSAxis {
  public:
    int deg = 0, min = 0;
    double sec = 0.0;
  };

  class DMSPoint {
  public:
    DMSAxis lat, lng;

  private:
    std::string getLetter(DMSAxis *axis, int &axisPart) {
      // positive
      if (axis->deg >= 0) {
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

    DMSPoint(
      int lat_deg, int lat_min, double lat_sec,
      int lng_deg, int lng_min, double lng_sec
    ) {
      lat.deg = lat_deg;
      lat.min = lat_min;
      lat.sec = lat_sec;
      lng.deg = lng_deg;
      lng.min = lng_min;
      lng.sec = lng_sec;
    }

    void empty() {
      lat.deg = 0;
      lat.min = 0;
      lat.sec = 0;
      lng.deg = 0;
      lng.min = 0;
      lng.sec = 0;
    }

    std::string stringifySingle(int axisPart) {
      DMSAxis *usedPart;

      if (axisPart == LAT_PART) usedPart = &lat;
      else if (axisPart == LNG_PART) usedPart = &lng;
      else return "";

      std::stringstream secStr;
      secStr << std::fixed << std::setprecision(2) << usedPart->sec;

      return (
        std::to_string(std::abs(usedPart->deg)) + "\370" + 
        std::to_string(usedPart->min) + "'" +
        secStr.str() + "\"" +
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