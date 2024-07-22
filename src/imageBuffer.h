/*
**	Author: zerico2005 (2023-2024)
**	Project: ABS-Fractal-Explorer
**	License: MIT License
**	A copy of the MIT License should be included with
**	this project. If not, see https://opensource.org/license/MIT
*/

#ifndef IMAGEBUFFER_H
#define IMAGEBUFFER_H

#include "Common_Def.h"
#include "copyBuffer.h"

#include "Program_Def.h"

class ImageBuffer {
	public:
		uint8_t* vram;
		dim32_t resX;
		dim32_t resY;
		size_t channels;
		/* Constructors */
			// No Initialization
			ImageBuffer();
			// Minimal Initialization
			ImageBuffer(size_t Channels);
			// Full Initialization
			ImageBuffer(dim32_t ResX, dim32_t ResY, size_t Channels);
			// Preallocated Buffer
			ImageBuffer(uint8_t* Vram, dim32_t ResX, dim32_t ResY, size_t Channels);
			/* Destructors */
			void deleteBuffer();

		
		bool allocated() const; // Returns if buffer is allocated
		bool bufferSafe() const; // Returns if the buffer is safe to write to
		
		void reallocateBuffer(dim32_t ResX, dim32_t ResY, size_t Channels);
		
		// Reallocates buffer only if more memory is needed
		void resizeBuffer(dim32_t ResX, dim32_t ResY, size_t Channels);

		void trimBuffer(); // Reallocates buffer to current image size
		void getBufferBox(BufferBox* buf) const;
		void vramCopy(const ImageBuffer* buf, bool reallocBuf = false);
		void clearBuffer();
		void clearBuffer(uint8_t r, uint8_t g, uint8_t b);
		void clearBuffer(uint8_t r, uint8_t g, uint8_t b, uint8_t a);
		// Returns the size of the buffer
		size_t getBufferSize() const;
		size_t getAllocatedSize() const;
		
		// Transformation
		fpCord x00; fpCord y00;
		fpCord x11; fpCord y11;
		fpCord x01; fpCord y01;
		fpCord x10; fpCord y10;
		fpCord realCord;
		fpCord imagCord;
		fp64 zoom;
		fp64 rot;
		fp64 stretchX; fp64 stretchY;

		// cord{x0,y0} cord{x1,y1}
		void setTransformationData(
			fpCord x0, fpCord y0, fpCord x1,fpCord y1
		);
		// cord{x00,y00} cord{x11,y11} cord{x01,y01} cord{x10,y10}
		void setTransformationData(
			fpCord x00, fpCord y00, fpCord x11, fpCord y11,
			fpCord x01, fpCord y01, fpCord x10, fpCord y10
		);
		void setFractalState(fpCord RealCord, fpCord ImagCord, fp64 Zoom, fp64 Rot, fp64 StretchX, fp64 StretchY);
		
		void setTransformationData(const ImageBuffer* buf);
		void printTransformationData(fp64 freq = 0.6);
		
		// Going from my experience coding frame_Transformation.cpp, this function would be extremely slow
		// void samplePixel(uint8_t* r,uint8_t* g,uint8_t* b,fp64 x, fp64 y);
	private:
		bool vramAllocated;
		size_t bytesAllocated;
};

#endif /* IMAGEBUFFER_H */