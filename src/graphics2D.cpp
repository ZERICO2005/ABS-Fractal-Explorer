/*
**	Author: zerico2005 (2023)
**	Project: ABS-Fractal-Explorer
**	License: MIT License
**	A copy of the MIT License should be included with
**	this project. If not, see https://opensource.org/license/MIT
*/

#include "gamma.h"
#include "graphics2D.h"
#include "graphics2D_Font.h"

colorRGB gColor;
uint8_t* VRAM;

uint32_t resX;
uint32_t resY;
uint32_t resZ;

#define plotColor(z)  VRAM[z * 3] = gColor.r; VRAM[z * 3 + 1] = gColor.g; VRAM[z * 3 + 2] = gColor.b;

/* Core Functions */

void setBuffer(uint8_t* buffer, uint32_t ResX, uint32_t ResY) {  // Initializer
	resX = ResX;
	resY = ResY;
	resZ = ResX * ResY;
	VRAM = buffer;
}

void swapBuffer(uint8_t* buffer0, uint8_t* buffer1) { // Uses buffer0 if neither buffer is in use
	if (VRAM == buffer0) {
		VRAM = buffer1;
	} else {
		VRAM = buffer0;
	}
}

uint8_t* getBuffer() {
	return VRAM;
}

void setRGB(uint8_t r, uint8_t g, uint8_t b) {
	gColor.r = r;
	gColor.g = g;
	gColor.b = b;
}
void setHexColor(uint32_t col) {
	gColor.b = col & 0xFF; col >>= 8;
	gColor.g = col & 0xFF; col >>= 8;
	gColor.r = col & 0xFF;
}
void setColor(colorRGB col) {
	gColor.r = col.r;
	gColor.g = col.g;
	gColor.b = col.b;
}

/* Core Functions */

/* Primatives */

void plot(uint32_t x, uint32_t y) { //x position, y position
	if (x < resX && y < resY) {
		uint32_t fill = (x + (y * resX));
		plotColor(fill);
	}
}
void plotFast(uint32_t z) {
	if (z < resZ) {
		plotColor(z);
	}
}
void fillScreen() { //x start, y start, x length, y length
    for (uint32_t v = 0; v < resX * resY; v++) {
		plotColor(v);
    }
}

void fillRect(uint32_t x0, uint32_t y0, uint32_t x1, uint32_t y1) { //x start, y start, x length, y length
    uint32_t v = (x0 + (y0 * resX));
    const uint32_t jump = resX - x1;
    for (uint32_t dY = 0; dY < y1; dY++) {
        for (uint32_t dX = 0; dX < x1; dX++) {
			if (v < resZ) { plotColor(v); }
            v++;
        }
        v += jump;
    }
}
void horiz(uint32_t x0, uint32_t y, uint32_t x1) { //x start, y postion, x length
    uint32_t z = ((x0 + x1) + (y * resX));
    for (uint32_t fill = x0 + (y * resX); fill < z; fill++) {
        plotColor(fill);
    }
}
void vert(uint32_t x, uint32_t y0, uint32_t y1) { //x postion, y start, y length
    uint32_t z = (x + ((y1 + y0) * resX));
    for (uint32_t fill = (y0 * resX) + x; fill < z; fill += resX) {
        plotColor(fill);
    }
}

void drawLine0(i32 x0, i32 y0, i32 x1, i32 y1) {
	i32 dX = x1 - x0;
	i32 dY = y1 - y0;
	i32 yI = 1;
	if (dY < 0) {
		yI = -1;
		dY = -dY;
	}
	i32 dD = 2 * dY - dX;
	i32 y = y0;
	for (i32 x = x0; x < x1; x++) {
		plot(x,y);
		if (dD > 0) {
			y += yI;
			dD += 2 * (dY - dX);
		} else {	
			dD += 2 * dY;
		}
	}
}

void drawLine1(i32 x0, i32 y0, i32 x1, i32 y1) {
	i32 dX = x1 - x0;
	i32 dY = y1 - y0;
    i32 xI = 1;
    if (dX < 0) {
		xI = -1;
		dX = -dX;
	}
    i32 dD = (2 * dX) - dY;
    i32 x = x0;

    for (i32 y = y0; y < y1; y++) {
        plot(x, y);
        if (dD > 0) {
            x = x + xI;
            dD = dD + (2 * (dX - dY));
        } else {
            dD = dD + 2 * dX;
        }
	}
}

void drawLine(i32 x0, i32 y0, i32 x1, i32 y1) {
    if (abs(y1 - y0) < abs(x1 - x0)) {
        if (x0 > x1) {
            drawLine0(x1, y1, x0, y0);
		} else {
            drawLine0(x0, y0, x1, y1);
		}
    } else {
        if (y0 > y1) {
            drawLine1(x1, y1, x0, y0);
        } else {
            drawLine1(x0, y0, x1, y1);
		}
    }
}

/* Primatives */

/* Geometric */

void drawRect(uint32_t x0, uint32_t y0, uint32_t x1, uint32_t y1) {
	horiz(x0,y0,x1);
	horiz(x0,y0+y1-1,x1);
	vert(x0,y0,y1);
	vert(x0+x1-1,y0,y1);
}

/* Geometric */

/* Text */

/* const uint8_t char6x8[1536] can be found in graphics2D_Font.h */

void text6x8(uint32_t xW, uint32_t yW, uint8_t lexicon) { //x position, y position, letter index
    uint8_t* bitImage = (uint8_t*)char6x8 + (lexicon * 6);
    uint32_t fill = (yW * resX + xW);
    uint8_t b = 1;
    for (uint8_t y = 0; y < 8; y++) {
        for (uint8_t x = 0; x < 6; x++) {
			if (*bitImage & b) {
				plotColor(fill);
			}
            bitImage++;
            fill++;
        }
        bitImage -= 6;
        fill += resX - 6; // 320 - 6
        b <<= 1;
    }
}

void printText(uint32_t xW, uint32_t yW, char* text) { // X, Y, Text
	uint32_t x = xW;
	uint32_t y = yW;
	uint32_t t = 0; // For tabs
	while (*text != '\0') {
		switch(*text) {
			case '\n': // New Line
				x = xW;
				y += 9;
				t = 0;
				break;
			case '\t': // Horizontal Tab
				x += (4 - ((t - 1) % 4)) * 7;
				t = 0;
				break;
			case '\v': // Vertical Tab
				y += 9;
				break;
			case '\b': // Backspace
				x -= 7;
				break;
			case '\r': // Return
				x = xW;
				break;
			default: //Text
				text6x8(x,y,*text);
				x += 7;
		}
		text++;
		t++;
	}
}
/* Unfinished Code 2023/08/10 19:53
void printFloat(uint32_t xC, uint32_t yC, fp64 input, uint8_t len, uint8_t dec) {
	if (len == 0) {
		xC += 5;
	} else {
		uint32_t integerPart = (uint32_t)floor(abs(in));
		printUInt(xC,yC,integerPart,len);
	}
	uint32_t decimalPart = (uint32_t)((abs(in) - floor(abs(in))) * pow(10.0,(fp64)dec));
	printUInt(decimalPart,dec,base,xC + (len * 4 + 2),yC);
	for (uint8_t i = 0; i < 7; i++) {
		plotPixel((xC + (len * 4 + 1)) + (primeRESX * (yC + (i))),0x00);
	}
	plotPixel((xC + (len * 4 + 1)) + (primeRESX * (yC + (5))),0xFF);
	if (in < 0.0) {
		if (len == 0) {
			xC -= 5;
		}
		for (uint32_t y = 0; y < 7; y++) {
			for (uint32_t x = 0; x < 5; x++) {
				plotPixel((xC + x) + (primeRESX * (yC + (y))),0x00);
			}
		}
		plotPixel((xC + 1) + (primeRESX * (yC + (3))),0xFF);
		plotPixel((xC + 2) + (primeRESX * (yC + (3))),0xFF);
		plotPixel((xC + 3) + (primeRESX * (yC + (3))),0xFF);
	}
}
*/
/* Text */

/* Other */

void shadeRect(uint32_t x0, uint32_t y0, uint32_t x1, uint32_t y1) {
	uint32_t v = (x0 + (y0 * resX));
    const uint32_t jump = resX - x1;
    for (uint32_t dY = 0; dY < y1; dY++) {
        for (uint32_t dX = 0; dX < x1; dX++) {
			if ((dX + dY) & 1) {
				plotColor(v);
			}
            v++;
        }
        v += jump;
    }
}

/* Other */