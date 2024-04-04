/**
 * ATIS networking header file.
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

#ifndef ATIS_NETWORKING
#define ATIS_NETWORKING

#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClientSecureBearSSL.h>

#include "config.h"
#include "helper.h"

/**
 * @brief Downloads the METAR information from ilmailusaa.fi.
 * 
 * @param[out] response A pointer to a char array, where the raw JSON-formatted data from ilmailusaa.fi will be written
 * @param[in] size_response The maximum size of the `response` array
 * @param[in] url A pointer to a char array where the URL is located
 */
void getMetar(char* response, int size_response, const char* url);

/**
 * @brief Decodes the raw METAR information obtained from `getMetar()`.
 *
 * @param[out] metar A pointer to a char array, where the current decoded METAR information will be written
 * @param[in] size_metar The maximum size of the `metar` array
 * @param[in] raw A pointer to a char array with raw JSON-formatted data from ilmailusaa.fi
 * @param[in] size_raw The size of the `raw` array
 * @return The number of characters written to `metar`
 */
int decodeMetar(char* metar, int size_metar, char* raw, int size_raw);

/**
 * @brief Splits the METAR information obtained from `decodeMetar()` into individual chunks containing one piece of information each.
 * 
 * @param[out] parsed A pointer to an array of character pointers, where the pointers to each piece of METAR information will be written 
 * @param[in] size_parsed The maximum size of the `parsed` array
 * @param[in] metar A pointer to a char array containing the current decoded METAR information
 * @param[in] size_metar The size of the `metar` array
 * @return The number of characters written to `parsed`
 */
int parseMetar(char** parsed, int size_parsed, char* metar, int size_metar);

#endif