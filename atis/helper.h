/**
 * ATIS helper file. This file contains includes and other practical
 * defines that would clutter up the main atis.ino file.
 * Copyright (C) 2023 PixelSergey
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

#ifndef ATIS_HELPER
#define ATIS_HELPER

// Enable or disable debug printing and serial features

#if DEBUG
    #define D_SerialBegin(...)  Serial.begin(__VA_ARGS__)
    #define D_print(...)        Serial.print(__VA_ARGS__)
    #define D_write(...)        Serial.write(__VA_ARGS__)
    #define D_println(...)      Serial.println(__VA_ARGS__)
#else
    #define D_SerialBegin(...)
    #define D_print(...)
    #define D_write(...)
    #define D_println(...)
#endif

// Shortcuts

#define PushToken(x) pushToken(phrase, size_phrase, pos, x);
#define PushNumbers(x, y) pushNumbers(phrase, size_phrase, pos, x, y);
#define PushChars(x, y) pushChars(phrase, size_phrase, pos, x, y);
#define PushDistance(x) pushDistance(phrase, size_phrase, pos, x);
#define PushWeather(x, y) pushWeather(phrase, size_phrase, pos, x, y);
#define PushHeight(x) pushHeight(phrase, size_phrase, pos, x);
#define Match(x) match.str(x).c_str()
#define Matched(x) match[x].length()

// STL includes

#include <string>
#include <vector>
#include <regex>

// Arduino library includes

#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClientSecureBearSSL.h>

#include "AudioFileSourcePROGMEM.h"
#include "AudioGeneratorMP3.h"
#include "AudioOutputI2SNoDAC.h"

// Audio include

#include "audio.h"

// Enums

enum TokenType {
    ZERO, ONE, TWO, THREE, FOUR, FIVE, SIX, SEVEN, EIGHT, NINER,
    ALPHA, BRAVO, CHARLIE, DELTA, ECHO, FOXTROT, GOLF, HOTEL, INDIA, JULIET, KILO, LIMA, MIKE, NOVEMBER, OSCAR, PAPA, QUEBEC, ROMEO, SIERRA, TANGO, UNIFORM, VICTOR, WHISKEY, XRAY, YANKEE, ZULU,
    THOUSAND,
    HUNDRED,
    FEET,
    METERS,
    KILOMETERS,
    THIS_IS,
    KUMPULA,
    HYVINKAA,
    INFORMATION,
    AT,
    TIME,
    NO_WEATHER_INFORMATION,
    AUTOMATIC_WEATHER_REPORT,
    WIND,
    CALM,
    VARIABLE,
    KNOTS,
    GUSTING,
    BETWEEN,
    AND,
    CAVOK,
    VISIBLE_RANGE,
    LESS_THAN,
    MORE_THAN,
    INCREASING,
    DECREASING,
    REMAINING,
    CLOUDS,
    FEW,
    SCATTERED,
    BROKEN,
    OVERCAST,
    CUMULONIMBUS,
    TOWERING_CUMULUS,
    NO_SIGNIFICANT_CLOUD,
    NO_CLOUD_DETECTED,
    VERTICAL,
    VISIBILITY,
    TEMPERATURE,
    DEWPOINT,
    UNKNOWN,
    MINUS,
    DEGREES,
    QNH,
    WINDSHEAR,
    ALL,
    RUNWAY,
    ERROR,
    WEATHER,
    HEAVY,
    LIGHT,
    VICINITY,
    RECENT,
    SHALLOW,
    PATCHES_OF,
    PARTIAL,
    LOW_DRIFTING,
    BLOWING,
    SHOWERS,
    THUNDERSTORMS,
    FREEZING,
    DRIZZLE,
    RAIN,
    SNOW,
    SNOW_GRAINS,
    ICE_PELLETS,
    HAIL,
    SMALL_HAIL,
    UNKNOWN_PRECIPITATION,
    MIST,
    FOG,
    SMOKE,
    VOLCANIC_ASH,
    WIDESPREAD_DUST,
    SAND,
    HAZE,
    SAND_WHIRLS,
    SQUALLS,
    FUNNEL_CLOUD,
    SAND_STORM,
    DUST_STORM,
};

// This enum contains all the possible METAR tokens
// Must be updated if the METAR standard changes or bugs are found
enum InformationType {
    I_STATION,
    I_TIME,
    I_NIL,
    I_AUTO,
    I_WIND,
    I_VARIABLE,
    I_CAVOK,
    I_VISIBILITY,
    I_RVR,
    I_WEATHER,
    I_CLOUD,
    I_NSC,
    I_NCD,
    I_VERTICAL,
    I_TEMPERATURE,
    I_QNH,
    I_WINDSHEAR,
    I_ALL,
    I_RWY,
    I_RUNWAY_NUMBER,
    I_ERROR,
};

// This map must be updated if audio files are added or removed

constexpr std::pair<const unsigned char*, unsigned int> tokenToAudio[] = {
    {A_mp3, A_mp3_len},
    {B_mp3, B_mp3_len},
    {C_mp3, C_mp3_len},
};

// This map must be updated if the METAR standard changes or bugs are found
constexpr std::pair<const char*, InformationType> regexToToken[] = {
    {"(ILZM)|(ILZD)|(IL[A-Z]{2}|EF[A-Z]{2})", I_STATION},
    {"[0-9]{2}([0-9]{4})Z", I_TIME},
    {"NIL", I_NIL},
    {"AUTO", I_AUTO},
    {"(?:((?:\\\\\\/){5})|(00000)|(?:(VRB)|([0-9]{3}))([0-9]{2})(?:G([0-9]{2}))?)KT", I_WIND},
    {"([0-9]{3})V([0-9]{3})", I_VARIABLE},
    {"CAVOK", I_CAVOK},
    {"((?:\\\\\\/){4})|([0-9]{4})", I_VISIBILITY},
    {"R([0-9]{2})\\\\\\/(?:(M)|(P))?([0-9]{4})(?:(U)|(D)|(N))?", I_RVR},
    {"(?:((?:\\\\\\/){6})|(?:(FEW)|(SCT)|(BKN)|(OVC))([0-9]{3}))(?:(CB)|(TCU))?", I_CLOUD},
    {"NSC", I_NSC},
    {"NCD", I_NCD},
    {"VV([0-9]{3})", I_VERTICAL},
    {"(?:(M)?([0-9]{2})|((?:\\\\\\/){2}))\\\\\\/(?:(M)?([0-9]{2})|((?:\\\\\\/){2}))", I_TEMPERATURE},
    {"Q(?:([0-9]{4})|((?:\\\\\\/){4}))", I_QNH},
    {"WS", I_WINDSHEAR},
    {"ALL", I_ALL},
    {"RWY", I_RWY},
    {"R([0-9]{2})", I_RUNWAY_NUMBER},
    {"((?:\\\\\\/){2})|(\\+)?(-)?((?:[A-Z]{2}){1,3})", I_WEATHER},
};

constexpr int weatherType[] = {
    17238,
    17746,
    18765,
    17218,
    21072,
    21060,
    19522,
    18515,
    21332,
    23110,
    23108,
    16722,
    20051,
    18259,
    19536,
    21063,
    21319,
    20565,
    21058,
    18246,
    21830,
    16726,
    21828,
    16723,
    23112,
    20304,
    20819,
    17222,
    21331,
    21316,
};

#endif