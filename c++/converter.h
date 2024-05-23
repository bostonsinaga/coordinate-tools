#ifndef __COORDINATE_TOOLS__CONVERTER_H__
#define __COORDINATE_TOOLS__CONVERTER_H__

#include "point.h"

namespace coordinate_tools {
  class Converter {
  public:
    static DecimalPoint dmsToDd(DMSPoint &before);
    static DMSPoint ddToDms(DecimalPoint &before);
  };
}

#endif // __COORDINATE_TOOLS__CONVERTER_H__