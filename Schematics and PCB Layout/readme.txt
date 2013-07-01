Schematic and PCB layout was done in Altium Designer Release 10.

The MicroBAR consists of two PCBs which are stacked on top of one another.  A single header connects these two boards.  "LAMP Microcontroller PCB" contains the microcontroller and all digital electronics, and it sits on the bottom.  "LAMP Chip PCB" contains the phototransistors, amplifier, MUXes, LED driver, and heater driver.  It sits on top.

Board Level Modifications (to be rolled into next revision):
1) A jumper wire was added between 3 and 6 on U4
2) A jumper wire was added between pin 17 on the expansion header and pin 10 on the B2B header
3) Only 3 LEDs were used along the top side of the detection region (the original plans call for 3 on top and 3 on bottom).

There are a number of problems with the SD card wiring:
4) A jumper wire was added between ICSP1 pin 3 and R1 (SD_SCK)
5) A jumper wire was added between ICSP1 pin 4 and R2 (SD_MOSI)
6) A jumper wire was added between ICSP1 pin 1 and U1 pin 25 (SD_MSIO)
7) Pins 1-3 on the SD slot header were wired incorrectly.  To fix this, the CS line going to pin 2 was jumpered to pin 1 and the original trace going to pin 2 was cut.  The MOSI line going to pin 3 was jumpered to pin 2 and the original trace going to pin 3 was cut.  And pin 3 was jumpered to ground.