Sparkade
=====

Captive portal access point that serves a JavaScript game to clients.

Getting Started
---------------

 - In Arduino, File->Preferences and add `http://arduino.esp8266.com/stable/package_esp8266com_index.json`
 - Tools->Board->Boards Manager and install 2.0.0 of esp8266
 - Close Arduino
 - Open the ESP8266 boards.txt file (Windows: \\Users\{UserName}\AppData\Local\Arduino15\packages\esp8266\hardware\esp8266\2.0.0\boards.txt) and replace `thing.upload.resetmethod=ck` with `thing.upload.resetmethod=nodemcu`
 - Download SdFat (https://github.com/greiman/SdFat) and copy SdFat directory to (Documents)/Arduino/libraries
 - Open libraries/SdFat/SdFatConfig.h and change `#define SD_SPI_CONFIGURATION 0` to `#define SD_SPI_CONFIGURATION 1`
 - Open libraries/SdFat/SdFatUtil.cpp and change `#else  // __arm__` to `#else ifndef ESP8266 // __arm__`
 - Open libraries/SdFat/utility/StdioStream.cpp and change:
    - Add the following function just before the other functions:
    ```
    //------------------------------------------------------------------------------
    uint8_t *memchr2(uint8_t *ptr, uint8_t ch, size_t size)
    {
      for (int i = 0; i < size; i++)
        if (*ptr++ == ch)
          return ptr;
      return NULL;
    }
    ```
    - Change `memchr(m_p, '\n', n))` to `memchr2(m_p, '\n', n))`
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