#!/bin/bash

rm ../atis/audio.h

for f in *.mp3;
do
    xxd -i $f | sed "s/unsigned char \(.*\) = {/const unsigned char \1 PROGMEM = {/" - >> ../atis/audio.h
done
