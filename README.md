# LCD on SSD1306

Library to display text on LCD with ssd1306 chip, for ATmega328pb/p.
Assumption is to minimaze memory footprint with only text display.
The only fancy feature is 'invert' method.

## usage

Just set display size and choose font in `ssd1306.h`

## notes

- tested on I2C 32x128 display, and there is no plan to use other protocols.

- included folder with differnt size fonts,
from [](https://github.com/lynniemagoo/oled-font-pack)

- fonts are memory thirsty, for example, very small fonts (5x8 pixels)
require 5x95 = 475B. Average size 6x16 would take 6x2x95=1.1kB.

- maximum size fonts of 19x32 pixels take 19x8x95=9kB,
which is one third of FLASH memory.

- fonts fit into FLASH memory with PROGMEM using pgmspace.h library.
Just remember about 10k writes limit of AVR MCU flash memory...just saying.

- basically took Wire library and left only necessery function
(no graphic, single font, no buffer, no scrolling)

- set page hight to fit font height, then write column by column:
each font character height is then page height and then column by column
written directly to display memory

- Adafruit_SSD1306 library takes 21254 bytes, when ripped off
Wire lib takes 10014 bytes.
