/**
 * ATIS parser header file.
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

#ifndef ATIS_PARSER
#define ATIS_PARSER

#include <regex>

#include "helper.h"

/**
 * @brief Pushes a single token onto the phrase array, ensuring that it is not written outside the array's bounds.
 * 
 * @param[out] phrase A TokenType array to write to
 * @param[in] size_phrase The maximum size of `phrase`
 * @param[out] pos Current position, passed by reference. This will be updated
 * @param[in] token The token to push
 */
void pushToken(TokenType* phrase, int size_phrase, int& pos, TokenType token);

/**
 * @brief Pushes a string of numbers (0-9) onto the phrase array
 * 
 * @param[out] phrase A TokenType array to write to
 * @param[in] size_phrase The maximum size of `phrase`
 * @param[out] pos Current position, passed by reference. This will be updated
 * @param[in] numbers A pointer to a char array of ASCII numbers
 * @param[in] count The number of elements to push
 */
void pushNumbers(TokenType* phrase, int size_phrase, int& pos, const char* numbers, int count);

/**
 * @brief Pushes a string of uppercase characters (A-Z) onto the phrase array
 * 
 * @param[out] phrase A TokenType array to write to
 * @param[in] size_phrase The maximum size of `phrase`
 * @param[out] pos Current position, passed by reference. This will be updated
 * @param[in] chars A pointer to a char array of uppercase ASCII characters
 * @param[in] count The number of elements to push
 */
void pushChars(TokenType* phrase, int size_phrase, int& pos, const char* chars, int count);

/**
 * @brief Pushes a four-digit distance value in meters to the phrase array
 * 
 * @param[out] phrase A TokenType array to write to
 * @param[in] size_phrase The maximum size of `phrase`
 * @param[out] pos Current position, passed by reference. This will be updated
 * @param[in] distance A pointer to a char array representing a distance in metres. The array must consist of 4 ASCII digits (0-9) 
 */
void pushDistance(TokenType* phrase, int size_phrase, int& pos, const char* distance);

/**
 * @brief Pushes a string of weather symbols onto the phrase array
 * 
 * @param[out] phrase A TokenType array to write to
 * @param[in] size_phrase The maximum size of `phrase`
 * @param[out] pos Current position, passed by reference. This will be updated
 * @param[in] weather A pointer to a char array containing weather symbols
 * @param[in] length The length of `weather`. Must be even
 */
void pushWeather(TokenType* phrase, int size_phrase, int& pos, const char* weather, int length);

/**
 * @brief Pushes a three-digit height value in feet to the phrase array
 * 
 * @param[out] phrase A TokenType array to write to
 * @param[in] size_phrase The maximum size of `phrase`
 * @param[out] pos Current position, passed by reference. This will be updated
 * @param[in] height A pointer to a char array representing a height in hundreds of feet. The array must consist of 3 ASCII digits (0-9)
 */
void pushHeight(TokenType* phrase, int size_phrase, int& pos, const char* height);

/**
 * @brief Pushes a set of speech tokens to the end of the `phrase` array depending on the information type and regex match
 * 
 * @param[out] phrase A TokenType array to write to
 * @param[in] size_phrase The maximum size of `phrase`
 * @param[out] pos Current position in the TokenType array
 * @param[in] match Regex match object, passed by reference
 * @param[in] type The type of METAR information to process
 * @return The number of tokens written to `phrase`
 */
int convertToken(TokenType* phrase, int size_phrase, int pos, std::cmatch& match, InformationType type);

/**
 * @brief Gets the current information letter based on the time of the METAR information
 * 
 * @param[in] time The 4-character time value from the METAR information
 * @return The TokenType value for the current information letter
*/
TokenType getInformationLetter(const char* time);

/**
 * @brief Transforms the split METAR information output by `parseMetar()` into a list of tokens to be played on the speaker based on the METAR standard.
 * The official METAR standard can be found at https://ilmailusaa.fi/pdf/Saahaitari_01-2021.pdf.
 * 
 * @param[out] phrase A TokenType array to write to
 * @param[in] size_phrase The maximum size of `phrase`
 * @param[in] metar A pointer to an array of character pointers, containing the pointers to each piece of METAR information
 * @param[in] size_metar The size of the `metar` array
 * @return The number of tokens written to `phrase`
 */
int generatePhrase(TokenType* phrase, int size_phrase, char** metar, int size_metar);

#endif
