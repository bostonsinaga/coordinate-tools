#ifndef __COORDINATE_TOOLS__POINT_H__
#define __COORDINATE_TOOLS__POINT_H__

#include <vector>
#include <iomanip>
#include <sstream>
#include "definitions.h"

namespace coordinate_tools {
  class Quadrant {
  public:
    enum {MAX_DEG_90, MAX_DEG_180};

    static void setMaxAbsoluteDegree(
      int &maxAbsDegree,
      int &maxDegreeFlag
    );

    static int setDeterminators(
      std::vector<int> &qrSigns,
      std::vector<int> &qrAdjustments,
      int valSign, int &updateDegree,
      int &maxDegreeFlag, int &maxAbsDegree
    );
  };

  class DecimalPoint {
  public:
    DecimalPoint() {}

    DecimalPoint(DecimalPoint &pt_in) {
      lat = pt_in.lat;
      lng = pt_in.lng;
      normalizePoint(this);
    }

    DecimalPoint(const DecimalPoint &pt_in) {
      lat = pt_in.lat;
      lng = pt_in.lng;
      normalizePoint(this);
    }

    DecimalPoint(double lat_in, double lng_in) {
      lat = lat_in;
      lng = lng_in;
      normalizePoint(this);
    }

    static void switchAxis(DecimalPoint *pt) {
      double buffer = pt->lat;
      pt->lat = pt->lng;
      pt->lng = buffer;
    }

    std::string stringify(std::string separator = ",", bool swapped = false);
    static void normalizeAngle(double &axis, int maxDegreeFlag = Quadrant::MAX_DEG_90);
    static void normalizePoint(DecimalPoint *pt);

    double lat = 0, lng = 0;
  };

  class DMSAxis {
  private:
    /**
     * The value of 'deg', 'min', and 'sec'
     * are absolute number.
     */
    int deg = 0, min = 0, sign = 1;
    double sec = 0.0;

  public:
    DMSAxis() {}

    DMSAxis(int deg_in, int min_in, double sec_in) {
      setDeg(deg_in);
      setMin(min_in);
      setSec(sec_in);
    }

    int getDeg(bool withSign = false) {
      if (withSign) return sign * deg;
      return deg;
    }

    int getSign() { return sign; }
    int getMin() { return min; }
    double getSec() { return sec; }

    void setDeg(int deg_in, bool noSign = false) {
      deg = std::abs(deg_in);

      if (!noSign) {
        if (deg_in != 0) sign = deg_in / deg;
        else sign = 1;
      }
    }

    void setMin(int min_in) {
      min = std::abs(min_in);
    }

    void setSec(double sec_in) {
      sec = std::abs(sec_in);
    }

    void setSign(int sign_in) {
      if (sign_in >= 0) sign = 1;
      else sign = -1;
    }
  };

  class DMSPoint {
  public:
    DMSAxis lat, lng;

  private:
    std::string getLetter(DMSAxis *axis, int &axisPart);

  public:
    DMSPoint() {}

    DMSPoint(DMSPoint &pt_in) {
      lat = pt_in.lat;
      lng = pt_in.lng;
      normalizePoint(this);
    }

    DMSPoint(const DMSPoint &pt_in) {
      lat = pt_in.lat;
      lng = pt_in.lng;
      normalizePoint(this);
    }

    DMSPoint(DMSAxis lat_in, DMSAxis lng_in) {
      lat = lat_in;
      lng = lng_in;
      normalizePoint(this);
    }

    DMSPoint(
      int lat_deg, int lat_min, double lat_sec,
      int lng_deg, int lng_min, double lng_sec
    );

    void empty();
    std::string stringifySingle(int axisPart);
    std::string stringifyPair(std::string separator = ",", bool swapped = false);

    static void switchAxis(DMSPoint *pt) {
      DMSAxis buffer = pt->lat;
      pt->lat = pt->lng;
      pt->lng = buffer;
    }

    static void normalizePoint(DMSPoint *pt);
    static void normalizeAngle(DMSAxis &axis, int maxDegreeFlag = Quadrant::MAX_DEG_90);
    static bool lessThanAngle(DMSAxis &axis, int maxAbsAngle);
  };
}

#endif // __COORDINATE_TOOLS__POINT_H__