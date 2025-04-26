# ESP32 Project "MoodLight"


This device aims to build a cozy, or inspirational, or emotive ambient surrounding you. For that, it uses a colorful 8x8 LED matrix to blast you with the mood.
It finds the right mood to transmit thanks to the information acquired by the sensors: temperature, humidity, light and sound level, detected BT or WiFi devices, weather forecast, etc..
The set of colors, the visual pattern effect and brightness intensity are selected to create the perfect ambient.

Used components: 
	* ESP32 LilyGo T7 v1.3 SuperMini MCU,
	* WS2812 LED 5050 RGB 8x8 64 LED Matrix,
	* 18650 LiPo Battery
	* TP4056 battery charger module (embeded in the T7)
	* 1 Button (embeded in the T7)
	* BME 280 temperature and humidity sensor*
	* Electret Microphone module*
	* Light sensor resistor (LDR)* 
		(* Future integration)

## Supported Features:
	1. Device should run locally, even when LAN (WiFi) newtork is down
	2. Auto-select the right "mood" profile, based on sensor's reading
	3. Manual selection of "mood" profile.
	4. Auto-OFF Timer, configurable Home Assistant (using the buttons*) and the LED matrix as "output"
	5. Light intensity adjust based on ambient lighting

## Technical Requirements
	- Define "Mood Profiles" with predifined name, color selection (may have diferent number of colors), effect pattern (Led to light-up, color to apply)
	- Use 2 push buttons with single press, double press, and long press.
	- Use the 8 x 8 matrix as output display, like for timer definition. Each led shall represent 1 minute. Flash green for "OK", red cross for "NOK" or "Cancel".