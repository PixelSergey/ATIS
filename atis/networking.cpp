/**
 * ATIS networking program file.
 * This file contains the logic to download, decode, and split the METAR information.
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

#include "networking.h"

void getMetar(char* response, int size_response, const char* url){
    std::unique_ptr<BearSSL::WiFiClientSecure> client(new BearSSL::WiFiClientSecure);
    client->setInsecure();
    HTTPClient https;
    https.setReuse(false);

    https.begin(*client, url);
    int responseCode = https.GET();

    if(responseCode <= 0){
        D_println("Error in HTTPS request");
        strncpy(response, "ERROR", size_response);
        response[size_response-1] = '\0';
        return;
    }

    strncpy(response, https.getString().c_str(), size_response);
    response[size_response-1] = '\0';
    https.end();
    client.reset();

    D_println("Response:");
    D_println(response);
}

int decodeMetar(char* metar, int size_metar, char* raw, int size_raw){
    char* begin = strstr(raw, "\"p1\":\"");
    if(begin == NULL){
        strncpy(metar, "ERROR", size_metar);
        metar[size_metar-1] = '\0';
        return min(5, size_metar);
    }
    begin += 6;

    char* end = strstr(begin, "\"");
    if(end == NULL || size_metar < end-begin){
        strncpy(metar, "ERROR", size_metar);
        metar[size_metar-1] = '\0';
        return min(5, size_metar);
    }

    int copied = end-begin-1;
    strncpy(metar, begin, copied);
    metar[copied] = '\0';
    D_print("Decoded: ");
    D_println(metar);
    return copied+1;
}

int parseMetar(char** parsed, int size_parsed, char* metar, int size_metar){
    // Split string on spaces
    parsed[0] = metar;
    int j = 1;
    for(int i=0; i<size_metar; i++){
        if(metar[i] == ' '){
            metar[i] = '\0';
            parsed[j] = metar + i + 1;
            j++;
        }
    }

    D_print("Parsed: ");
    for(int i=0; i<j; i++){
        D_print(parsed[i]);
        D_print("|");
    }
    D_println();
    return j;
}
