/**
 *  ATIS configuration file. This file may be edited to change,
 *  for example, pin assignments and other details.
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

#include <Arduino.h>
#include <string>
#include <vector>
#include <map>

#include <WiFiManager.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClient.h>
#include <WiFiClientSecureBearSSL.h>
#include <ArduinoJson.h>

#include "AudioFileSourcePROGMEM.h"
#include "AudioGeneratorMP3.h"
#include "AudioOutputI2SNoDAC.h"

#ifndef ATIS_CONFIG
#define ATIS_CONFIG

// Configure these values

#define DEBUG 1  // Set to 0 in production to save space
#define LED D7
#define URL "https://ilmailusaa.fi/backend.php?{%22mode%22:%22awsaviation%22,%22radius%22:%22100%22,%22points%22:[{%22_locationName%22:%22ILZD%22}]}"

// Automatic debug print handling

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

// Audio data

#include "audio.h"

#endif
