/**
 * ATIS configuration file. This file may be edited to change,
 * for example, pin assignments, the WiFi credentials, the URL, and other details.
 *  
 * The following values can be configured:
 * - DEBUG: Set to 1 to enable Serial and printing, 0 to disable
 * - PIN_LED: The pin that the LED is connected to
 * - PIN_CS: The SD card's Chip Select pin 
 * - URL: The URL where ATIS gets its data. Currently only ilmailusaa.fi URLs are supported. 
 * 
 * Copyright (C) 2023-2024 PixelSergey
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

#ifndef ATIS_CONFIG
#define ATIS_CONFIG

#define DEBUG 1
#define PIN_LED D2
#define PIN_CS D1
#define URL "https://ilmailusaa.fi/backend.php?{%22mode%22:%22awsaviation%22,%22radius%22:%22100%22,%22points%22:[{%22_locationName%22:%22ILZM%22}]}"
#define PATH_URL "/url.txt"
#define VOICEPACK "female"
#define PATH_VOICEPACK "/voicepack.txt"

#define WIFI_SSID "SSID"
#define WIFI_PASSWORD "PASSWORD"
#define PATH_SSID "/ssid.txt"
#define PATH_PASSWORD "/password.txt"

#define SIZE_PHRASE 200
#define SIZE_RESPONSE 350
#define SIZE_METAR 150
#define SIZE_PARSED 25

#define SIZE_VOICEPACK 20
#define SIZE_URL 200
#define SIZE_SSID 50
#define SIZE_PASSWORD 50

#endif
