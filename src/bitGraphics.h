/*
**	Author: zerico2005 (2023-2024)
**	Project: ABS-Fractal-Explorer
**	License: MIT License
**	A copy of the MIT License should be included with
**	this project. If not, see https://opensource.org/license/MIT
*/

#ifndef BITGRAPHICS_H
#define BITGRAPHICS_H
#include "Common_Def.h"
#include "copyBuffer.h"

class Bit_Graphics {
	public:
		Bit_Graphics();
		Bit_Graphics(size_t resX, size_t resY);
		bool isInitialized();
		void init_Bit_Graphics(size_t resX, size_t resY);
		void terminate_Bit_Graphics();
		void resizeBuffer(size_t resX, size_t resY);
		void swapBuffer();
		void clearBuffer(); // Writes 0's
		uint8_t* getDrawBuffer();
		uint8_t* getDisplayBuffer();
		void getDrawBufferBox(BufferBox* box);
		void getDisplayBufferBox(BufferBox* box);
		size_t getResX();
		size_t getResY();
		size_t getResZ();
		size_t getBufferSize();
	/* Set color */
		void gColor_RGB(uint8_t r, uint8_t g, uint8_t b);
		void gColor_Hex(uint32_t col);
		void gColor_HSV(fp64 h, fp64 s, fp64 v); // 0.0-360.0, 0.0-1.0, 0.0-1.0
		void gColor_HSV(fp32 h, fp32 s, fp32 v); // 0.0-360.0, 0.0-1.0, 0.0-1.0
	/* Primatives */
		void plot(size_t x, size_t y); //x position, y position
		void plotFast(size_t z); //z position
		void fillScreen();
		void fillRect(size_t x0, size_t y0, size_t x1, size_t y1); //x start, y start, x length, y length
		void horiz(size_t x0, size_t y, size_t x1); //x start, y postion, x length
		void vert(size_t x, size_t y0, size_t y1); //x postion, y start, y length
		void drawRect(size_t x0, size_t y0, size_t x1, size_t y1); //x start, y start, x length, y length
		void drawLine(int32_t x0, int32_t y0, int32_t x1, int32_t y1); // start cord, end cord
	/* Text */
		void printText6x8(size_t xW, size_t yW, char* text);
		// ResX is used if width is not set
		void printTextWarp6x8(size_t xW, size_t yW, char* text, size_t width);
		void text6x8(size_t xW, size_t yW, char lexicon);
		// void debugPrintBuffer();
	private:
		void internal_plot(size_t x, size_t y);
		void internal_plotFast(size_t z);
		void internal_horiz(size_t x0, size_t y, size_t x1);
		void drawLine0(int32_t x0, int32_t y0, int32_t x1, int32_t y1);
		void drawLine1(int32_t x0, int32_t y0, int32_t x1, int32_t y1);
		uint8_t* buf; // Current buffer
		uint8_t* buf0;
		uint8_t* buf1;
		size_t ResX;
		size_t ResY;
		size_t ResZ; // Total size
		uint8_t gColor[4];
		bool initialized;
};

#endif /* BITGRAPHICS_H */