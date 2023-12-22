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
#include "helper.h"

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

    std::string response = std::string(https.getString().c_str());
    https.end();

    D_println("Response:");
    D_println(response.c_str());

    return response;
}

std::string decodeMetar(std::string metar){
    StaticJsonDocument<384> jsonDocument;
    DeserializationError error = deserializeJson(jsonDocument, metar);
    if(error){
        D_print("Error deserialising JSON: ");
        D_println(error.f_str());
        return "Error";
    }

    std::string decoded = jsonDocument.as<JsonObject>().begin()->value()["p1"].as<std::string>();
    D_println("Decoded:");
    D_println(decoded.c_str());

    return decoded;
}

std::vector<std::string> parseMetar(std::string metar){
    std::vector<std::string> parsed;

    // Remove terminating equals sign
    metar.pop_back();
    
    // Split string on spaces
    size_t i = 0;
    while(i != std::string::npos){
        i = metar.find(" ");
        std::string token = metar.substr(0, i);
        parsed.push_back(token);
        metar.erase(0, i+1);
    }

    D_println("Parsed:");
    for(std::string x : parsed) D_println(x.c_str());

    return parsed;
}

std::vector<std::string> generatePhrase(std::vector<std::string> metar){
    std::vector<std::string> phrase;

    phrase.push_back("THIS_IS");
    phrase.push_back("KUMPULA");
    phrase.push_back("INFORMATION");

    int time = millis();
    phrase.push_back(time % 3 == 0 ? "A" : (time % 3 == 1 ? "B" : "C"));

    D_println("Phrase:");
    for(std::string x : phrase) D_println(x.c_str());

    return phrase;
}

std::vector<std::string> getNewMetarPhrase(){
    std::string metar = getMetar();
    std::string decoded = decodeMetar(metar);
    std::vector<std::string> parsed = parseMetar(decoded);
    std::vector<std::string> phrase = generatePhrase(parsed);
    return phrase;
}

void setup(){
    // Pin setup
    pinMode(LED, OUTPUT);
    digitalWrite(LED, HIGH);  // Turn on setup light
    D_SerialBegin(115200);

    // WiFi setup
    // The WiFiManager class is rather heavy.
    // These curly brackets create a scope for wifiManager,
    // so that the object is destroyed after a connection is established.
    {
        WiFiManager wifiManager;
        wifiManager.autoConnect("ATIS");
    }

    // Turn off setup light
    digitalWrite(LED, LOW);
}

void loop(){
    std::vector<std::string> phrase = getNewMetarPhrase();
    for(int i=0; i<phrase.size(); i++) say(phrase[i]);
    
    delay(2000);
}
