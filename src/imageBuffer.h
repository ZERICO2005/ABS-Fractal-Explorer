/*
**	Author: zerico2005 (2023)
**	Project: ABS-Fractal-Explorer
**	License: MIT License
**	A copy of the MIT License should be included with
**	this project. If not, see https://opensource.org/license/MIT
*/

#ifndef IMAGEBUFFER_H
#define IMAGEBUFFER_H

#include "Common_Def.h"
#include "copyBuffer.h"

class ImageBuffer {
	public:
		uint8_t* vram;
		uint32_t resX;
		uint32_t resY;
		uint8_t channels;
		uint32_t subSample;
		// No Initialization
		ImageBuffer();
		// Minimal Initialization
		ImageBuffer(uint8_t Channels);
		// Full Initialization
		ImageBuffer(uint32_t ResX, uint32_t ResY, uint8_t Channels);
		// Preallocated Buffer
		ImageBuffer(uint8_t* Vram, uint32_t ResX, uint32_t ResY, uint8_t Channels);
		void deleteBuffer();
		bool allocated(); // Returns if buffer is allocated
		void reallocateBuffer(uint32_t ResX, uint32_t ResY, uint8_t Channels);
		void resizeBuffer(uint32_t ResX, uint32_t ResY, uint8_t Channels); // Reallocates buffer only if more memory is needed
		void trimBuffer(); // Reallocates buffer to current image size
		void getBufferBox(BufferBox* buf);
		void vramCopy(ImageBuffer* buf, bool reallocBuf = false);
		void clearBuffer();
		void clearBuffer(uint8_t r, uint8_t g, uint8_t b);
		void clearBuffer(uint8_t r, uint8_t g, uint8_t b, uint8_t a);
		size_t getBufferSize();
		size_t getAllocatedSize();
		// Transformation
		fp64 x00; fp64 y00;
		fp64 x11; fp64 y11;
		fp64 x01; fp64 y01;
		fp64 x10; fp64 y10;
		fp64 xCenter; fp64 yCenter;
		fp64 xSize; fp64 ySize;
		fp64 rot;
		void setTransformationData(fp64 x0,fp64 y0,fp64 x1,fp64 y1); // cord{x0,y0} cord{x1,y1}
		void setTransformationData(fp64 x00,fp64 y00,fp64 x11,fp64 y11,fp64 x01,fp64 y01,fp64 x10,fp64 y10); // cord{x00,y00} cord{x11,y11} cord{x01,y01} cord{x10,y10}
		void setTransformationData(ImageBuffer* buf);
		void printTransformationData(fp64 freq = 0.6);
		void samplePixel(uint8_t* r,uint8_t* g,uint8_t* b,fp64 x, fp64 y);
	private:
		bool vramAllocated;
		size_t bytesAllocated;
};

#endif /* IMAGEBUFFER_H */