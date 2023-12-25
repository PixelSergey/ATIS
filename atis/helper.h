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

// STL includes

#include <string>
#include <vector>
#include <map>
#include <regex>

// Arduino library includes

#include <Arduino.h>
#include <WiFiManager.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClient.h>
#include <WiFiClientSecureBearSSL.h>
#include <ArduinoJson.h>

#include "AudioFileSourcePROGMEM.h"
#include "AudioGeneratorMP3.h"
#include "AudioOutputI2SNoDAC.h"

// Audio include

#include "audio.h"

// This map must be updated if audio files are added or removed
std::map<std::string, std::pair<const unsigned char*, unsigned int>> tokenToAudio = {
    {"A", {A_mp3, A_mp3_len}},
    {"B", {B_mp3, B_mp3_len}},
    {"C", {C_mp3, C_mp3_len}},
    {"THIS_IS", {THIS_IS_mp3, THIS_IS_mp3_len}},
    {"KUMPULA", {KUMPULA_mp3, KUMPULA_mp3_len}},
    {"INFORMATION", {INFORMATION_mp3, INFORMATION_mp3_len}},
};

// This enum contains all the possible METAR tokens
// Must be updated if the METAR standard changes or bugs are found
enum TokenType {
    STATION,
    TIME,
    NIL,
    AUTO,
    WIND,
    VARIABLE,
    VISIBILITY,
    RVR,
    WEATHER,
    CLOUD,
    NSC,
    NCD,
    VERTICAL,
    TEMPERATURE,
    QNH,
    WINDSHEAR,
    ALL,
    RWY,
    RUNWAY_NUMBER,
    ERROR,
};

// This map must be updated if the METAR standard changes or bugs are found
std::map<std::string, TokenType> regexToToken = {
    {"^(IL..$|^EF..)$", STATION},
    {"^[0-9]{2}([0-9]{4})Z$", TIME},
    {"^NIL$", NIL},
    {"^AUTO$", AUTO},
    {"^(?:(\\/{5})|(VRB|[0-9]{3})([0-9]{2})(?:G([0-9]{2}))?)KT$", WIND},
    {"^([0-9]{3})V([0-9]{3})$", VARIABLE},
    {"^(\\/{4})|([0-9]{4})$", VISIBILITY},
    {"^R([0-9]{2})\\/([MP]?)([0-9]{4})([UDN]?)$", RVR},
    {"^(\\+|-|VC|RE|MI|BC|PR|DR|BL|SH|TS|FZ|DZ|RA|SN|SG|PL|GR|GS|UP|BR|FG|FU|VA|DU|SA|HZ|PO|SQ|FC|SS|DS)(\\+|-|VC|RE|MI|BC|PR|DR|BL|SH|TS|FZ|DZ|RA|SN|SG|PL|GR|GS|UP|BR|FG|FU|VA|DU|SA|HZ|PO|SQ|FC|SS|DS)?(\\+|-|VC|RE|MI|BC|PR|DR|BL|SH|TS|FZ|DZ|RA|SN|SG|PL|GR|GS|UP|BR|FG|FU|VA|DU|SA|HZ|PO|SQ|FC|SS|DS)?$", WEATHER},
    {"^(FEW|SCT|BKN|OVC)([0-9]{3})(CB|TCU)?$", CLOUD},
    {"^NSC$", NSC},
    {"^NCD$", NCD},
    {"^VV([0-9]{3})$", VERTICAL},
    {"^(?:(M)?([0-9]{2})|\\/{2})\\/(?:(M)?([0-9]{2})|\\/{2})$", TEMPERATURE},
    {"^Q(?:([0-9]{4})|\\/{4})$", QNH},
    {"^WS$", WINDSHEAR},
    {"^ALL$", ALL},
    {"^RWY$", RWY},
    {"^R([0-9]{2})$", RUNWAY_NUMBER},
};

#endif