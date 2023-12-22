/**
 *  ATIS helper file. This file contains includes and other practical
 *  defines that would clutter up the main atis.ino file.
 *  Copyright (C) 2023 PixelSergey
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

#ifndef ATIS_HELPER
#define ATIS_HELPER

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

#include <string>
#include <vector>
#include <map>

#include <Arduino.h>
#include <WiFiManager.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClient.h>
#include <WiFiClientSecureBearSSL.h>
#include <ArduinoJson.h>

#include "AudioFileSourcePROGMEM.h"
#include "AudioGeneratorMP3.h"
#include "AudioOutputI2SNoDAC.h"

#endif