/*
**	Author: zerico2005 (2023)
**	Project: ABS-Fractal-Explorer
**	License: MIT License
**	A copy of the MIT License should be included with
**	this project. If not, see https://opensource.org/license/MIT
*/

#ifndef GRAPHICS_BUFFER_H
#define GRAPHICS_BUFFER_H

#include "Common_Def.h"

/* To be used in with Graphics_Buffer to get/set parameters */
struct Graphics_Box {
	uint8_t* buf;
	size_t resX;
	size_t resY;
	uint8_t channels;
	size_t pitch;
	uint8_t padding;
	size_t bytesAllocated;
	size_t bufferIndex;
}; typedef struct Graphics_Box Graphics_Box;

/* Handles graphics and dynamic image buffer sizes */
class Graphics_Buffer {
	public:
		/* Constructors */
			Graphics_Buffer();
			Graphics_Buffer(const Graphics_Box* box, size_t BufferCount);
			Graphics_Buffer(size_t ResX, size_t ResY, uint8_t Channels, size_t BufferCount);
			Graphics_Buffer(size_t ResX, size_t ResY, uint8_t Channels, size_t Pitch, uint8_t Padding, size_t BufferCount);
			~Graphics_Buffer();
		/* Initializers */
			int initialize(const Graphics_Box* box, size_t BufferCount);
			int initialize(size_t ResX, size_t ResY, uint8_t Channels, size_t BufferCount);
			int initialize(size_t ResX, size_t ResY, uint8_t Channels, size_t Pitch, uint8_t Padding, size_t BufferCount);
			int terminate();
		/* Reallocation */
			int setBufferCount(size_t BufferCount);
			int resize(const Graphics_Box* box);
			int resize(size_t ResX, size_t ResY);
			int resize(size_t ResX, size_t ResY, uint8_t Channels);
			int resize(size_t ResX, size_t ResY, uint8_t Channels, size_t Pitch, uint8_t Padding);
			int reallocate(const Graphics_Box* box);
			int reallocate(size_t ResX, size_t ResY);
			int reallocate(size_t ResX, size_t ResY, uint8_t Channels);
			int reallocate(size_t ResX, size_t ResY, uint8_t Channels, size_t Pitch, uint8_t Padding);
			int trim();
		/* Buffers */
			bool getGraphicsBox(Graphics_Box* box);
		/* Clear Screen */
			void clearAllBuffers();
			void clearBuffer();
			void clearBuffer(uint8_t byte);
			void clearBuffer(uint8_t luminance, uint8_t alpha = 0xFF);
			void clearBuffer(uint8_t red, uint8_t green, uint8_t blue, uint8_t alpha = 0xFF);
			void clearBuffer(uint32_t color);
			void clearBuffer(fp32 hue, fp32 saturation, fp32 value, fp32 alpha = 1.0f);
		/* Graphics */
			/* Graphics Color */
				void setColor(uint8_t luminance, uint8_t alpha = 0xFF);
				void setColor(uint8_t red, uint8_t green, uint8_t blue, uint8_t alpha = 0xFF);
				void setColor(uint32_t color);
				void setColor(fp32 hue, fp32 saturation, fp32 value, fp32 alpha = 1.0f);
			/* Primatives */
				void plotFast(size_t z);
				void plot(int32_t x, int32_t y);
				void horiz(int32_t x0, int32_t y, int32_t x1);
				void vert(int32_t x, int32_t y0, int32_t y1);
				void fillRect(int32_t x0, int32_t y0, int32_t x1, int32_t y1);
				void drawRect(int32_t x0, int32_t y0, int32_t x1, int32_t y1);
				void drawLine(int32_t x0, int32_t y0, int32_t x1, int32_t y1);
				void fillEllipse(int32_t x0, int32_t y0, int32_t x1, int32_t y1);
				void drawEllipse(int32_t x0, int32_t y0, int32_t x1, int32_t y1);
				void drawRect(int32_t x0, int32_t y0, int32_t x1, int32_t y1);
				void drawLine(int32_t x0, int32_t y0, int32_t x1, int32_t y1);
				void fillEllipse(int32_t x0, int32_t y0, int32_t x1, int32_t y1);
				void drawEllipse(int32_t x0, int32_t y0, int32_t x1, int32_t y1);
			/* Text */
				int printText6x8(int32_t x, int32_t y, bool warp = true, const char* text, size_t length);
				int printfText6x8(int32_t x, int32_t y, bool warp = true, const char* fm, ...);
	private:
		bool initialized;
		uint8_t gColor[4];
		Graphics_Box box;
		void drawLine0(int32_t x0, int32_t y0, int32_t x1, int32_t y1);
		void drawLine1(int32_t x0, int32_t y0, int32_t x1, int32_t y1);
};


#endif /* GRAPHICS_BUFFER_H */