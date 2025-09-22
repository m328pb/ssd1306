/*
 Used to measure memory usage: env:mem_usage in platformio.ini
 then do avr-size on firmware.elf
 */
#include "ssd1306.h"

int main() {
  oled display;

  display.clear();
  display.setPos(1, 1);
  display.println("test ssd1306");
  display.invert = true;
}
