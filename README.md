# ATIS

Automatic terminal information service - on NodeMCU!

## General info

There are two ways of listening to aviation weather reports.
The first is to buy an expensive aviation radio and tune in to the local airport's ATIS frequency.
The second is to install this app on your NodeMCU!

- Uses the NodeMCU v1.0, ESP8266-12E v2 Amica board.
    - There are multiple versions of the NodeMCU with confusing names.
    This board is the newest official one and has a standard breadboard size.
    See [this article](https://frightanic.com/iot/comparison-of-esp8266-nodemcu-development-boards/) for reference.
- Downloads data from the [Finnish aviation weather service](https://ilmailusaa.fi)
- Parses it using the [standard METAR report format](https://ilmailusaa.fi/pdf/Saahaitari_01-2021.pdf)

## Installation

1. Build the circuit as shown on the diagrams below
1. Install the [Arduino IDE](https://www.arduino.cc/en/main/software) and optionally set up a development environment in Visual Studio Code
1. Install the [NodeMCU Add-On](https://randomnerdtutorials.com/how-to-install-esp8266-board-arduino-ide/)
1. Run `./encode.sh` to encode the default voice pack
    - You can also specify a voice pack by running `./encode.sh <name>`
    - Currently available voice packs: `female`, `male`
1. Connect your NodeMCU to your computer via USB and upload the code

## Circuit

The circuit contains a NodeMCU, a speaker module, and an LED.
The speaker module is connected to +5V, and is wired to the RX port of the NodeMCU and GND via an NPN transistor.
The RX port is used by the `AudioOutputI2SNoDAC` library to output sound.

![Breadboard diagram](/meta/ATIS_bb.png)

![Schematic diagram](/meta/ATIS_shem.png)

## Credits

- Finnish meteorological institute
- Aeroplane manufacturers
- [How to exit vim](https://stackoverflow.com/questions/11828270/how-do-i-exit-the-vim-editor)
