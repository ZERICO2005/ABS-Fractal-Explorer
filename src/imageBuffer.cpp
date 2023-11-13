/*
**	Author: zerico2005 (2023)
**	Project: ABS-Fractal-Explorer
**	License: MIT License
**	A copy of the MIT License should be included with
**	this project. If not, see https://opensource.org/license/MIT
*/

#include "Common_Def.h"
#include "imageBuffer.h"
#include "copyBuffer.h"

/* ImageBuffer */
	// No Initalization
	ImageBuffer::ImageBuffer() {
		vram = nullptr;
		resX = 0;
		resY = 0;
		channels = 0;
		vramAllocated = false;
		bytesAllocated = 0;
	}
	// Minimal Initialization
	ImageBuffer::ImageBuffer(uint8_t Channels) {
		vram = nullptr;
		resX = 0;
		resY = 0;
		channels = Channels;
		vramAllocated = false;
		bytesAllocated = 0;
	}
	// Full Initialization
	ImageBuffer::ImageBuffer(uint32_t ResX, uint32_t ResY, uint8_t Channels) {
		reallocateBuffer(ResX, ResY, Channels);
	}

	// Preallocated Buffer
	ImageBuffer::ImageBuffer(uint8_t* Vram, uint32_t ResX, uint32_t ResY, uint8_t Channels) {
		vram = Vram;
		resX = ResX;
		resY = ResY;
		channels = Channels;
		if (vram == nullptr) {
			vramAllocated = false;
			bytesAllocated = 0;
		} else {
			vramAllocated = true;
			bytesAllocated = resX * resY * channels;
		}
	}
	
	// Copies the data from another ImageBuffer
	void ImageBuffer::vramCopy(ImageBuffer* buf, bool reallocBuf) {
		if (buf == nullptr || buf->vram == nullptr || buf->allocated() == false) {
			return;
		}
		if (reallocBuf == true) {
			reallocateBuffer(buf->resX,buf->resY,buf->channels);
		} else {
			resizeBuffer(buf->resX,buf->resY,buf->channels);
		}
		memcpy(vram,buf->vram,buf->getBufferSize());
		vramAllocated = true;
	}

	// Clears the buffer of data
	void ImageBuffer::clearBuffer() {
		if (vram == nullptr || vramAllocated == false) {
			return;
		}
		memset(vram,0,bytesAllocated);
	}
	void ImageBuffer::clearBuffer(uint8_t r, uint8_t g, uint8_t b) {
		if (vram == nullptr || vramAllocated == false || bytesAllocated < 3) {
			return;
		}
		vram[0] = r; vram[1] = g; vram[2] = b;
		for (size_t i = 3; i < bytesAllocated; i++) {
			vram[i] = vram[i - 3];
		}
	}
	void ImageBuffer::clearBuffer(uint8_t r, uint8_t g, uint8_t b, uint8_t a) {
		if (vram == nullptr || vramAllocated == false || bytesAllocated < 4) {
			return;
		}
		vram[0] = r; vram[1] = g; vram[2] = b; vram[3] = a;
		for (size_t i = 4; i < bytesAllocated; i++) {
			vram[i] = vram[i - 4];
		}
	}

	// Frees buffer if allocated
	void ImageBuffer::deleteBuffer() {
		if (vramAllocated == true) {
			FREE(vram);
			vramAllocated = false;
			bytesAllocated = 0;
		}
	}

	// Returns if buffer is allocated
	bool ImageBuffer::allocated() { return vramAllocated; }
	// Returns if the buffer is safe to write to
	bool ImageBuffer::bufferSafe() {
		if (vramAllocated == false) { return false; }
		if (bytesAllocated == 0) { return false; }
		if (vram == nullptr) { return false; }
		if (resX == 0) { return false; }
		if (resY == 0) { return false; }
		if (channels == 0) { return false; }
		if (bytesAllocated < resX * resY * channels) { return false; }
		return true;
	}
	
	// Returns the size of the buffer
	size_t ImageBuffer::getBufferSize() {
		return resX * resY * channels;
	}
	
	// Returns the current amount of bytes allocated
	size_t ImageBuffer::getAllocatedSize() {
		return bytesAllocated;
	}

	void ImageBuffer::reallocateBuffer(uint32_t ResX, uint32_t ResY, uint8_t Channels) {
		resX = ResX;
		resY = ResY;
		channels = Channels;
		bytesAllocated = resX * resY * channels;
		if (vramAllocated == true) {
			vram = (uint8_t*)realloc((void*)vram, bytesAllocated);
		} else {
			vram = (uint8_t*)malloc(bytesAllocated);
		}
		vramAllocated = true;
		if (vram == nullptr) {
			vramAllocated = false;
			bytesAllocated = 0;
		}
	}

	// Reallocates buffer only if more memory is needed
	void ImageBuffer::resizeBuffer(uint32_t ResX, uint32_t ResY, uint8_t Channels) {
		if (ResX * ResY * Channels > bytesAllocated) {
			reallocateBuffer(ResX, ResY, Channels);
			return;
		}
		resX = ResX;
		resY = ResY;
		channels = Channels;
	}

	// Reallocates buffer to current image size
	void ImageBuffer::trimBuffer() {
		if (resX * resY * channels < bytesAllocated) {
			reallocateBuffer(resX, resY, channels);
		}
	}

	void ImageBuffer::getBufferBox(BufferBox* buf) {
		buf->resX = resX;
		buf->resY = resY;
		buf->channels = channels;
		buf->padding = 0;
		buf->vram = vram;
	}

	// cord{x0,y0} cord{x1,y1}
	void ImageBuffer::setTransformationData(fp64 x0,fp64 y0,fp64 x1,fp64 y1) { 
		x00 = x0; y00 = y0;
		x11 = x1; y11 = y1;
		x01 = x0; y01 = y1;
		x10 = x1; y10 = y0;
	}
	// cord{x00,y00} cord{x11,y11} cord{x01,y01} cord{x10,y10}
	void ImageBuffer::setTransformationData(fp64 ix00,fp64 iy00,fp64 ix11,fp64 iy11,fp64 ix01,fp64 iy01,fp64 ix10,fp64 iy10) {
		x00 = ix00; y00 = iy00;
		x11 = ix11; y11 = iy11;
		x01 = ix01; y01 = iy01;
		x10 = ix10; y10 = iy10;
	}
	// Copies transformation data from another ImageBuffer*
	void ImageBuffer::setTransformationData(ImageBuffer* buf) {
		if (buf == nullptr) { return; }
		x00 = buf->x00; y00 = buf->y00;
		x11 = buf->x11; y11 = buf->y11;
		x01 = buf->x01; y01 = buf->y01;
		x10 = buf->x10; y10 = buf->y10;
		xCenter = buf->xCenter; yCenter = buf->yCenter;
		xSize = buf->xSize; ySize = buf->ySize;
		rot = buf->rot;
	}
	void ImageBuffer::printTransformationData(fp64 freq) {
		printfInterval(freq,
			"\ndeg(%.3lf) res{%ux%u} vram(%p)"
			"\n{%6.4lf,%.4lf} --- {%6.4lf,%6.4lf}"
			"\n{%6.4lf,%.4lf} --- {%6.4lf,%6.4lf}",
			rot * 360.0 / TAU,resX,resY,vram,
			x00,y00,x10,y10,
			x01,y01,x11,y11
		);
		//fflush(stdout);
	}

	void ImageBuffer::samplePixel(uint8_t* r,uint8_t* g,uint8_t* b,fp64 x, fp64 y) {
		
	}
/* ImageBuffer */