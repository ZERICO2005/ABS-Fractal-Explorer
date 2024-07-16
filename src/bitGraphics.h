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
struct BufferBox;

class Bit_Graphics {
	public:
		Bit_Graphics();
		Bit_Graphics(size_t resX, size_t resY);
		bool isInitialized() const;
		void init_Bit_Graphics(size_t resX, size_t resY);
		void terminate_Bit_Graphics();
		void resizeBuffer(size_t resX, size_t resY);
		void swapBuffer();
		void clearBuffer(); // Writes 0's
		uint8_t* getDrawBuffer() const;
		uint8_t* getDisplayBuffer() const;
		void getDrawBufferBox(BufferBox* box);
		void getDisplayBufferBox(BufferBox* box);
		size_t getResX() const;
		size_t getResY() const;
		size_t getResZ() const;
		size_t getBufferSize() const;
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
		void printText6x8(size_t xW, size_t yW, const char* text);
		// ResX is used if width is not set
		void printTextWarp6x8(size_t xW, size_t yW, const char* text, size_t width);
		void text6x8(size_t xW, size_t yW, char lexicon);
		// void debugPrintBuffer();


		/* Turbo Functions (Less Portable) */
			inline void turbo_gColor_RGB(uint8_t r, uint8_t g, uint8_t b) {
				gColor[0] = r;
				gColor[1] = g;
				gColor[2] = b;
				gColor[3] = 0xFF;
			}
			inline void turbo_gColor_Hex(uint32_t col) {
				gColor[2] = col & 0xFF; col >>= 8;
				gColor[1] = col & 0xFF; col >>= 8;
				gColor[0] = col & 0xFF;
				gColor[3] = 0xFF;
			}
			inline void turbo_fillScreen() {
				#ifdef __AVX__
					constexpr size_t turbo_Multiple = 8;
				#else
					constexpr size_t turbo_Multiple = 4;
				#endif
				const size_t copyLen = (ResZ / turbo_Multiple) * turbo_Multiple;
				const size_t copyRemainder = ResZ % turbo_Multiple;
				const uint32_t color = *(uint32_t*)(void*)gColor;
				uint32_t* memPtr = (uint32_t*)(void*)buf;
				for (size_t dX = 0; dX < copyLen; dX++) {
					*memPtr = color;
					memPtr++;
				}
				for (size_t dX = 0; dX < copyRemainder; dX++) {
					*memPtr = color;
					memPtr++;
				}
			}
			inline void turbo_fillRect(size_t x0, size_t y0, size_t x1, size_t y1) { //x start, y start, x length, y length
				if (x0 >= ResX || y0 >= ResY || x1 == 0 || y1 == 0) { return; }
				if (x0 + x1 > ResX || y0 + y1 > ResY) { return; }
				
				constexpr size_t Bit_Graphics_Channels = 4;
				#ifdef __AVX__
					constexpr size_t turbo_Multiple = 8;
				#else
					constexpr size_t turbo_Multiple = 4;
				#endif
				uint8_t* fillPtr = &buf[(y0 * ResX + x0) * Bit_Graphics_Channels];
				const size_t jump = ResX * Bit_Graphics_Channels;
				const size_t copyLen = (x1 / turbo_Multiple) * turbo_Multiple;
				const size_t copyRemainder = x1 % turbo_Multiple;
				const uint32_t color = *(uint32_t*)(void*)gColor;
				for (size_t dY = 0; dY < y1; dY++) {
					uint32_t* memPtr = (uint32_t*)(void*)fillPtr;
					for (size_t dX = 0; dX < copyLen; dX++) {
						*memPtr = color;
						memPtr++;
					}
					for (size_t dX = 0; dX < copyRemainder; dX++) {
						*memPtr = color;
						memPtr++;
					}
					fillPtr += jump;
				}
			}
			inline void turbo_horiz(size_t x0, size_t y, size_t x1) { //x start, y postion, x length
				if (x0 >= ResX || y >= ResY || x1 == 0) { return; }
				if (x0 + x1 > ResX) { return; }
				
				constexpr size_t Bit_Graphics_Channels = 4;
				#ifdef __AVX__
					constexpr size_t turbo_Multiple = 8;
				#else
					constexpr size_t turbo_Multiple = 4;
				#endif
				const size_t copyLen = (x1 / turbo_Multiple) * turbo_Multiple;
				const size_t copyRemainder = x1 % turbo_Multiple;
				const uint32_t color = *(uint32_t*)(void*)gColor;
				uint32_t* memPtr = (uint32_t*)(void*)&buf[(y * ResX + x0) * Bit_Graphics_Channels];
				for (size_t dX = 0; dX < copyLen; dX++) {
					*memPtr = color;
					memPtr++;
				}
				for (size_t dX = 0; dX < copyRemainder; dX++) {
					*memPtr = color;
					memPtr++;
				}
			}
			inline void turbo_vert(size_t x, size_t y0, size_t y1) { //x postion, y start, y length
				if (x >= ResX || y0 >= ResY || y1 == 0) { return; }
				if (y0 + y1 > ResY) { return; }

				constexpr size_t Bit_Graphics_Channels = 4;
				uint8_t* fillPtr = &buf[(y0 * ResX + x) * Bit_Graphics_Channels];
				const size_t jump = ResX * Bit_Graphics_Channels;
				const uint32_t color = *(uint32_t*)(void*)gColor;
				for (size_t dY = 0; dY < y1; dY++) {
					uint32_t* memPtr = (uint32_t*)(void*)fillPtr;
					*memPtr = color;
					fillPtr += jump;
				}
			}

			//x start, y start, x length, y length
			inline void turbo_drawRect(size_t x0, size_t y0, size_t x1, size_t y1) { 
				turbo_horiz(x0,y0,x1);
				turbo_horiz(x0,y0+y1-1,x1);
				turbo_vert(x0,y0,y1);
				turbo_vert(x0+x1-1,y0,y1);
			}
	
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