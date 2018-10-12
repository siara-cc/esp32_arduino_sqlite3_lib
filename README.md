# Sqlite3 Arduino library for ESP32

This library enables access to SQLite database files from SPIFFS or SD Cards through ESP32 SoC.  Given below is a picture of a board that has a ready-made Micro SD slot (using SDMMC 4 bit mode - see example sqlite3_sdmmc):

![](ESP_WROOM_32_breakout.png?raw=true)

Also shown below is the wiring between ESP-WROOM-32 breakout board and Micro SD Shield (using SPI mode - see example sqlite3_sdspi):

![](ESP32_MSD_Shield_Wiring.jpg?raw=true)

## Usage

Sqlite3 C API such as `sqlite3_open` can be directly invoked. Before calling please invoke:

```c++
   SD_MMC.begin(); // for Cards attached to the High speed 4-bit port 
   SPI.begin(); SD.begin(); // for Cards attached to the SPI bus
   SPIFFS.begin(); // For SPIFFS
```
as appropriate.

The ESP32 Arduino library has an excellent VFS layer.  Even multiple cards can be supported on the SPI bus by specifying the pin number and mount point using the `begin()` method.

The default mount points are:
```c++
   '/sdcard' // for SD_MMC 
   '/sd' // for SD on SPI
   '/spiffs' // For SPIFFS
```

and the filenames are to be prefixed with these paths in the `sqlite3_open()` function (such as `sqlite3_open("/spiffs/my.db")`).

Please see the examples for full illustration of usage for the different file systems. The sample databases given (under data folder) need to be copied to the Micro SD card root folder before the SD example can be used.  Please see the comments section of the example.

## Wiring

While there is no wiring needed for SPIFFS, for attaching cards to SPI bus, please use the following connections:

```c++
 * SD Card    |  ESP32
 *  DAT2 (1)      -
 *  DAT3 (2)      SS (D5)
 *  CMD  (3)      MOSI (D23)
 *  VDD  (4)      3.3V
 *  CLK  (5)      SCK (D19)
 *  VSS  (6)      GND
 *  DAT0 (7)      MISO (D18)
 *  DAT1 (8)      -
```

And for SD card attached to High-speed 4-bit SD_MMC port, use:

```c++
 * SD Card    |  ESP32
 *  DAT2 (1)      D12
 *  DAT3 (2)      D13
 *  CMD  (3)      D15
 *  VDD  (4)      3.3V
 *  CLK  (5)      D14
 *  VSS  (6)      GND
 *  DAT0 (7)      D2
 *  DAT1 (8)      D4
```

If you are using a board such as shown in the picture above, this wiring is ready-made.

## Installation

Please download this library, unzip it to the libraries folder of your ESP32 sdk location. The location varies according to your OS.  For example, it is usually found in the following locations:
```
Windows: C:\Users\(username)\AppData\Roaming\Arduino15
Linux: /home/<username>/.arduino15
MacOS: /home/<username>/Library/Arduino15
```
Under Arduino15 folder please navigate to `packages/esp32/hardware/esp32/<version>/libraries`

If you do not have the ESP32 sdk for Arduino, please see https://github.com/espressif/arduino-esp32 for installing it.

## Dependencies / pre-requisites

No dependencies except for the Arduino SDK. The Sqlite3 code is included with the library.

## Limitations on ESP32

* No serious limitations, except its a bit slow on large datasets. It takes around 700 ms to retrieve from a dataset containing 10 million rows, even using the index.

## Limitations of this library

* Locking is not implemented.  So it cannot be reliably used in a multi-threaded / multi-core code set, except for read-only operations.
* As of now many features of Sqlite3 have been omitted, except for basic table and index operations.  These are expected to be made available shortly.
* The SPI bus wiring is not yet tested.

## Limitations of Flash memory

Any Flash memory such as those available on SPIFFS or Micro SD cards have limitation on number of writes / erase per sector.  Usually the limitation is 10000 writes or 100000 writes (on the same sector).  Although ESP32 supports wear-levelling,  this is to be kept in mind before venturing into write-intensive database projects.  There is no limitation on reading from Flash.

## Acknowledgements
* This library was developed based on NodeMCU module developed by [Luiz Felipe Silva](https://github.com/luizfeliperj). The documentation can be found [here](https://nodemcu.readthedocs.io/en/master/en/modules/sqlite3/).
* The census2000 and baby names databases were taken from here: http://2016.padjo.org/tutorials/sqlite-data-starterpacks/. But no license information is available.
* The mdr512.db (Million Domain Rank database) was created with data from [The Majestic Million](https://majestic.com/reports/majestic-million) and is provided under CC 3.0 Attribution license.
* The [ESP32 core for Arduino](https://github.com/espressif/arduino-esp32)
* [The Arduino platform](https://arduino.cc)

## Screenshot (output of Micro SD example)
![](output_screenshot.png?raw=true)

## Issues
Please contact the author (Arundale Ramanathan) at arun@siara.cc if you find any problem (or create issue here).
