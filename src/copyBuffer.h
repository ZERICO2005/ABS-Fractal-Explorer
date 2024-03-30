/*
**	Author: zerico2005 (2023-2024)
**	Project: ABS-Fractal-Explorer
**	License: MIT License
**	A copy of the MIT License should be included with
**	this project. If not, see https://opensource.org/license/MIT
*/

#ifndef COPYBUFFER_H
#define COPYBUFFER_H
#include "Common_Def.h"

struct BufferBox {
	uint8_t* vram;
	uint32_t resX;
	uint32_t resY;
	uint8_t channels;
	uint8_t padding;
};

void initBufferBox(BufferBox* box, uint8_t* buf, uint32_t resX, uint32_t resY, uint8_t channels, uint8_t padding = 0);

uint32_t getBufferBoxPitch(const BufferBox* box);

size_t getBufferBoxSize(const BufferBox* box);

// Returns false in BufferBox should not be written to
bool validateBufferBox(const BufferBox* box);
// Prints Errors and returns false in BufferBox should not be written to
bool printValidateBufferBox(const BufferBox* box);

/* buf{src, dst}, src{cord, size}, dst{cord, size}, allowClipping | Copies a portion of Src buffer to a portion of Dst */
void copyBuffer(
	BufferBox bufSrc, BufferBox bufDst,
	uint32_t sx0, uint32_t sy0, uint32_t sx1, uint32_t sy1,
	int32_t dx0, int32_t dy0, uint32_t dx1, uint32_t dy1,
	bool allowClipping
);  

/* buf{src, dst}, dst{cord} allowClipping | Copies Src buffer to a location on Dst */
void copyBuffer(BufferBox bufSrc, BufferBox bufDst, int32_t x, int32_t y, bool allowClipping);

/* buf{src, dst}, allowClipping | Copies Src buffer to Dst */
void copyBuffer(BufferBox bufSrc, BufferBox bufDst, bool allowClipping);

// Copies all of src into dst + verticalOffset if the pitch of each BufferBox is the same
inline void copyBuffer_VeritcalOffset(
	const BufferBox& dst, const BufferBox& src,
	size_t verticalOffset
) {
	if (
		(src.vram == nullptr || dst.vram == nullptr) ||
		(verticalOffset >= (size_t)dst.resY)
	) {
		return;
	}
	size_t srcPitch = (size_t)src.resX * (size_t)src.channels;
	srcPitch += (src.padding != 0 && srcPitch % (size_t)src.padding != 0) ? ((size_t)src.padding - (srcPitch % (size_t)src.padding)) : 0;
	size_t dstPitch = (size_t)dst.resX * (size_t)dst.channels;
	dstPitch += (dst.padding != 0 && dstPitch % (size_t)dst.padding != 0) ? ((size_t)dst.padding - (dstPitch % (size_t)dst.padding)) : 0;
	if (srcPitch != dstPitch) { return; }

	size_t offset = (verticalOffset) * dstPitch;
	size_t copySize = ((size_t)dst.resY - verticalOffset) * dstPitch;
	if (copySize > src.resY * srcPitch) {
		copySize = src.resY * srcPitch;
	}
	memcpy(&dst.vram[offset], src.vram, copySize);
}

#endif /* COPYBUFFER_H */