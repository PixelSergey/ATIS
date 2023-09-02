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


void loadAudio() {
  vox["A"] = new AudioFileSourcePROGMEM(A_mp3, A_mp3_len);
  vox["B"] = new AudioFileSourcePROGMEM(B_mp3, B_mp3_len);
  vox["C"] = new AudioFileSourcePROGMEM(C_mp3, C_mp3_len);
  vox["THIS_IS"] = new AudioFileSourcePROGMEM(THIS_IS_mp3, THIS_IS_mp3_len);
  vox["KUMPULA"] = new AudioFileSourcePROGMEM(KUMPULA_mp3, KUMPULA_mp3_len);
  vox["INFORMATION"] = new AudioFileSourcePROGMEM(INFORMATION_mp3, INFORMATION_mp3_len);
}


void setup() {
  delay(1000);

  out = new AudioOutputI2SNoDAC();
  aud = new AudioGeneratorMP3();

  loadAudio();

  int time = millis();
  phrase = {"THIS_IS", "KUMPULA", "INFORMATION", time%3==0 ? "A" : (time%3==1 ? "B" : "C")};
  pos = 0;
}

void loop() {
  if (aud->isRunning()) {
    if (!aud->loop()) aud->stop();
  } else {
    if (pos<phrase.size()) {
      aud->begin(vox[phrase[pos]], out);
      pos++;
    } else {
      delay(1000);
    }
  }
}

