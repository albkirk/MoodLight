# MoodLight Project

This device aims to build a cozy, or inspirational, or emotive ambient surrounding you. For that, it uses a colorful 8x8 LED matrix to blast you with the mood.
It finds the right mood to transmit thanks to the information acquired by the sensors: temperature*, humidity*, light* and sound level*, detected BT* or WiFi* devices, weather forecast*, etc..
The set of colors, the visual pattern effect and brightness intensity are selected to create the perfect ambient.

(* planned integration)

## Supported Features

    1.  Single color or multi-color smooth transition,
    2.  "mood" profiles with customizable color pallete (Hot, Cool, Nature, Inspire,...),
    3.  Manual selection of "mood" profile,
    4.  Selects the right "mood" profile, based on ambient data reading,*
    5.  Auto-OFF Timer, configurable using Home Assistant, using the buttons* with the LED matrix as "Display",*
    6.  Light intensity adjust manually, or based on ambient light,*
    7.  Flashlight (Long press Mode Button),
    8.  Works everywhere, regardless of Wi-Fi newtork availability,
    9.  [Home Assistant](https://www.home-assistant.io) integration,
    10. [WLED](https://kno.wled.ge) compatible.
        (* Future integration)


## Used components

    - LilyGo T7 v1.3 ESP32 MCU,
    - WS2812b LED 5050 RGB 8x8 64 LED Matrix,
    - 18650 LiPo battery,
    - TP4056 battery charger module (embeded in the T7),
    - 1 Button (embeded in the T7),
    - AHT10 temperature and humidity sensor, *
    - Electret Microphone module, *
    - Light sensor resistor (LDR), * 
        (* Future integration)


## PinOut

    ESP32   |   WS2812b
    5v      |   V+
    GND     |   V-
    IO16    |   In 

## Technical Requirements

    - Define "Mood Profiles" with predifined name, color selection (may have diferent number of colors), effect pattern (Led to light-up, color to apply)

    - Use push buttons with single press, or n presses, or long press.

    - Use the matrix as output display, like timer definition, selected EFX, etc. Ex.: each led may represent 5 minutes of the ON timer, or flash green for "OK", or draw a red cross for "NOK" or "Cancel" message.

    - Battery powered to take it anywhere

    - Handy flash light (White color full brightness)


## Case
https://cad.onshape.com/documents/a11340a8b514efb50a936062/w/a1de64b26606eb292ac9daf7/e/f47711be4b7f09a631f87059

*STL Files*:
[ - Front -](Media/Mood-Front.stl)
[- Back -](Media/Mood-Back.stl)
[- Matrix -](Media/Mood-Matrix.stl)
[- Mode Button -](Media/Mood-Mode.stl)
[- PWR Switch -](Media/Mood-PWR.stl)

<img src="Media/Foto%20Display.jpg" alt="Foto Display" style="width: 50%;">
<img src="Media/Complete%20Wiring.jpg" alt="Complete Wiring" style="width: 50%;">
