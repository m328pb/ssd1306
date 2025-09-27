#pragma once
#include <avr/pgmspace.h>
#include "i2c.h"
#include "strings.h"

// chose the font you want to use from font folder
// only one font possible!!
#include "fonts/font6x16.h"
// #include "fonts/font19x32.h"

#define SSD1306_I2C_ADDRESS 0x3C
#define WIDTH 128 // screen width
#define HEIGHT 32 // screen height
#define CONTRAST 0x7D

#define PAGE_HEIGHT 8
#define PAGES ((HEIGHT / PAGE_HEIGHT) - 1)
#define PAGES_PER_FONT (FONT_HEIGHT / PAGE_HEIGHT)
#define CHAR_BYTES ((FONT_WIDTH + 1) * PAGES_PER_FONT + 1)
#define ROWS (FONT_HEIGHT / PAGE_HEIGHT)
#define COLS (WIDTH / (FONT_WIDTH + 1))

#if FONT_HEIGHT <= 8
#define FONT_TYPE uint8_t
#define PGMREAD pgm_read_byte
#elif FONT_HEIGHT <= 16
#define FONT_TYPE uint16_t
#define PGMREAD pgm_read_word
#elif FONT_HEIGHT <= 64
#define FONT_TYPE uint64_t
#define PGMREAD pgm_read_dword
#endif

class OLED{

private:
  I2C comm; // possibly you can use other protocol

  const FONT_TYPE (*font)[FONT_WIDTH];
  uint8_t char_buffer[CHAR_BYTES];
  unsigned int index = 0; // column of the char
  uint8_t x;
  uint8_t y;
  I2C::error err;

  void cmd(const uint8_t *command, uint8_t size, bool progmem = false);
  void initialize(void);
  bool check_status(void);
  void setCursor();
  void drawChar(char c);
  void writeCol(uint64_t col);
  void init_char_buffer(void);

public:
  bool invert;
  I2C &get() { return comm; }; // adjust returned type to used protocol
  void clear();
  void setPos(uint8_t x, uint8_t y);
  void println(const char *c);
  void print(const char c);
  void print(const char *c);
  OLED();
};
