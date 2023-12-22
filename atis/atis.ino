/**
 *  ATIS main program file.
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

#include "config.h"

std::vector<std::string> phrase;
int pos;

void playClip(const unsigned char* data, unsigned int length){
    AudioOutputI2SNoDAC* out = new AudioOutputI2SNoDAC();
    AudioGeneratorMP3* aud = new AudioGeneratorMP3();

    AudioFileSourcePROGMEM* clip = new AudioFileSourcePROGMEM(data, length);
    aud->begin(clip, out);
    while(aud->loop());
    aud->stop();
}

void say(std::string token){
    if(token == "A"){
        playClip(A_mp3, A_mp3_len);
        return;
    }
    if(token == "B"){
        playClip(B_mp3, B_mp3_len);
        return;
    }
    if(token == "C"){
        playClip(C_mp3, C_mp3_len);
        return;
    }
    if(token == "THIS_IS"){
        playClip(THIS_IS_mp3, THIS_IS_mp3_len);
        return;
    }
    if(token == "KUMPULA"){
        playClip(KUMPULA_mp3, KUMPULA_mp3_len);
        return;
    }
    if(token == "INFORMATION"){
        playClip(INFORMATION_mp3, INFORMATION_mp3_len);
        return;
    }
}

std::string getMetar(){
    std::unique_ptr<BearSSL::WiFiClientSecure> client(new BearSSL::WiFiClientSecure);
    client->setInsecure();
    HTTPClient https;

    https.begin(*client, URL);
    int responseCode = https.GET();

    if(responseCode <= 0){
        D_println("Error in HTTPS request");
        return "Error";
    }

    String response = https.getString();
    https.end();

    StaticJsonDocument<384> doc;
    DeserializationError error = deserializeJson(doc, response);
    if(error){
        D_print("Error deserialising JSON: ");
        D_println(error.f_str());
        return "Error";
    }

    std::string message = doc.as<JsonObject>().begin()->value()["p1"].as<std::string>();
    D_println(message.c_str());

    return message;
}

std::vector<std::string> parseMetar(std::string metar){
    std::vector<std::string> parsed;

    metar.pop_back();  // Removes final = sign
    
    // Split string on spaces
    size_t i = 0;
    while(i != std::string::npos){
        i = metar.find(" ");
        std::string token = metar.substr(0, i);
        parsed.push_back(token);
        metar.erase(0, i+1);
    }

    for(std::string x : parsed) D_println(x.c_str());

    return parsed;
}

std::vector<std::string> generatePhrase(){
    std::vector<std::string> result;

    int time = millis();
    result = {"THIS_IS", "KUMPULA", "INFORMATION", time % 3 == 0 ? "A" : (time % 3 == 1 ? "B" : "C")};

    return result;
}

void setup(){
    // Pin setup
    pinMode(LED, OUTPUT);
    digitalWrite(LED, HIGH);  // Turn on setup light
    D_SerialBegin(115200);

    // WiFi setup
    // The WiFiManager class is rather heavy.
    // These brackets create a scope for wifiManager: the object is destroyed after a connection is established.
    {
        WiFiManager wifiManager;
        wifiManager.autoConnect("ATIS");
    }

    // Data retrieval from ilmailusää service
    std::string metar = getMetar();
    std::vector<std::string> parsed = parseMetar(metar);

    // Audio setup
    phrase = generatePhrase();
    pos = 0;

    // Turn off setup light
    digitalWrite(LED, LOW);
}

void loop(){
    if(pos < phrase.size()){
        say(phrase[pos]);
        pos++;
    }else{
        phrase = generatePhrase();
        pos = 0;
        delay(1000);
    }
}
