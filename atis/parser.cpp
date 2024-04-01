/**
 * ATIS parser program file.
 * This file contains the logic to parse the downloaded METAR information and convert it to an array of speech tokens.
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

#include "parser.h"

void pushToken(TokenType* phrase, int size_phrase, int& pos, TokenType token){
    if(pos >= size_phrase){
        pos = size_phrase;
        return;
    }

    D_print("Pushing "); D_println(token);
    phrase[pos] = token;
    pos++;
}

void pushNumbers(TokenType* phrase, int size_phrase, int& pos, const char* numbers, int count){
    for(int i=0; i<count; i++) pushToken(phrase, size_phrase, pos, TokenType(numbers[i]-'0'+ZERO));
}

void pushChars(TokenType* phrase, int size_phrase, int& pos, const char* chars, int count){
    for(int i=0; i<count; i++) pushToken(phrase, size_phrase, pos, TokenType(chars[i]-'A'+ALPHA));
}

void pushDistance(TokenType* phrase, int size_phrase, int& pos, const char* distance){
    if(strncmp(distance, "9999", 4) == 0){
        PushToken(ONE);
        PushToken(ZERO);
        PushToken(KILOMETERS);
        return;
    }
    if(strncmp(distance, "0000", 4) == 0){
        PushToken(ZERO);
        PushToken(METERS);
        return;
    }
    if(strncmp(distance+1, "000", 3) == 0){
        pushNumbers(phrase, size_phrase, pos, distance, 1);
        PushToken(KILOMETERS);
        return;
    }
    if(strncmp(distance+2, "00", 2) == 0){
        if(strncmp(distance, "0", 1) != 0){
            pushNumbers(phrase, size_phrase, pos, distance, 1);
            PushToken(THOUSAND);
        }
        pushNumbers(phrase, size_phrase, pos, distance+1, 1);
        PushToken(HUNDRED);
        PushToken(METERS);
        return;
    }

    pushNumbers(phrase, size_phrase, pos, distance, 4);
    PushToken(METERS);
}

void pushWeather(TokenType* phrase, int size_phrase, int& pos, const char* weather, int length){
    int len = sizeof(weatherType)/sizeof(int);
    for(int i=0; 2*i<length; i++){
        const int value = weather[2*i] | (weather[2*i+1] << 8);
        int weather_index = std::find(weatherType, weatherType+len, value) - weatherType;
        if(weather_index >= len) continue;
        PushToken(TokenType(VICINITY + weather_index))
    }
}

void pushHeight(TokenType* phrase, int size_phrase, int& pos, const char* height){
    if(strncmp(height, "000", 3) == 0){
        PushToken(ZERO);
        PushToken(FEET);
        return;
    }

    if(strncmp(height, "0", 1) != 0){
        PushNumbers(height, 2);
        PushToken(THOUSAND);
    }else if(strncmp(height+1, "0", 1) != 0){
        PushNumbers(height+1, 1);
        PushToken(THOUSAND);
    }

    if(strncmp(height+2, "0", 1) != 0){
        PushNumbers(height+2, 1);
        PushToken(HUNDRED);
    }

    PushToken(FEET);
}

int convertToken(TokenType* phrase, int size_phrase, int pos, std::cmatch& match, InformationType type){
    D_println("Converting");
    switch(type){
        case I_STATION:
            PushToken(THIS_IS);
            if(Matched(1)){
                PushToken(HYVINKAA);
            }else if(Matched(2)){
                PushToken(KUMPULA);
            }else if(Matched(3)){
                PushChars(Match(3), 4);
            }
            PushToken(INFORMATION);
            PushToken(ALPHA);
            break;

        case I_TIME:
            PushToken(AT); PushToken(TIME);
            PushNumbers(Match(1), 4);
            PushToken(ZULU);
            break;

        case I_NIL:
            PushToken(NO_WEATHER_INFORMATION);
            break;

        case I_AUTO:
            PushToken(AUTOMATIC_WEATHER_REPORT);
            break;

        case I_WIND:
            PushToken(WIND);
            if(Matched(1)){
                PushToken(UNKNOWN);
                break;
            }
            if(Matched(2)){
                PushToken(CALM);
                break;
            }
            if(Matched(3)) PushToken(VARIABLE);
            if(Matched(4)){
                PushNumbers(Match(4), 3);
                PushToken(DEGREES);
            }
            if(Matched(5)){
                PushNumbers(Match(5), 2);
                PushToken(KNOTS);
            }
            if(Matched(6)){
                PushToken(GUSTING);
                PushNumbers(Match(6), 2);
                PushToken(KNOTS);
            }
            break;

        case I_VARIABLE:
            PushToken(VARIABLE);
            PushToken(BETWEEN);
            PushNumbers(Match(1), 3);
            PushToken(AND);
            PushNumbers(Match(2), 3);
            PushToken(DEGREES);
            break;

        case I_CAVOK:
            PushToken(CAVOK);
            break;

        case I_VISIBILITY:
            PushToken(VISIBILITY);
            if(Matched(1)){
                PushToken(UNKNOWN);
                break;
            }
            if(Matched(2)){
                PushDistance(Match(2));
            }
            break;

        case I_RVR:
            PushToken(RUNWAY);
            PushNumbers(Match(1), 2);
            PushToken(VISIBLE_RANGE);
            if(Matched(2)){
                PushToken(LESS_THAN);
            }else if(Matched(3)){
                PushToken(MORE_THAN);
            }
            PushDistance(Match(4));
            if(Matched(5)){
                PushToken(AND);
                PushToken(INCREASING);
            }else if(Matched(6)){
                PushToken(AND);
                PushToken(DECREASING);
            }else if(Matched(7)){
                PushToken(AND);
                PushToken(REMAINING);
            }
            break;

        case I_WEATHER:
            if(Matched(1)){
                PushToken(WEATHER);
                PushToken(UNKNOWN);
                break;
            }
            if(Matched(2)) PushToken(HEAVY);
            if(Matched(3)) PushToken(LIGHT);
            if(Matched(4)) PushWeather(Match(4), Matched(4));
            break;

        case I_CLOUD:
            if(Matched(1)){
                if(!Matched(7) && !Matched(8)){
                    PushToken(CLOUDS);
                    PushToken(UNKNOWN);
                    break;
                }
                if(Matched(7)) PushToken(CUMULONIMBUS);
                if(Matched(8)) PushToken(TOWERING_CUMULUS);
                PushToken(CLOUDS);
                break;
            }
            if(Matched(2)) PushToken(FEW);
            if(Matched(3)) PushToken(SCATTERED);
            if(Matched(4)) PushToken(BROKEN);
            if(Matched(5)) PushToken(OVERCAST);
            if(Matched(6)) PushHeight(Match(6));
            if(Matched(7)) PushToken(CUMULONIMBUS);
            if(Matched(8)) PushToken(TOWERING_CUMULUS);
            break;

        case I_NSC:
            PushToken(NO_SIGNIFICANT_CLOUD);
            break;

        case I_NCD:
            PushToken(NO_CLOUD_DETECTED);
            break;

        case I_VERTICAL:
            PushToken(VERTICAL);
            PushToken(VISIBILITY);
            PushHeight(Match(1));
            break;

        case I_TEMPERATURE:
            PushToken(TEMPERATURE);
            if(Matched(1)) PushToken(MINUS);
            if(Matched(2)) PushNumbers(Match(2), 2);
            if(Matched(3)) PushToken(UNKNOWN);
            PushToken(DEWPOINT);
            if(Matched(4)) PushToken(MINUS);
            if(Matched(5)) PushNumbers(Match(5), 2);
            if(Matched(6)) PushToken(UNKNOWN);
            break;

        case I_QNH:
            PushToken(QNH);
            if(Matched(1)) PushNumbers(Match(1), 4);
            if(Matched(2)) PushToken(UNKNOWN);
            break;

        case I_WINDSHEAR:
            PushToken(WINDSHEAR);
            break;

        case I_ALL:
            PushToken(ALL);
            break;

        case I_RWY:
            PushToken(RUNWAY);
            break;

        case I_RUNWAY_NUMBER:
            PushToken(RUNWAY);
            PushNumbers(Match(1), 2);
            break;

        default:
        case I_ERROR:
            PushToken(ERROR);
            break;
    }
    return pos;
}

int generatePhrase(TokenType* phrase, int size_phrase, char** metar, int size_metar){
    int pos = 0;
    for(int i=0; i<size_metar; i++){
        InformationType type = I_ERROR;
        std::cmatch match;
        D_print("Searching match for "); D_println(metar[i]);
        for(std::pair<const char*, InformationType> it : regexToToken){
            bool found = std::regex_match(metar[i], match, std::regex(it.first));
            if(!found) continue;

            D_print("Found match for "); D_print(metar[i]); D_print(" with type "); D_println(it.second);
            type = it.second;
            break;
        }

        pos = convertToken(phrase, size_phrase, pos, match, type);
    }

    D_print("Phrase: ");
    for(int i=0; i<pos; i++){
        D_print(phrase[i]);
        D_print(" ");
    }
    D_println();
    return pos;
}
