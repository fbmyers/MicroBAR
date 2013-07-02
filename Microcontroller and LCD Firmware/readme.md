Microcontroller and LCD Firmware
========


Firmware is based on the Arduino 1.0.4 Platform.  The "controller.ino" is the main sketch.  The MicroBAR hardware is based on the Arduino Mega 2560 platform.  The LCD is the Amulet MK-480272C color touchscreen LCD module, which itself features a built-in graphics environment that is programmed with modified HTML/CSS/Javascript.  The Amulet GEMcompiler 1.1.2 was used to compile the HTML files and download them onto the LCD module.

The Atmega 2560 chip must first be programmed with the Arduino Bootloader and the Atmega 8U2 must be programmed with the DFU USB firmware (both using the ICSP headers on the board and an AVR-ISP module).  Then the system will behave as an Arduino and the firmware sketches may be uploaded via USB.  A customized DFU firmware (which identifies itself as "Berkeley MicroBAR 2", rather than "Arduino Mega 2560", is available here.  The bootloader can be installed via the Arduino IDE.

Note that not all hardware features are currently implemented in firmware.