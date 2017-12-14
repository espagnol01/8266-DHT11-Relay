# Readme

## General Information

The following sketch connects your ESP8266 to your wifi network and then it connects and subscribe to your MQTT server. After it is connected, it gets you information from a DHT11 temperature/humidity sensor every 1 minute and you can control a simple relay connected to the board. 
This sketch has been tested on a NodeMCU with the DHT11 connected to pin D5 and the relay connected to pin D6. Everything is specified inside the code.

## Recognition

This code has been merge using to different pieces of code. The temperature sensor reader was written by RobPope and the relay function was written by Dimitar Manovski. I only modified some small things and merge everything together. So all the thanks to them to make this possible.
