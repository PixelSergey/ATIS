#include <Arduino.h>
#include <string>
#include <vector>
#include <map>

#include "AudioFileSourcePROGMEM.h"
#include "AudioGeneratorWAV.h"
#include "AudioOutputI2SNoDAC.h"

#include "audio.h"

AudioGeneratorWAV *wav;
AudioOutputI2SNoDAC *out;
std::map<std::string, AudioFileSourcePROGMEM*> vox;

std::vector<std::string> phrase;
int pos;


void setup() {
  delay(1000);

  vox["A"] = new AudioFileSourcePROGMEM(A_wav, A_wav_len);
  vox["B"] = new AudioFileSourcePROGMEM(B_wav, B_wav_len);
  out = new AudioOutputI2SNoDAC();
  wav = new AudioGeneratorWAV();

  phrase = {"A", "B"};
  pos = 0;
}

void loop() {
  if (wav->isRunning()) {
    if (!wav->loop()) wav->stop();
  } else {
    if (pos<phrase.size()) {
      delay(200);
      wav->begin(vox[phrase[pos]], out);
      pos++;
    } else {
      delay(1000);
    }
  }
}

