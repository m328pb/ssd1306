#include <avr/pgmspace.h>
#include <string.h>
#include <stdio.h>

#include "ssd1306.h"
#include "i2c.h"
#include "uart.h"

/*
// Maximum simplified lib for text displaying on SSD1306 OLED display
// initializing the class will already initialize the OLED and set cursor to 0,0
// uer methods are:
// .clear() - clear display, also restore position to x,y
// .print(char *c) - print text
// .println(char *c) - print text and return to begining. NOT CHNAGING TO NEW LINE
// .x .y to to set cursor position
// .invert to "colors"
// isOLED is true if display is connected
//
// setup display pixels in min_SSD1306.h file
// select desired font in min_SSDI1306.h file, possible fonts are in font.h file
// adjust also CONTRAST in min_SSD1306.h file
*/

// select desired font in min_SSDI1306.h file
extern const FONT_TYPE FONT[][FONT_WIDTH];

oled::oled() : font(FONT), x(0), y(0), invert(false)
{
    initialize();
    if (!isOLED)
        initUSART();
};

void oled::init_char_buffer(void)
{
    memset(char_buffer, 0x0, CHAR_BYTES);
    index = 0;
    char_buffer[index++] = 0x40;
}

void oled::initialize()
{
    mw_init();
    // Init sequence
    static const uint8_t PROGMEM init[] = {
        0xAE, // display off
        0xD5, // Set display clock divide ratio/oscillator frequency
        0x80, // the suggested ratio 0x80

        0xA8, // Multiplex ratio
        HEIGHT - 1,

        0xD3, // Set display offset
        0x00, // no offset

        0x40, // Set display start line (0x40->0 ; 7F->63)

        0x8D, // Charge pump setting
        0x14, // 0x10 disable, 0x14 enable

        0x20, // Set memory addressing mode
        0x01, // horizontal addressing mode (0x01 vertical)

        0xA1, // Set segment re-map (column address 127 mapped to SEG0)

        0xC8, // Set COM output scan direction (com63 to COM0)

        0xDA, // Set COM pins hardware configuration
        0x02, // Alternative COM pin configuration (sequencial COM pin + left/right remap)

        0x81,     // Set contrast control
        CONTRAST, // Max contrast from 0 to 0xFF (half is 7D)

        0xD9, // Set pre-charge period
        0xF1, // Phase 1 and 2 period of 1 DCLK (15uS)

        0xDB, // Set VCOMH voltage level
        0x20, // 0x20 (0.77*Vcc) (0x40 in adafruit??)

        0xA4, // Set entire display on/off (0xA4 normal mode, 0xA5 entire display on)

        0xA6, // Set normal/inverse display (0xA6 normal, 0xA7 inverse)

        0x2E,  // Deactivate scroll
        0xAF}; // display on
    cmd(init, sizeof(init), true);
    clear();
}

void oled::setCursor()
{
    uint8_t init[] = {
        0x21,                      // set column address range
        (uint8_t)(x * FONT_WIDTH), // start column
        WIDTH - 1,                 // end column

        0x22,                                   // set page address range
        (uint8_t)(PAGES_PER_FONT * y),          // start page
        (uint8_t)(PAGES_PER_FONT * (y + 1) - 1) // end page
    };

    cmd(init, sizeof(init) / sizeof(init[0]));
};

void oled::clear()
{
    if (isOLED)
    {
        uint8_t store_x = x;
        uint8_t store_y = y;
        setPos(0, 0);

        for (int i = 0; i < ROWS; i++)
        {
            setPos(0, i);
            for (int j = 0; j <= COLS; j++)
                print(0x20); // 'space'
        }

        setPos(store_x, store_y);
    }
}

void oled::print(const char *str)
{
    uint8_t n = strlen(str);
    while (n--)
        print(*str++);
}

void oled::println(const char *str)
{
    print(str);
    print('\n');
}

void oled::print(const char c)
{
    if (isOLED)
    {
        if (!c)
            return;
        if (c == '\n') // coming from println
        {
            y++;
            setPos(x, y);
            return;
        }
        drawChar(c);
    }
    else
        printByte(c);
}

void oled::cmd(const uint8_t *cmds, uint8_t n, bool progmem)
{
    // send set of commands to the display
    // don't send more then TWI_BUFFER_SIZE (32) bytes at once
    // if (progmem) use pgmspace library (cmd set with PRAGMA)
    // otherway use regular memory

    uint8_t buffer[n + 1] = {0};
    // leave first element 0, means command mode: Co = 0, D/C = 0
    if (progmem)
    {
        for (int i = 0; i < n; i++)
            buffer[i + 1] = pgm_read_byte(cmds++);
    }
    else
        memcpy(&buffer[1], cmds, n);

    if (mw_write(buffer, n + 1))
        isOLED = true;
    else
        isOLED = false;
}

void oled::drawChar(const char c)
{
    init_char_buffer();
    FONT_TYPE line;

    for (int i = 0; i < FONT_WIDTH; i++)
    {
        line = PGMREAD(&font[c - 0x20][i]);
        if (invert)
            line = ~line;
        writeCol(line);
    }
    writeCol((FONT_TYPE)0x00); // space between chars
    mw_write(char_buffer, CHAR_BYTES);
}

void oled::setPos(uint8_t x, uint8_t y)
{
    if (y * PAGES_PER_FONT > PAGES)
        y = PAGES;
    if (x >= WIDTH / FONT_WIDTH)
        x = WIDTH / FONT_WIDTH - 1;

    this->x = x;
    this->y = y;
    setCursor();
}

void oled::writeCol(uint64_t col)
{

    // iterate over variable in 8bits chunks
    for (int i = 0; i < PAGES_PER_FONT; i++)
    {
        uint8_t c = col >> (8 * i);
        char_buffer[index++] = c;
    }
}
