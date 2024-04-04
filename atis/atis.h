/**
 * ATIS main program header.
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

#ifndef ATIS_MAIN
#define ATIS_MAIN

#include <Arduino.h>
#include <SPI.h>
#include <SD.h>

#include "config.h"
#include "helper.h"

#include "player.h"
#include "networking.h"
#include "parser.h"

/**
 * @brief Downloads the current METAR information, processes it, and converts it into an array of tokens.
 * 
 * @param[out] phrase A TokenType array, where the speech tokens corresponding to current METAR information will be written
 * @param[in] size_phrase The maximum size of `phrase`
 * @return The number of tokens written
 */
int getNewMetarPhrase(TokenType* phrase, int size_phrase);

/**
 * @brief Loads a config file to a location in memory
 * 
 * @param[out] target A pointer to a character array where the data will be written
 * @param[in] size_target The size of `target`
 * @param[in] defaultText A pointer to a character array with the data to write if the SD card is inaccessible
 * @param[in] filepath A pointer to a character array with the path to the file
*/
void loadConfig(char* target, int size_target, const char* defaultText, const char* filepath);

/**
 * @brief Sets up the NodeMCU's pins and WiFi connectivity.
 */
void setup();

/**
 * @brief Main program loop
 */
void loop();

#endif