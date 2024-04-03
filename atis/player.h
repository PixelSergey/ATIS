/**
 * ATIS audio player header file.
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

#ifndef ATIS_PLAYER
#define ATIS_PLAYER

#include "AudioFileSourceSD.h"
#include "AudioGeneratorMP3.h"
#include "AudioOutputI2SNoDAC.h"
#undef stack

#include "helper.h"

/**
 * @brief Plays a single MP3 audio clip from program memory.
 * 
 * @param[in] path A pointer to a char array containing the path of the audio file to play
 */
void playMp3(char* path);

/**
 * @brief Plays the sound file of an individual token of speech.
 * All available token names can be found in the TokenType enum in `helper.h`.
 * 
 * @param[in] token The token to play
 * @param[in] voicepack A pointer to a character array with the voicepack name
 */
void playToken(TokenType token, char* voicepack);

#endif