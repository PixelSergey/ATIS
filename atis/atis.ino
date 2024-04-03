/**
 * ATIS main program file.
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

#include "atis.h"

TokenType phrase[SIZE_PHRASE];

int getNewMetarPhrase(TokenType* phrase, int size_phrase){
    char response[SIZE_RESPONSE];
    char metar[SIZE_METAR];
    char* parsed[SIZE_PARSED];
    getMetar(response, SIZE_RESPONSE);
    int size_metar = decodeMetar(metar, SIZE_METAR, response, SIZE_RESPONSE);
    int size_parsed = parseMetar(parsed, SIZE_PARSED, metar, size_metar);
    int size_generated = generatePhrase(phrase, size_phrase, parsed, size_parsed);
    return size_generated;
}

void setup(){
    // Pin setup
    pinMode(PIN_LED, OUTPUT);
    digitalWrite(PIN_LED, HIGH);  // Turn on setup light
    D_SerialBegin(115200);

    if (!SD.begin(PIN_CS)){
        D_println("SD initialisation failed");
        return;
    }

    WiFi.mode(WIFI_STA);
    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

    while(WiFi.status() != WL_CONNECTED) delay(100);
    D_println("WiFi connected");

    // Turn off setup light
    digitalWrite(PIN_LED, LOW);
}

void loop(){
    int size_generated = getNewMetarPhrase(phrase, SIZE_PHRASE);
    D_println("Generation complete!");
    //for(int i=0; i<size_generated; i++) playToken(phrase[i]);
    for(int i=0; i<36; i++) playToken(TokenType(i));
    D_println("End of loop\n-----------------------\n");
    delay(2000);
}
