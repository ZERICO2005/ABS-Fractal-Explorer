/*
**	Author: zerico2005 (2023)
**	Project: ABS-Fractal-Explorer
**	License: MIT License
**	A copy of the MIT License should be included with
**	this project. If not, see https://opensource.org/license/MIT
*/

#ifndef COPYBUFFER_H
#define COPYBUFFER_H
#include "Common_Def.h"

struct _BufferBox {
	uint8_t* vram;
	uint32_t resX;
	uint32_t resY;
	uint8_t channels;
	uint8_t padding;
}; typedef struct _BufferBox BufferBox;

void initBufferBox(BufferBox* box, uint8_t* buf, uint32_t resX, uint32_t resY, uint8_t channels, uint8_t padding = 0);

uint32_t getBufferBoxPitch(BufferBox* box);

size_t getBufferBoxSize(BufferBox* box);

// Returns false in BufferBox should not be written to
bool validateBufferBox(BufferBox* box);
// Prints Errors and returns false in BufferBox should not be written to
bool printValidateBufferBox(BufferBox* box);

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

#endif /* COPYBUFFER_H */