/*
**	Author: zerico2005 (2023)
**	Project: ABS-Fractal-Explorer
**	License: MIT License
**	A copy of the MIT License should be included with
**	this project. If not, see https://opensource.org/license/MIT
*/

#include "Common_Def.h"
#include "copyBuffer.h"
#include "bitGraphics.h"

/*
**	Binary Font Data (ASCII) 
**	6x8 pixel font "even" by Christian Munk (CMunk)
**	See README.txt for more details
*/
const uint8_t char6x8[1536] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,6,223,223,6,0,7,3,0,0,7,3,228,60,39,228,60,39,36,110,202,83,118,36,66,37,18,72,164,66,118,255,153,127,230,176,0,0,7,3,0,0,0,126,195,129,129,0,0,129,129,195,126,0,17,10,4,4,10,17,48,48,252,252,48,48,0,0,160,96,0,0,16,16,16,16,16,16,0,0,192,192,0,0,224,240,24,12,7,3,126,255,145,137,255,126,0,132,254,255,128,0,198,231,177,153,207,198,98,235,137,137,255,118,48,44,35,255,255,32,111,239,137,137,251,115,126,255,137,137,251,114,3,3,225,249,31,7,118,255,137,137,255,118,78,223,145,145,255,126,0,0,102,102,0,0,0,0,86,54,0,0,8,8,20,20,34,34,20,20,20,20,20,20,34,34,20,20,8,8,6,7,209,217,15,6,126,129,181,189,161,30,240,252,39,39,252,240,255,255,137,137,255,118,126,255,129,129,230,103,255,255,129,129,255,126,255,255,137,137,235,227,255,255,9,9,11,3,126,255,129,145,118,247,255,255,8,8,255,255,0,129,255,255,129,0,96,224,128,128,255,127,255,255,24,60,231,195,255,255,128,128,224,224,255,14,28,28,14,255,255,14,28,56,112,255,126,255,129,129,255,126,255,255,17,17,31,14,30,63,97,225,191,158,255,255,49,113,223,142,102,239,137,137,251,114,3,1,255,255,1,3,127,255,128,128,255,127,15,63,224,224,63,15,255,112,56,56,112,255,227,247,28,28,247,227,3,15,248,248,15,3,227,243,153,141,231,227,0,255,129,129,129,0,3,7,12,24,240,224,0,129,129,129,255,0,4,6,3,3,6,4,128,128,128,128,128,128,0,1,3,2,0,0,96,244,148,148,124,248,255,127,132,132,252,120,120,252,132,132,204,72,120,252,132,132,127,255,120,252,148,148,220,88,16,254,255,17,19,2,24,188,164,164,248,124,255,255,8,8,248,240,0,136,251,251,128,0,128,136,251,123,0,0,255,255,16,56,236,196,0,129,255,255,128,0,252,248,4,248,4,248,252,248,4,4,252,248,120,252,132,132,252,120,252,248,36,36,60,24,24,60,36,36,248,252,252,248,4,4,28,24,72,220,148,164,236,72,0,4,126,255,132,64,124,252,128,128,124,252,28,124,192,192,124,28,60,124,128,124,128,124,196,236,56,56,236,196,140,188,224,96,60,12,204,236,164,148,220,204,8,8,126,247,129,129,0,0,255,255,0,0,129,129,247,126,8,8,16,8,24,24,16,8,63,32,32,168,112,32};

Bit_Graphics::Bit_Graphics() {
	terminate_Bit_Graphics();
}
Bit_Graphics::Bit_Graphics(size_t resX, size_t resY) {
	initialized = false;
	init_Bit_Graphics(resX,resY);
}
bool Bit_Graphics::isInitialized() { return initialized; }
void Bit_Graphics::init_Bit_Graphics(size_t resX, size_t resY) {
	if (initialized == true) { return; }
	if (resX == 0 || resY == 0) { return; }
	ResX = resX;
	ResY = resY;
	ResZ = resX * resY;
	buf0 = (uint8_t*)malloc(ResZ * 3);
	if (buf0 == NULL) { terminate_Bit_Graphics(); return; }
	buf1 = (uint8_t*)malloc(ResZ * 3);
	if (buf1 == NULL) { terminate_Bit_Graphics(); return; }
	buf = buf1;
	gColor_RGB(0,0,0);
	initialized = true;
	buf = buf0;
	clearBuffer();
	swapBuffer();
	clearBuffer();
}
void Bit_Graphics::terminate_Bit_Graphics() {
	initialized = false;
	buf = NULL;
	FREE(buf0);
	FREE(buf1);
	ResX = 0;
	ResY = 0;
	ResZ = 0;
	gColor_RGB(0,0,0);
}
void Bit_Graphics::resizeBuffer(size_t resX, size_t resY) {
	if (initialized == false) { return; }
	if (resX == ResX && resY == ResY) { return; } // No changes made
	if (resX == 0 || resY == 0) { return; }
	ResX = resX;
	ResY = resY;
	if (ResX * ResY != ResZ) { // Different amount of bytes allocated
		ResZ = ResX * ResY;
		buf0 = (uint8_t*)realloc(buf0,ResZ * 3);
		if (buf0 == NULL) { terminate_Bit_Graphics(); return; }
		buf1 = (uint8_t*)realloc(buf1,ResZ * 3);
		if (buf1 == NULL) { terminate_Bit_Graphics(); return; }
	}
	buf = buf0;
	clearBuffer();
	swapBuffer();
	clearBuffer();
}
void Bit_Graphics::swapBuffer() {
	if (initialized == false) { return; }
	buf = (buf == buf0) ? buf1 : buf0;
}
void Bit_Graphics::clearBuffer() {
	for (size_t i = 0; i < ResZ * 3; i++) {
		buf[i] = 0x00;
	}
}
uint8_t* Bit_Graphics::getDrawBuffer() {
	if (initialized == false) { return NULL; }
	return (buf == buf0) ? buf0 : buf1;
}
uint8_t* Bit_Graphics::getDisplayBuffer() {
	if (initialized == false) { return NULL; }
	return (buf == buf0) ? buf1 : buf0;
}
void Bit_Graphics::getDrawBufferBox(BufferBox* box) {
	if (box == NULL) { return; }
	if (initialized == false) {
		box->vram = NULL;
		box->resX = 0;
		box->resY = 0;
		box->channels = 0;
		box->padding = 0;
	}
	box->vram = (buf == buf0) ? buf0 : buf1;
	box->resX = ResX;
	box->resY = ResY;
	box->channels = 3;
	box->padding = 0;
}
void Bit_Graphics::getDisplayBufferBox(BufferBox* box) {
	if (box == NULL) { return; }
	if (initialized == false) {
		box->vram = NULL;
		box->resX = 0;
		box->resY = 0;
		box->channels = 0;
		box->padding = 0;
	}
	box->vram = (buf == buf0) ? buf1 : buf0;
	box->resX = ResX;
	box->resY = ResY;
	box->channels = 3;
	box->padding = 0;
}
size_t Bit_Graphics::getResX() { return ResX; }
size_t Bit_Graphics::getResY() { return ResY; }
size_t Bit_Graphics::getResZ() { return ResZ; }
size_t Bit_Graphics::getBufferSize() { return ResZ * 3; }
/* Set color */
void Bit_Graphics::gColor_RGB(uint8_t r, uint8_t g, uint8_t b) {
	gColor[0] = r;
	gColor[1] = g;
	gColor[2] = b;
}
void Bit_Graphics::gColor_Hex(uint32_t col) {
	gColor[2] = col & 0xFF; col >>= 8;
	gColor[1] = col & 0xFF; col >>= 8;
	gColor[0] = col & 0xFF;
}
void Bit_Graphics::gColor_HSV(fp64 h, fp64 s, fp64 v) { // 0.0-360.0, 0.0-1.0, 0.0-1.0

}
void Bit_Graphics::gColor_HSV(fp32 h, fp32 s, fp32 v) { // 0.0-360.0, 0.0-1.0, 0.0-1.0

}

/* Internal Routines | No safety checks */
void Bit_Graphics::internal_plot(size_t x, size_t y) {
	size_t z = (y * ResX + x) * 3;
	buf[z] = gColor[0]; z++;
	buf[z] = gColor[1]; z++;
	buf[z] = gColor[2];
}
void Bit_Graphics::internal_plotFast(size_t z) { //z position
	z *= 3;
	buf[z] = gColor[0]; z++;
	buf[z] = gColor[1]; z++;
	buf[z] = gColor[2];
}

/* Primatives */
void Bit_Graphics::plot(size_t x, size_t y) { //x position, y position
	if (x > ResX || y > ResY) { return; }
	size_t z = (y * ResX + x) * 3;
	buf[z] = gColor[0]; z++;
	buf[z] = gColor[1]; z++;
	buf[z] = gColor[2];
}
void Bit_Graphics::plotFast(size_t z) { //z position
	if (z > ResZ) { return; }
	z *= 3;
	buf[z] = gColor[0]; z++;
	buf[z] = gColor[1]; z++;
	buf[z] = gColor[2];
}
void Bit_Graphics::fillScreen() {
	size_t i = 0;
	for (size_t z = 0; z < ResZ; z++) {
		buf[i] = gColor[0]; i++;
		buf[i] = gColor[1]; i++;
		buf[i] = gColor[2]; i++;
	}
}
void Bit_Graphics::fillRect(size_t x0, size_t y0, size_t x1, size_t y1) { //x start, y start, x length, y length
	if (x0 >= ResX || y0 >= ResY || x1 == 0 || y1 == 0) { return; }
	if (x0 + x1 > ResX || y0 + y1 > ResY) { return; }
	size_t v = (y0 * ResX + x0);
	const size_t jump = ResX - x1;
	for (size_t dY = 0; dY < y1; dY++) {
		for (size_t dX = 0; dX < x1; dX++) {
			internal_plotFast(v);
			v++;
		}
		v += jump;
	}
}
void Bit_Graphics::horiz(size_t x0, size_t y, size_t x1) { //x start, y postion, x length
	if (x0 >= ResX || y >= ResY || x1 == 0) { return; }
	if (x0 + x1 > ResX) { return; }
	size_t fill = x0 + y * ResX;
	size_t z = fill + x1;
	for (; fill < z; fill++) {
		internal_plotFast(fill);
	}
}
void Bit_Graphics::vert(size_t x, size_t y0, size_t y1) { //x postion, y start, y length
	if (x >= ResX || y0 >= ResY || y1 == 0) { return; }
	if (y0 + y1 > ResY) { return; }
	size_t fill = x + y0 * ResX;
	size_t z = fill + y1 * ResX;
	for (; fill < z; fill += ResX) {
		internal_plotFast(fill);
	}
}
void Bit_Graphics::drawRect(size_t x0, size_t y0, size_t x1, size_t y1) { //x start, y start, x length, y length
	horiz(x0,y0,x1);
	horiz(x0,y0+y1-1,x1);
	vert(x0,y0,y1);
	vert(x0+x1-1,y0,y1);
}

void Bit_Graphics::drawLine0(int32_t x0, int32_t y0, int32_t x1, int32_t y1) {
	int32_t dX = x1 - x0;
	int32_t dY = y1 - y0;
	int32_t yI = 1;
	if (dY < 0) {
		yI = -1;
		dY = -dY;
	}
	int32_t dD = 2 * dY - dX;
	int32_t y = y0;
	for (int32_t x = x0; x < x1; x++) {
		plot(x,y);
		if (dD > 0) {
			y += yI;
			dD += 2 * (dY - dX);
		} else {	
			dD += 2 * dY;
		}
	}
}

void Bit_Graphics::drawLine1(int32_t x0, int32_t y0, int32_t x1, int32_t y1) {
	int32_t dX = x1 - x0;
	int32_t dY = y1 - y0;
    int32_t xI = 1;
    if (dX < 0) {
		xI = -1;
		dX = -dX;
	}
    int32_t dD = (2 * dX) - dY;
    int32_t x = x0;

    for (int32_t y = y0; y < y1; y++) {
        plot(x, y);
        if (dD > 0) {
            x = x + xI;
            dD = dD + (2 * (dX - dY));
        } else {
            dD = dD + 2 * dX;
        }
	}
}

void Bit_Graphics::drawLine(int32_t x0, int32_t y0, int32_t x1, int32_t y1) {
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
/* Text */
void Bit_Graphics::text6x8(size_t xW, size_t yW, char lexicon) {
	if (xW > ResX || yW > ResY) { return; }
	if (xW + 6 > ResX || yW + 8 > ResY) { return; }
	uint8_t* bitImage = (uint8_t*)char6x8 + ((size_t)lexicon * 6);
	size_t fill = (yW * ResX + xW);
	uint8_t b = 1;
	for (uint8_t y = 0; y < 8; y++) {
		for (uint8_t x = 0; x < 6; x++) {
			if (*bitImage & b) {
				internal_plotFast(fill);
			}
			bitImage++;
			fill++;
		}
		bitImage -= 6;
		fill += ResX - 6; // 320 - 6
		b <<= 1;
	}
}
void Bit_Graphics::printText6x8(size_t xW, size_t yW, char* text) {
	if (initialized == false) { return; }
	size_t x = xW;
	size_t y = yW;
	size_t t = 0; // For tabs
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
void Bit_Graphics::printTextWarp6x8(size_t xW, size_t yW, char* text, size_t width = 0) {
	if (initialized == false) { return; }
	if (width == 0) { width = ResX; }
	size_t x = xW;
	size_t y = yW;
	size_t t = 0; // For tabs
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
			if (xW + 7 > width) { // New Line
				x = xW;
				y += 9;
				t = 0;
			}
		}
		text++;
		t++;
	}
}
/*
void Bit_Graphics::debugPrintBuffer() {
	gColor_RGB(48,8,48);
	fillRect(12,12,29,10);
	gColor_RGB(216,255,216);
	if (buf == buf0) {
		printText6x8(13,13,"buf0");
	} else if (buf == buf1) {
		printText6x8(13,13,"buf1");
	} else {
		printText6x8(13,13,"buf?");
	}
}
*/
