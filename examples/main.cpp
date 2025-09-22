#include "ssd1306.h"

int main() {
  oled display;
  display.clear();
  display.setPos(1, 1);
  display.println("test ssd1306");
  display.invert = true;
  display.setPos(0,0);
  display.print("invert");
}
