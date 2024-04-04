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
char voicepack[SIZE_VOICEPACK];
char url[SIZE_URL];

int getNewMetarPhrase(TokenType* phrase, int size_phrase){
    char response[SIZE_RESPONSE];
    char metar[SIZE_METAR];
    char* parsed[SIZE_PARSED];
    getMetar(response, SIZE_RESPONSE, url);
    int size_metar = decodeMetar(metar, SIZE_METAR, response, SIZE_RESPONSE);
    int size_parsed = parseMetar(parsed, SIZE_PARSED, metar, size_metar);
    int size_generated = generatePhrase(phrase, size_phrase, parsed, size_parsed);
    return size_generated;
}

void loadConfig(char* target, int size_target, const char* defaultText, const char* filepath){
    bool configSuccess = false;

    if(SD.exists(filepath)){
        File config = SD.open(filepath, FILE_READ);
        if(config){
            unsigned char data[size_target];
            int read = config.read(data, size_target);
            config.close();
            if(read > 0){
                snprintf(target, min(read+1, size_target), "%s", data);
                for(int i=0; i<size_target; i++) if(target[i]=='\n' || target[i]=='\t') target[i]='\0';
                configSuccess = true; 
            }
        }
    }
    
    if(!configSuccess){
        strncpy(target, defaultText, size_target);
    }
}

void setup(){
    // Pin setup
    pinMode(PIN_LED, OUTPUT);
    digitalWrite(PIN_LED, HIGH);  // Turn on setup light
    pinMode(PIN_BUTTON, INPUT_PULLUP);

    D_SerialBegin(115200);
    D_println();

    if (!SD.begin(PIN_CS)){
        D_println("SD initialisation failed");
    }else{
        D_println("SD initialised");
    }

    char ssid[SIZE_SSID];
    char password[SIZE_PASSWORD];

    loadConfig(voicepack, SIZE_VOICEPACK, VOICEPACK, PATH_VOICEPACK);
    loadConfig(url, SIZE_URL, URL, PATH_URL);
    loadConfig(ssid, SIZE_SSID, WIFI_SSID, PATH_SSID);
    loadConfig(password, SIZE_PASSWORD, WIFI_PASSWORD, PATH_PASSWORD);

    D_print("Voicepack in use: "); D_println(voicepack);
    D_print("URL in use: "); D_println(url);
    D_print("WiFi SSID in use: "); D_println(ssid);
    D_print("WiFi password in use: "); D_println(password);

    WiFi.mode(WIFI_STA);
    WiFi.begin(ssid, password);

    while(WiFi.status() != WL_CONNECTED) delay(100);
    D_println("WiFi connected");

    // Turn off setup light
    digitalWrite(PIN_LED, LOW);
}

void loop(){
    if(digitalRead(PIN_BUTTON) == HIGH) return;

    digitalWrite(PIN_LED, HIGH);
    int size_generated = getNewMetarPhrase(phrase, SIZE_PHRASE);
    D_println("Generation complete!");
    //for(int i=0; i<size_generated; i++) playToken(phrase[i]);
    for(int i=0; i<117; i++) playToken(TokenType(i), voicepack);
    D_println("End of loop\n-----------------------\n");
    digitalWrite(PIN_LED, LOW);
}
