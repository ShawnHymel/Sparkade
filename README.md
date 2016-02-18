Sparkade
=====

Captive portal access point that serves a JavaScript game to clients.

Installation
------------

 - In Arduino, File->Preferences and add `http://arduino.esp8266.com/stable/package_esp8266com_index.json`
 - Tools->Board->Boards Manager and install 2.0.0 of esp8266
 - Close Arduino
 - Open the ESP8266 boards.txt file (Windows: \\Users\{UserName}\AppData\Local\Arduino15\packages\esp8266\hardware\esp8266\2.0.0\boards.txt) and replace `thing.upload.resetmethod=ck` with `thing.upload.resetmethod=nodemcu`
 - Open Arduino and load SparkadeServer.ino
 - Select "SparkFun Thing" from Boards and COM port
 - Upload to the ESP8266 Thing
 - Copy contents from one of the SD_xxx directories to the root directory of an SD card
 - Insert SD card and cycle power
 - Connect to "Sparkade" SSID
 - Browse to 10.10.10.1 (or any non-HTTPS site e.g. sparkade.com)

Repository Contents
-------------------

* **/SparkadeServer** - Server firmware for the SparkFun ESP8266 Thing
* **/SD_xxx** - Copy the contents of any one of these to an SD card

Version History
---------------

* v0.1 - Serves up a simple Breakout game. Simple logging.

License Information
-------------------

This product is _**open source**_! 

Please review the LICENSE.md file for license information. 

Distributed as-is; no warranty is given.