#include <Arduino.h>
#include <string>
#include <vector>
#include <map>

#include "AudioFileSourcePROGMEM.h"
#include "AudioGeneratorMP3.h"
#include "AudioOutputI2SNoDAC.h"

#include "audio.h"

AudioGeneratorMP3 *aud;
AudioOutputI2SNoDAC *out;
std::map<std::string, AudioFileSourcePROGMEM*> vox;

std::vector<std::string> phrase;
int pos;


void setup() {
  delay(1000);

  vox["A"] = new AudioFileSourcePROGMEM(A_mp3, A_mp3_len);
  vox["B"] = new AudioFileSourcePROGMEM(B_mp3, B_mp3_len);
  out = new AudioOutputI2SNoDAC();
  aud = new AudioGeneratorMP3();

  phrase = {"A", "B"};
  pos = 0;
}

void loop() {
  if (aud->isRunning()) {
    if (!aud->loop()) aud->stop();
  } else {
    if (pos<phrase.size()) {
      delay(200);
      aud->begin(vox[phrase[pos]], out);
      pos++;
    } else {
      delay(1000);
    }
  }
}

