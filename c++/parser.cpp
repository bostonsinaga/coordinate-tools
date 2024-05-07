#ifndef __COORDINATE_TOOLS__PARSER_CPP__
#define __COORDINATE_TOOLS__PARSER_CPP__

#include "parser.h"

namespace coordinate_tools {

  // ex: [ -7.123, 110.123 ]
  bool Parser::testDecimal(std::string text, bool needReset) {
    if (needReset) reset(DECIMAL_POINT_TYPE);

    std::string decStr[2] = {"", ""};
    int axisPart = LAT_PART;

    bool anyComma = false,
         anySucceed = false,
         anySeparator = false;

    enum {ignore_last, number_last, separator_last};
    int chLast = ignore_last;

    enum {sign_part, int_part, frac_part};
    int decParts[2];

    auto resetDecParts = [&]() {
      decParts[LAT_PART] = sign_part;
      decParts[LNG_PART] = sign_part;
    };

    resetDecParts();

    for (int i = 0; i < text.length(); i++) {

      bool keepAdd = false,
           pairNeedTest = false;

      // negative sign
      if (text[i] == '-' && decParts[axisPart] == sign_part) {
        keepAdd = true;
        chLast = ignore_last;
        decParts[axisPart] = int_part;
      }
      // number
      else if (std::isdigit(text[i])) {

        if (decParts[axisPart] == sign_part) {
          decParts[axisPart] = int_part;
        }

        keepAdd = true;
        chLast = number_last;
        anySeparator = false;
      }
      // dot or separator
      else if (text[i] == '.' && decParts[axisPart] == int_part) {
        decParts[axisPart] = frac_part;
        chLast = ignore_last;
        keepAdd = true;
      }
      // separator
      else if (text[i] == ',') {
        chLast = separator_last;

        // error comma
        if (anyComma) {
          anyComma = false;
          pairNeedTest = true;
        }
        // comma separator
        else if (!anySeparator) {
          axisPart++;
          anyComma = true;
          anySeparator = true;
          resetDecParts();
        }
      }
      else if (text[i] == ' ') {

        // whitespace separator
        if (!anySeparator &&
          chLast == number_last &&
          decParts[axisPart] == frac_part
        ) {
          axisPart++;
          anySeparator = true;
          resetDecParts();
        }
        // error whitespace
        else if (chLast != separator_last) {
          pairNeedTest = true;
        }

        chLast = separator_last;
      }
      // error character
      else pairNeedTest = true;

      // always test at the end of iteration
      if (i == text.length() - 1) pairNeedTest = true;

      // add character to string
      if (keepAdd) decStr[axisPart] += text[i];

      // push to decimal points
      if (pairNeedTest) {

        DecimalPoint decPt;
        bool anyFail = false;

        try {
          decPt.lat = std::stod(decStr[LAT_PART]);
          decPt.lng = std::stod(decStr[LNG_PART]);
        }
        catch(...) { anyFail = true; }

        axisPart = LAT_PART;
        decStr[LAT_PART] = "";
        decStr[LNG_PART] = "";
        resetDecParts();

        if (!anyFail) {
          decPoints.push_back(decPt);
          anySucceed = true;
        }
      }
    }

    return anySucceed;
  }

  // ex: [ 7°7'22.80"S, 110°7'22.80"E ]
  bool Parser::testDMS(std::string text, bool needReset) {
    if (needReset) reset(DMS_POINT_TYPE);

    bool anySucceed = false,
         anySeparator = false,
         halfDegSign = false,
         prevNum = false;

    int valSigns[2] = {1, 1},
        axisParts[2] = {LAT_PART, LNG_PART},
        axisIndex = 0;

    enum {degree_part, minute_part, second_part, compass_part};
    int dmsParts[4], dmsIndex;
    std::string dmsStr[2][3];

    /** LAMBDA */

    auto emptyDmsStr = [&](std::string *dmsStr_in) {
      for (int i = 0; i < 3; i++) {
        dmsStr_in[i] = "";
      }
    };

    auto setDMSParts = [&](int flag) {
      dmsIndex++;
      dmsParts[dmsIndex] = flag;
    };

    auto testDMSParts = [&](int part)->bool {
      for (int i = 0; i <= part; i++) {
        if (dmsParts[i] != i) return false;
      }
      return true;
    };

    auto resetAxisParts = [&]() {
      axisParts[LAT_PART] = LAT_PART;
      axisParts[LNG_PART] = LNG_PART;
    };

    auto resetDMSParts = [&]() {
      for (int i = 0; i < 4; i++) { dmsParts[i] = degree_part; }
      dmsIndex = 0;
    };

    auto setAxisState = [&](int vSg, int axPt) {

      valSigns[axisIndex] *= vSg;
      axisParts[axisIndex] = axPt;
      axisIndex++;
      resetDMSParts();

      if (axisIndex == 2) {

        // fix 'axisParts' duplication
        if (axisParts[LAT_PART] == axisParts[LNG_PART]) {
          if (axisParts[LAT_PART] == LAT_PART) {
            axisParts[LNG_PART] = LNG_PART;
          }
          else axisParts[LNG_PART] = LAT_PART;
        }

        // fix swapped 'axisParts'
        if (axisParts[LAT_PART] == LNG_PART &&
          axisParts[LNG_PART] == LAT_PART
        ) {
          resetAxisParts();
          std::string strBuff;

          for (int i = 0; i < 3; i++) {
            strBuff = dmsStr[LAT_PART][i];
            dmsStr[LAT_PART][i] = dmsStr[LNG_PART][i];
            dmsStr[LNG_PART][i] = strBuff;
          }
        }
      }
    };

    // initialize 'dmsParts'
    resetDMSParts();

    /** LOOP */

    for (int i = 0; i < text.length(); i++) {

      bool keepAdd = false,
           isCurNum = false,
           pairNeedTest = false;

      // degree sign
      if (int(text[i]) == 45 && !halfDegSign &&
        testDMSParts(degree_part)
      ) {
        halfDegSign = true;
        continue;
      }
      else if (
        int(text[i]) == -90 && halfDegSign &&
        testDMSParts(degree_part)
      ) {
        halfDegSign = false;
        setDMSParts(minute_part);
      }
      // minute sign
      else if (text[i] == '\'' && testDMSParts(minute_part)) {
        setDMSParts(second_part);
      }
      // second sign
      else if (text[i] == '"' && testDMSParts(second_part)) {
        setDMSParts(compass_part);
      }
      else if (std::isalpha(text[i]) && (
        testDMSParts(compass_part) || (prevNum && testDMSParts(second_part))
      )) {
        // north letter
        if (
          text[i] == 'N' || // general
          text[i] == 'n' || // ...
          text[i] == 'U' || // indonesian
          text[i] == 'u'    // ...
        ) {
          setAxisState(1, LAT_PART);
        }
        // south letter
        else if (
          text[i] == 'S' || // general
          text[i] == 's'    // ...
        ) {
          setAxisState(-1, LAT_PART);
        }
        // east letter
        else if (
          text[i] == 'E' || // general
          text[i] == 'e' || // ...
          text[i] == 'T' || // indonesian
          text[i] == 't'    // ...
        ) {
          setAxisState(1, LNG_PART);
        }
        // west letter
        else if (
          text[i] == 'W' || // general
          text[i] == 'w' || // ...
          text[i] == 'B' || // indonesian
          text[i] == 'b'    // ...
        ) {
          setAxisState(-1, LNG_PART);
        }
      }
      // negative sign
      else if (
        text[i] == '-' &&
        dmsParts[dmsIndex] == degree_part &&
        dmsStr[axisParts[axisIndex]][degree_part] == ""
      ) {
        valSigns[axisIndex] *= -1;
      }
      // number
      else if (std::isdigit(text[i])) {
        prevNum = true;
        isCurNum = true;
        keepAdd = true;
      }
      // dot as decimal point
      else if (text[i] == '.' && prevNum &&
        dmsParts[dmsIndex] == second_part
      ) {
        keepAdd = true;
      }
      // comma separator
      else if (text[i] == ',') {
        if (axisIndex > 0) {
          anySeparator = true;
        }
        // error comma
        else pairNeedTest = true;
      }
      // whitespace or separator
      else if (text[i] == ' ') {
        if (!anySeparator && prevNum) {
          anySeparator = true;
        }
      }
      // error character
      else pairNeedTest = true;

      // always test at the end of iteration
      if (axisIndex == 2 ||
        i == text.length() - 1
      ) {
        pairNeedTest = true;
      }

      // reset 'prevNum'
      if (!isCurNum) prevNum = false;

      // add character to string
      if (keepAdd) dmsStr[axisParts[axisIndex]][dmsParts[dmsIndex]] += text[i];

      // push to DMS points
      if (pairNeedTest) {

        DMSPoint testPt;
        bool anyFail = false;
        axisIndex = 0; // reset

        try {
          testPt = DMSPoint(
            // latitude
            std::stod(dmsStr[axisParts[LAT_PART]][degree_part]) * valSigns[LAT_PART],
            std::stod(dmsStr[axisParts[LAT_PART]][minute_part]),
            std::stod(dmsStr[axisParts[LAT_PART]][second_part]),
            // longitude
            std::stod(dmsStr[axisParts[LNG_PART]][degree_part]) * valSigns[LNG_PART],
            std::stod(dmsStr[axisParts[LNG_PART]][minute_part]),
            std::stod(dmsStr[axisParts[LNG_PART]][second_part])
          );
        }
        catch(...) { anyFail = true; }

        // reset
        for (int i = 0; i < 2; i++) {
          valSigns[i] = 1;
          emptyDmsStr(dmsStr[i]);
        }

        resetAxisParts();

        // push to points
        if (!anyFail) {
          dmsPoints.push_back(testPt);
          anySucceed = true;
        }
      }
    }

    return anySucceed;
  }
}

#endif // __COORDINATE_TOOLS__PARSER_CPP__