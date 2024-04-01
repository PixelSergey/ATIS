#!/usr/bin/python
# ATIS encoding script. This script converts the audio files under
# the ./audio directory to C++ variables.
# Copyright (C) 2023-2024 PixelSergey
#
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <https://www.gnu.org/licenses/>.

import sys
import os
import math

# Constants
output_path = "./atis/audio.h"

audiofiles = ["ZERO", "ONE", "TWO", "THREE", "FOUR", "FIVE", "SIX", "SEVEN", "EIGHT", "NINER"]

tempate = """/**
 * ATIS audio data file. Automatically generated by `encode.py`.
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

#ifndef ATIS_AUDIO
#define ATIS_AUDIO

constexpr unsigned int audioDataLengths[] PROGMEM = {{
{lengths}
}};

constexpr unsigned int audioDataOffsets[] PROGMEM = {{
{offsets}
}};

constexpr unsigned char audioDataMp3[] PROGMEM = {{
{audio_data}
}};

#endif
"""


# Logic
if len(sys.argv) < 2:
    voicepack = "female"
else:
    voicepack = sys.argv[1]

if not os.path.exists(f"./audio/{voicepack}"):
    print(f"Voice pack {voicepack} does not exist")
    sys.exit()

print(f"Encoding {voicepack} voice pack...")

audiodata = []

for filename in audiofiles:
    path = f"./audio/{voicepack}/{filename}.mp3"
    if not os.path.exists(path):
        print(f"Missing {filename} audio file!")
        sys.exit()
    with open(path, "rb") as f:
        data = f.read()
        audiodata.append(data)

lengths = [len(data) for data in audiodata]
offsets = [sum(lengths[:i]) for i,_ in enumerate(lengths)]  # Could be optimised to O(n) but whatever
lengthstring = "\n".join([str(length)+"," for length in lengths])
offsetstring = "\n".join([str(offset)+"," for offset in offsets])

datadump = [hex(char)+"," for char in  b''.join(audiodata)]
datasplit = [''.join(datadump[32*i:32*(i+1)]) for i in range(math.ceil(len(datadump)/32))]
datastring = "\n".join(datasplit)

with open(output_path, "w") as f:
    f.write(tempate.format(
        lengths = lengthstring,
        offsets = offsetstring,
        audio_data = datastring,
    ))

print("Done!")
