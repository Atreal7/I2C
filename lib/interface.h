#ifndef INTERFACE
#define INTERFACE

#include "pico/stdlib.h"
#include <stdio.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#define SDA_PIN 16
#define SCL_PIN 17
#define OLED_ADDR 0x3C

// Height in frames (8px)
#define HEIGHT 8
#define WIDTH 128

// A row  8 pixels tall
// so example
// 0101010101010101
// Would become
//00
//11
//00
//11
//00
//11
//00
//11
// Uses set height and width cause im lazy to make this whole thing dynamic and that would kill memory usage
typedef struct row{
    uint8_t pixels[WIDTH];
}row;
typedef struct frame{
    row rows[HEIGHT];
}frame;

#define BAUDRATE 100000

void i2cInit();
int i2cSendPacket(uint8_t address,uint8_t *instructions, size_t num, bool Strict);

int SetPixel(int x, int y, frame *Screen, bool bit);

int displayFrame(uint8_t address,frame screen);

#endif
