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

AudioGeneratorMP3 *aud;
AudioOutputI2SNoDAC *out;
std::map<std::string, AudioFileSourcePROGMEM*> vox;

std::vector<std::string> phrase;
int pos;

void loadAudio(){
    vox["A"] = new AudioFileSourcePROGMEM(A_mp3, A_mp3_len);
    vox["B"] = new AudioFileSourcePROGMEM(B_mp3, B_mp3_len);
    vox["C"] = new AudioFileSourcePROGMEM(C_mp3, C_mp3_len);
    vox["THIS_IS"] = new AudioFileSourcePROGMEM(THIS_IS_mp3, THIS_IS_mp3_len);
    vox["KUMPULA"] = new AudioFileSourcePROGMEM(KUMPULA_mp3, KUMPULA_mp3_len);
    vox["INFORMATION"] = new AudioFileSourcePROGMEM(INFORMATION_mp3, INFORMATION_mp3_len);
}

void setup(){
    // Pin setup

    D_SerialBegin(115200);

    pinMode(LED, OUTPUT);
    digitalWrite(LED, HIGH);

    // WiFi setup

    // The WiFiManager class is rather heavy. This creates a scope for the variable,
    // which is destroyed when it properly sets up the WiFi connection
    {
        WiFiManager wifiManager;
        wifiManager.autoConnect("ATIS");
    }

    // Data retrieval from ilmailusää service

    std::unique_ptr<BearSSL::WiFiClientSecure> client(new BearSSL::WiFiClientSecure);
    client->setInsecure();
    HTTPClient https;

    https.begin(*client, URL);
    int responseCode = https.GET();

    if(responseCode <= 0){
        D_println("Error in HTTPS request");
        return;
    }

    String response = https.getString();
    https.end();

    StaticJsonDocument<384> doc;
    DeserializationError error = deserializeJson(doc, response);
    if(error){
        D_print("Error deserialising JSON: ");
        D_println(error.f_str());
        return;
    }

    String message = doc.as<JsonObject>().begin()->value()["p1"].as<String>();
    D_println(message);

    digitalWrite(LED, LOW);

    // Audio setup

    out = new AudioOutputI2SNoDAC();
    aud = new AudioGeneratorMP3();

    loadAudio();

    // Phrase setup - will be replaced by automated report

    int time = millis();
    phrase = {"THIS_IS", "KUMPULA", "INFORMATION", time % 3 == 0 ? "A" : (time % 3 == 1 ? "B" : "C")};
    pos = 0;
}

void loop(){
    if(aud->isRunning()){
        if(!aud->loop()) aud->stop();
    }else{
        if(pos < phrase.size()){
            aud->begin(vox[phrase[pos]], out);
            pos++;
        }else{
            digitalWrite(LED, LOW);
            delay(1000);
        }
    }
}
