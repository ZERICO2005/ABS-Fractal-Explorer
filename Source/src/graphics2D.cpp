#include "gamma.h"
#include "graphics2D.h"

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
const uint8_t char6x8[1536] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,6,223,223,6,0,7,3,0,0,7,3,228,60,39,228,60,39,36,110,202,83,118,36,66,37,18,72,164,66,118,255,153,127,230,176,0,0,7,3,0,0,0,126,195,129,129,0,0,129,129,195,126,0,17,10,4,4,10,17,48,48,252,252,48,48,0,0,160,96,0,0,16,16,16,16,16,16,0,0,192,192,0,0,224,240,24,12,7,3,126,255,145,137,255,126,0,132,254,255,128,0,198,231,177,153,207,198,98,235,137,137,255,118,48,44,35,255,255,32,111,239,137,137,251,115,126,255,137,137,251,114,3,3,225,249,31,7,118,255,137,137,255,118,78,223,145,145,255,126,0,0,102,102,0,0,0,0,86,54,0,0,8,8,20,20,34,34,20,20,20,20,20,20,34,34,20,20,8,8,6,7,209,217,15,6,126,129,181,189,161,30,240,252,39,39,252,240,255,255,137,137,255,118,126,255,129,129,230,103,255,255,129,129,255,126,255,255,137,137,235,227,255,255,9,9,11,3,126,255,129,145,118,247,255,255,8,8,255,255,0,129,255,255,129,0,96,224,128,128,255,127,255,255,24,60,231,195,255,255,128,128,224,224,255,14,28,28,14,255,255,14,28,56,112,255,126,255,129,129,255,126,255,255,17,17,31,14,30,63,97,225,191,158,255,255,49,113,223,142,102,239,137,137,251,114,3,1,255,255,1,3,127,255,128,128,255,127,15,63,224,224,63,15,255,112,56,56,112,255,227,247,28,28,247,227,3,15,248,248,15,3,227,243,153,141,231,227,0,255,129,129,129,0,3,7,12,24,240,224,0,129,129,129,255,0,4,6,3,3,6,4,128,128,128,128,128,128,0,1,3,2,0,0,96,244,148,148,124,248,255,127,132,132,252,120,120,252,132,132,204,72,120,252,132,132,127,255,120,252,148,148,220,88,16,254,255,17,19,2,24,188,164,164,248,124,255,255,8,8,248,240,0,136,251,251,128,0,128,136,251,123,0,0,255,255,16,56,236,196,0,129,255,255,128,0,252,248,4,248,4,248,252,248,4,4,252,248,120,252,132,132,252,120,252,248,36,36,60,24,24,60,36,36,248,252,252,248,4,4,28,24,72,220,148,164,236,72,0,4,126,255,132,64,124,252,128,128,124,252,28,124,192,192,124,28,60,124,128,124,128,124,196,236,56,56,236,196,140,188,224,96,60,12,204,236,164,148,220,204,8,8,126,247,129,129,0,0,255,255,0,0,129,129,247,126,8,8,16,8,24,24,16,8,63,32,32,168,112,32};

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