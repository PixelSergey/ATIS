#!/bin/bash

if [ -z "$1" ]
then
    DIR="female"
else
    DIR=$1
fi

if [ ! -d "./audio/$DIR" ]
then
    echo "Voice pack \"$DIR\" does not exist"
    exit
fi

rm ./atis/audio.h
cd ./audio/$DIR

for f in *.mp3;
do
    xxd -i $f | sed "s/unsigned char \(.*\) = {/const unsigned char \1 PROGMEM = {/" - >> ../../atis/audio.h
done
