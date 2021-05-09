## Introduction

The LilyGo T5 4.7 is an e-ink display combined with an esp32 chip, which allows the display to be updated with data fetched over wifi/bluetooth. The examples from LilyGo's [getting started repository](https://github.com/Xinyuan-LilyGO/LilyGo-EPD47) cover the basics in terms of installation and displaying local data: text via built-in functions (with an extra font header) or an image converted to a .h file via their [imgconvert](https://github.com/Xinyuan-LilyGO/LilyGo-EPD47/blob/master/scripts/imgconvert.py) tool.

I created this repository to serve images over the network to the LilyGo to be able to draw images on a server/desktop before sending them over to the device. Two main reasons for this approach:

1. Flexibility of displaying data, no need to re-flash the ESP32 chip
2. Complex drawings are easier to generate on the desktop than on the LilyGo/ESP32

## Installation

For hardware you'll need the LilyGo T5 4.7 of course. I got mine from [AliExpress](https://nl.aliexpress.com/item/1005002006058892.html).

For the software you'll need:

- Arduino IDE (https://www.arduino.cc/en/software). For setup instructions see the official repository: https://github.com/Xinyuan-LilyGO/LilyGo-EPD47
- Python (with pip package manager)
    - You'll need to install the following python libraries afterwards:

```
pip install numpy opencv-python flask
```


## Demo

![Demo](demo.gif)

## Architecture

You'll find two folders in this repository:

```
├───lilygo              - Contains the ESP32 code which needs to be flashed on the device
└───server              - Contains the code to run on the server
```

## Internals

The E-Ink display has the following characteristics:
- pixel data should be in a ```uint8_t*``` buffer
- each pixel is 4 bits long (so two pixels per byte)
  - 4 bits -> 16 grayscale tones supported
  - Byte order is BIG endian -> most significant byte first
    - 0x0 -> Black
    - 0xF -> White

The webserver (```server/imageserver.py```) converts any given image to the format explained previously using a combination of ```numpy```, ```opencv```. It also uses the ```flask``` library as a webserver.


## Licence

You're free to re-use this code however you want (commercially or otherwise), attribution would be nice, but is not required by any means.
