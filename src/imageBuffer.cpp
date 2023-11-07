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
		vram = NULL;
		resX = 0;
		resY = 0;
		channels = 0;
		subSample = 0;
		vramAllocated = false;
		bytesAllocated = 0;
	}
	// Minimal Initialization
	ImageBuffer::ImageBuffer(uint8_t Channels) {
		vram = NULL;
		resX = 0;
		resY = 0;
		channels = Channels;
		subSample = 1;
		vramAllocated = false;
		bytesAllocated = 0;
	}
	// Full Initialization
	ImageBuffer::ImageBuffer(uint32_t ResX, uint32_t ResY, uint8_t Channels) {
		reallocateBuffer(ResX, ResY, Channels);
		subSample = 1;
	}

	// Preallocated Buffer
	ImageBuffer::ImageBuffer(uint8_t* Vram, uint32_t ResX, uint32_t ResY, uint8_t Channels) {
		vram = Vram;
		resX = ResX;
		resY = ResY;
		channels = Channels;
		subSample = 1;
		if (vram == NULL) {
			vramAllocated = false;
			bytesAllocated = 0;
		} else {
			vramAllocated = true;
			bytesAllocated = resX * resY * channels;
		}
	}
	
	// Copies the data from another ImageBuffer
	void ImageBuffer::vramCopy(ImageBuffer* buf, bool reallocBuf) {
		if (buf == NULL || buf->vram == NULL || buf->allocated() == false) {
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
		if (vram == NULL) {
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
	void ImageBuffer::setTransformationData(fp64 x00,fp64 y00,fp64 x11,fp64 y11,fp64 x01,fp64 y01,fp64 x10,fp64 y10) {
		x00 = x00; y00 = y00;
		x11 = x11; y11 = y11;
		x01 = x01; y01 = y01;
		x10 = x10; y10 = y10;
	}
	// Copies transformation data from another ImageBuffer*
	void ImageBuffer::setTransformationData(ImageBuffer* buf) {
		if (buf == NULL) { return; }
		x00 = buf->x00; y00 = buf->y00;
		x11 = buf->x11; y11 = buf->y11;
		x01 = buf->x01; y01 = buf->y01;
		x10 = buf->x10; y10 = buf->y10;
		xCenter = buf->xCenter; yCenter = buf->yCenter;
		xSize = buf->xSize; ySize = buf->ySize;
		rot = buf->rot;
	}
	void ImageBuffer::printTransformationData(fp64 freq) {
		printfInterval(freq,"\ndeg(%.3lf) res{%ux%u} vram(%p)",rot * 360 / TAU,resX,resY,vram);
		printfInterval(freq,"\n{%.4lf,%.4lf} --- {%.4lf,%.4lf}",x00,y00,x10,y10);
		printfInterval(freq,"\n{%.4lf,%.4lf} --- {%.4lf,%.4lf}",x01,y01,x11,y11);
		//fflush(stdout);
	}

	void ImageBuffer::samplePixel(uint8_t* r,uint8_t* g,uint8_t* b,fp64 x, fp64 y) {
		
	}
/* ImageBuffer */