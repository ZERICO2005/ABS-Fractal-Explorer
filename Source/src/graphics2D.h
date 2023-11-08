#ifndef GRAPHICS2D_H
#define GRAPHICS2D_H

#include "gamma.h"

struct _colorRGB {
	uint8_t r;
	uint8_t g;
	uint8_t b;
};
typedef struct _colorRGB colorRGB;

// Core Functions
void setBuffer(uint8_t* buffer, uint32_t ResX, uint32_t ResY); // Initializer
void swapBuffer(uint8_t* buffer0, uint8_t* buffer1); // Uses buffer0 if neither buffer is in use
uint8_t* getBuffer();
void setRGB(uint8_t r, uint8_t g, uint8_t b);
void setHexColor(uint32_t col);
void setColor(colorRGB col);

//Primatives

void plot(uint32_t x, uint32_t y); //x position, y position
void plotFast(uint32_t z); //z position
void fillScreen();

void fillRect(uint32_t x0, uint32_t y0, uint32_t x1, uint32_t y1); //x start, y start, x length, y length
void horiz(uint32_t x0, uint32_t y, uint32_t x1); //x start, y postion, x length
void vert(uint32_t x, uint32_t y0, uint32_t y1); //x postion, y start, y length

void drawLine(i32 x0, i32 y0, i32 x1, i32 y1); //xy start, xy end

// Geometric

void drawRect(uint32_t x0, uint32_t y0, uint32_t x1, uint32_t y1);

//Other
void printText(uint32_t xW, uint32_t yW, char* text);
void text6x8(uint32_t xW, uint32_t yW, uint8_t lexicon);
void shadeRect(uint32_t x0, uint32_t y0, uint32_t x1, uint32_t y1);

#endif /* GRAPHICS2D_H */