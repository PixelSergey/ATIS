/**
 * ATIS main program file.
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

#include "config.h"
#include "helper.h"

TokenType phrase[200];

/**
 * Plays a single MP3 audio clip from program memory.
 * Suitable data is generated by `encode.sh` and written to `audio.h`.
 * 
 * @param data A pointer to the audio data in MP3 format
 * @param length The length of the data array
 */
void playMp3(const unsigned char* data, unsigned int length){
    D_print("Playing ");
    AudioOutputI2SNoDAC* out = new AudioOutputI2SNoDAC();
    AudioGeneratorMP3* aud = new AudioGeneratorMP3();
    AudioFileSourcePROGMEM* clip = new AudioFileSourcePROGMEM(data, length);

    D_print("Looping ");
    aud->begin(clip, out);
    while(aud->loop());
    aud->stop();

    D_print("Deleting ");

    delete clip;
    delete aud;
    delete out;

    D_println("Exiting");
}

/**
 * Plays the sound file of an individual token of speech.
 * Token names are the same as the MP3 filenames, and are also listed in the `tokenToAudio` map in `helper.h`.
 * 
 * @param token The name of the token to play
 */
void playToken(TokenType token){
    if(token >= std::size(tokenToAudio)) return;

    const unsigned char* data = tokenToAudio[token].first;
    unsigned int length = tokenToAudio[token].second;
    playMp3(data, length);
}

/**
 * Download the METAR information from ilmailusaa.fi.
 * The precise URL is defined in `config.h`.
 * 
 * @return The raw JSON-formatted HTTP reponse body returned by ilmailusaa.fi 
 */
void getMetar(char* response, int size_response){
    std::unique_ptr<BearSSL::WiFiClientSecure> client(new BearSSL::WiFiClientSecure);
    client->setInsecure();
    HTTPClient https;
    https.setReuse(false);

    https.begin(*client, URL);
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

/**
 * Decode the raw METAR information obtained from `getMetar()`.
 * 
 * @param metar A string containing a JSON-formatted response from ilmailusaa.fi
 * @return A string containing the current METAR information
 */
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

/**
 * Splits the METAR information obtained from `decodeMetar()` into individual chunks containing one piece of information each.
 * 
 * @param metar A METAR string in standard format
 * @return A vector containing the METAR information split on spaces
 */
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

void pushWeather(TokenType* phrase, int size_phrase, int& pos, const char* weather){
    const int value = weather[0] | (weather[1] << 8);
    int len = sizeof(weatherType)/sizeof(int);
    int weather_index = std::find(weatherType, weatherType+len, value) - weatherType;
    if(weather_index >= len) return;
    PushToken(TokenType(VICINITY + weather_index))
}

void pushHeight(TokenType* phrase, int size_phrase, int& pos, const char* height){
    if(strncmp(height, "000", 3) == 0){
        PushToken(ZERO);
        PushToken(FEET);
        return;
    }

    if(strncmp(height, "0", 1) != 0){
        PushNumbers(phrase, 2);
        PushToken(THOUSAND);
    }else if(strncmp(height+1, "0", 1) != 0){
        PushNumbers(phrase+1, 1);
        PushToken(THOUSAND);
    }

    if(strncmp(height+2, "0", 1) != 0){
        PushNumbers(height+2, 1);
        PushToken(HUNDRED);
    }

    PushToken(FEET);
}

/**
 * Pushes a set of speech tokens to the end of a phrase depending on the METAR token type.
 * 
 * @param phrase A phrase vector, passed by reference
 * @param match The regex match object for the token
 * @param type The type of token to be converted into speech
*/
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
                PushToken(HEAVY);
            }
            if(Matched(2)){
                PushToken(LIGHT);
            }
            for(int i=3; i<=5; i++){
                if(Matched(i)) PushWeather(Match(i));
            }
            break;

        case I_CLOUD:
            if(Matched(1)) PushToken(FEW);
            else if(Matched(2)) PushToken(SCATTERED);
            else if(Matched(3)) PushToken(BROKEN);
            else if(Matched(4)) PushToken(OVERCAST);
            PushHeight(Match(5));
            if(Matched(6)) PushToken(CUMULONIMBUS);
            else if(Matched(7)) PushToken(TOWERING_CUMULUS);
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
            break;

        case I_QNH:
            break;

        case I_WINDSHEAR:
            break;

        case I_ALL:
            break;

        case I_RWY:
            break;

        case I_RUNWAY_NUMBER:
            break;

        default:
        case I_ERROR:
            break;
    }
    return pos;
}

/**
 * Transforms the split METAR information output by `parseMetar()` into a list of tokens to be played on the speaker based on the METAR standard.
 * The official METAR standard can be found at <https://ilmailusaa.fi/pdf/Saahaitari_01-2021.pdf>.
 * 
 * @param metar A vector of METAR information
 * @return A vector containing individual speech tokens
 */
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

/**
 * Combines the above functions to automatically download and convert current METAR information into a list of tokens.
 * 
 * @return A vector containing the speech tokens corresponding to current METAR information
 */
int getNewMetarPhrase(TokenType* phrase, int size_phrase){
    char response[350];
    char metar[150];
    char* parsed[25];
    getMetar(response, 350);
    int size_metar = decodeMetar(metar, 150, response, 350);
    int size_parsed = parseMetar(parsed, 25, metar, size_metar);
    int size_generated = generatePhrase(phrase, size_phrase, parsed, size_parsed);
    return size_generated;
}

/**
 * Sets up the NodeMCU's pins and WiFi connectivity.
 */
void setup(){
    // Pin setup
    pinMode(LED, OUTPUT);
    digitalWrite(LED, HIGH);  // Turn on setup light
    D_SerialBegin(115200);

    WiFi.mode(WIFI_STA);
    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

    while(WiFi.status() != WL_CONNECTED) delay(100);
    D_println("WiFi connected");

    // Turn off setup light
    digitalWrite(LED, LOW);
}

/**
 * Main program loop
 */
void loop(){
    int size_generated = getNewMetarPhrase(phrase, 200);
    D_println("Generation complete!");
    for(int i=0; i<size_generated; i++) playToken(phrase[i]);
    D_println("End of loop\n-----------------------\n");
    delay(2000);
}
