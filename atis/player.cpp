/**
 * ATIS audio player program file.
 * This file contains the logic to play audio data using a speaker connected to the RX port.
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

#include "player.h"

void playMp3(char* path){
    D_print("Playing ");
    AudioOutputI2SNoDAC* out = new AudioOutputI2SNoDAC();
    AudioGeneratorMP3* aud = new AudioGeneratorMP3();
    AudioFileSourceSD* clip = new AudioFileSourceSD(path);

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

void playToken(TokenType token){
    char path[20+sizeof(VOICEPACK)];
    sprintf(path, "/audio/" VOICEPACK "/%ld.mp3", token);
    if(!SD.exists(path)) return;

    playMp3(path);
}
