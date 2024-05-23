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
}

#endif // __COORDINATE_TOOLS__CONVERTER_CPP__