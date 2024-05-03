#ifndef __COORDINATE_TOOLS__PARSER_CPP__
#define __COORDINATE_TOOLS__PARSER_CPP__

#include "parser.h"

namespace coordinate_tools {

  // // returns 'pairNeedTest'
  // void Parser::testString(AxisString &axStr, bool keepAdd) {

  //   bool isCurNum = false,
  //        pairNeedTest = false;

  //   if (!keepAdd) {
  //     // negative sign
  //     if (text[i] == '-' &&
  //       ((axStr.axisPart == axStr.latPart && axStr.latStr.length() == 0) ||
  //       (axStr.axisPart == axStr.lngPart && axStr.lngStr.length() == 0))
  //     ) {
  //       keepAdd = true;
  //     }
  //     // number
  //     else if (std::isdigit(text[i])) {
  //       keepAdd = true;
  //       axStr.chLast = axStr.numLast;
  //       isCurNum = true;
  //     }
  //     // dot or separator
  //     else if (text[i] == '.') {
  //       if (!axStr.anySeparator) {
  //         if (!axStr.dmsPart == axStr.secPart) {
  //           keepAdd = true;
  //           axStr.anySeparator = true;
  //         }
  //       }
  //       else axStr.switchAxis(pairNeedTest);
  //     }
  //     // primary separator
  //     else if (text[i] == ',') {
  //       if (!axStr.anySeparator) axStr.addToString(true, '.');
  //       axStr.switchAxis(pairNeedTest);
  //     }
  //     // secondary separator
  //     else if (text[i] == ' ') {
  //       if (axStr.anySeparator) axStr.switchAxis(pairNeedTest);
  //       else if (
  //         !axStr.anySeparator && axStr.chLast == axStr.numLast &&
  //         axStr.isUsingDMS(axStr.minPart)
  //       ) {
  //         axStr.addToString(true, '\0', true);
  //       }
  //     }
  //     // not numeric or a wild dash character
  //     else {
  //       axStr.chLast = axStr.othLast;
  //       pairNeedTest = true;
  //     }
  //   }

  //   if (i == text.length() - 1) pairNeedTest = true;

  //   if (!isCurNum && axStr.chLast != axStr.othLast) {
  //     axStr.chLast = axStr.incLast;
  //   }

  //   axStr.addToString(keepAdd, text[i]);
  //   return pairNeedTest;
  // }

  // // ex: [ -7.123, 110.123 ]
  // bool Parser::testDecimal(std::string text, bool needReset) {

  //   if (needReset) reset();
  //   AxisString axStr;

  //   for (int i = 0; i < text.length(); i++) {

  //     if (testString(axStr)) {
  //       double lat = 0, lng = 0;

  //       try { lat = std::stod(axStr.latStr); }
  //       catch(...) {}

  //       try { lng = std::stod(axStr.lngStr); }
  //       catch(...) {}

  //       bool isStringsContainVar = axStr.isStringsContain();

  //       if (isStringsContainVar || axStr.chLast == axStr.othLast) {
  //         axStr.axisPart = axStr.latPart;
  //         axStr.latStr = "";
  //         axStr.lngStr = "";

  //         if (isStringsContainVar) {
  //           points.push_back(Point(lat, lng));
  //           axStr.anySucceed = true;
  //         }
  //       }
  //     }
  //   }

  //   return axStr.anySucceed;
  // }

  // ex: [ 7°7'22.80"S, 110°7'22.80"E ]
  bool Parser::testDMS(std::string text, bool needReset) {
    if (needReset) reset();

    bool anySucceed = false,
         anySeparator = false,
         halfDegSign = false,
         prevNum = false;

    int valSigns[2] = {1, 1},
        axisParts[2] = {LAT_PART, LNG_PART},
        axisIndex = 0;

    enum {degPart, minPart, secPart, sgnPart};
    int dmsParts[4] = {degPart, degPart, degPart, degPart};
    std::string dmsStr[2][3];
    int dmsIndex = 0;

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

    auto setAxisState = [&](int vSg, int axPt) {

      valSigns[axisIndex] *= vSg;
      axisParts[axisIndex] = axPt;
      axisIndex++;

      for (int i = 0; i < 4; i++) { dmsParts[i] = degPart; }
      dmsIndex = 0;

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

    /** LOOP */

    for (int i = 0; i < text.length(); i++) {

      bool keepAdd = false,
           isCurNum = false,
           pairNeedTest = false;

      // degree sign
      if (int(text[i]) == 45 && !halfDegSign &&
        testDMSParts(degPart)
      ) {
        halfDegSign = true;
        continue;
      }
      else if (
        int(text[i]) == -90 && halfDegSign &&
        testDMSParts(degPart)
      ) {
        halfDegSign = false;
        setDMSParts(minPart);
      }
      // minute sign
      else if (text[i] == '\'' && testDMSParts(minPart)) {
        setDMSParts(secPart);
      }
      // second sign
      else if (text[i] == '"' && testDMSParts(secPart)) {
        setDMSParts(sgnPart);
      }
      else if (std::isalpha(text[i]) && (
        testDMSParts(sgnPart) || (prevNum && testDMSParts(secPart))
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
        dmsParts[dmsIndex] == degPart &&
        dmsStr[axisParts[axisIndex]][degPart] == ""
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
        dmsParts[dmsIndex] == secPart
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
      if (keepAdd) {
        dmsStr[axisParts[axisIndex]][dmsParts[dmsIndex]] += text[i];
      }

      // push to DMS points
      if (pairNeedTest) {

        DMSPoint testPt;
        bool anyFail = false;
        axisIndex = 0; // reset

        try {
          testPt = DMSPoint(
            // latitude
            std::stod(dmsStr[axisParts[LAT_PART]][degPart]) * valSigns[LAT_PART],
            std::stod(dmsStr[axisParts[LAT_PART]][minPart]),
            std::stod(dmsStr[axisParts[LAT_PART]][secPart]),
            // longitude
            std::stod(dmsStr[axisParts[LNG_PART]][degPart]) * valSigns[LNG_PART],
            std::stod(dmsStr[axisParts[LNG_PART]][minPart]),
            std::stod(dmsStr[axisParts[LNG_PART]][secPart])
          );
        }
        catch(...) { anyFail = true; }

        /** RESET */

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