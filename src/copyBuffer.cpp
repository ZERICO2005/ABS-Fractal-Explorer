/*
**	Author: zerico2005 (2023)
**	Project: Easy-GUI
**	License: MIT License
**	A copy of the MIT License should be included with
**	this project. If not, see https://opensource.org/license/MIT
*/

#include "Common_Def.h"
#include "copyBuffer.h"

void initBufferBox(BufferBox* box, uint8_t* vram, uint32_t resX, uint32_t resY, uint8_t channels, uint8_t padding) {
	if (box == NULL) {
		return;
	}
	box->vram = vram;
	box->resX = resX;
	box->channels = channels;
	box->resY = resY;
	box->padding = padding;
}

uint32_t getBufferBoxPitch(BufferBox* box) {
	if (box == NULL) {
		return 0;
	}
	uint32_t pitch = box->resX * (uint32_t)box->channels;
	if (box->padding == 0) {
		return pitch;
	}
	pitch += (pitch % box->padding == 0) ? 0 : (box->padding - (pitch % box->padding));
	return pitch;
}

size_t getBufferBoxSize(BufferBox* box) {
	if (box == NULL) {
		return 0;
	}
	uint32_t pitch = getBufferBoxPitch(box);
	return (size_t)(pitch * box->resY);
}

/* Internal use only, does not contain safety measures */
void blitBuffer(
	uint8_t* srcBuf, uint8_t* dstBuf,
	uint32_t srcResX, uint32_t dstResX, uint32_t channels,
	uint32_t sizeX, uint32_t sizeY,
	uint32_t srcX, uint32_t srcY,
	uint32_t dstX, uint32_t dstY,
	uint32_t srcPad, uint32_t dstPad
) {
	if (srcPad != 0) {
		srcPad = ( ((srcResX * channels) % srcPad == 0) ? 0 : (srcPad - ((srcResX * channels) % srcPad)) );
	}
	if (dstPad != 0) {
		dstPad = ( ((dstResX * channels) % dstPad == 0) ? 0 : (dstPad - ((dstResX * channels) % dstPad)) );
	}
	uint32_t sz = (srcY * srcResX + srcX) * channels + (srcY * srcPad);
	uint32_t dz = (dstY * dstResX + dstX) * channels + (dstY * dstPad);
	uint32_t srcJump = (srcResX - sizeX) * channels + srcPad;
	uint32_t dstJump = (dstResX - sizeX) * channels + dstPad;
	for (uint32_t y = 0; y < sizeY; y++) {
		for (uint32_t x = 0; x < sizeX * channels; x++) {
			dstBuf[dz] = srcBuf[sz]; dz++; sz++;
		}
		sz += srcJump;
		dz += dstJump;
	}
}

/* buf{src, dst}, src{cord, size}, dst{cord, size}, allowClipping | Copies a portion of Src buffer to a portion of Dst */
void copyBuffer(
	BufferBox bufSrc, BufferBox bufDst,
	uint32_t sx0, uint32_t sy0, uint32_t sx1, uint32_t sy1,
	int32_t dx0, int32_t dy0, uint32_t dx1, uint32_t dy1,
	bool allowClipping
) {
	//printf("\nsrc{%u,%u: %ux%u} dst{%d,%d: %ux%u}",sx0,sy0,sx1,sy1,dx0,dy0,dx1,dy1); fflush(stdout);
	/* Trival Errors (Null pointers and scalars with 0 magnitude) */
	if (bufSrc.vram == NULL) {
		//printf("\nError: bufScr.vram is NULL"); fflush(stdout);
		return;
	}
	if (bufDst.vram == NULL) {
		//printf("\nError: bufDst.vram is NULL"); fflush(stdout);
		return;
	}
	if (bufSrc.channels == 0 || bufDst.channels == 0) {
		//printf("\nError: Buffers cannot have 0 channels src(%u) dst(%u)",bufSrc.channels,bufDst.channels); fflush(stdout);
		return;
	} else if (bufSrc.channels != bufDst.channels) {
		//printf("\nError: Buffers have imcompatible bitwidths src(%u) dst(%u)",bufSrc.channels,bufDst.channels); fflush(stdout);
		return;
	}
	if (bufSrc.resX == 0 || bufSrc.resY == 0) { return; }
	if (bufDst.resX == 0 || bufDst.resY == 0) {  return; }
	if (sx1 == 0 || sy1 == 0) { return; }
	if (dx1 == 0 || dy1 == 0) { return; }
	/* Invalid Coordinates */
	if (sx0 >= bufSrc.resX || sy0 >= bufSrc.resY) { return; } // Positive OOB
	if (dx0 >= (int32_t)bufDst.resX || dy0 >= (int32_t)bufDst.resY) { return; } // Positive OOB
	if (allowClipping) { /* Corrects clipping values */
		if ((int32_t)(dx0 + sx0 + sx1) < 0 || (int32_t)(dy0 + sy0 + sy1) < 0) { return; } /* Negative OOB */
		/* Negative values */
		if (dx0 < 0) { sx0 += abs(dx0); sx1 -= abs(dx0); dx0 = 0; }
		if (dy0 < 0) { sy0 += abs(dy0); sy1 -= abs(dy0); dy0 = 0; }
		/* Src dimension */
		if (sx0 + sx1 > bufSrc.resX) { sx1 = bufSrc.resX - sx0; }
		if (sy0 + sy1 > bufSrc.resY) { sy1 = bufSrc.resY - sy0; }
		/* Dst dimension */
		if (dx0 + dx1 > bufSrc.resX) { dx1 = bufDst.resX - dx0; }
		if (dy0 + dy1 > bufSrc.resY) { dy1 = bufDst.resY - dy0; }
		/* Copy size */
		if (sx1 > dx1) { sx1 = dx1; }
		if (sy1 > dy1) { sy1 = dy1; }
	} else { 
		if (dx0 < 0 || dy0 < 0) { return; } /* Negative Dst OOB */
		if (sx0 + sx1 > bufSrc.resX || sy0 + sy1 > bufSrc.resY) { return; } /* Positive Src OOB */
		if (dx0 + dx1 > bufDst.resX || dy0 + dy1 > bufDst.resY) { return; } /* Positive Dst OOB */
		if (sx1 > dx1 || sy1 > dy1) { return; } /* Src is larger than Dst */
	}
	blitBuffer(
		bufSrc.vram,bufDst.vram,
		bufSrc.resX,bufDst.resX,(uint32_t)bufDst.channels,
		sx1,sy1,
		sx0,sy0,
		dx0,dy0,
		(uint32_t)bufSrc.padding,(uint32_t)bufDst.padding
	);
}

/* buf{src, dst}, dst{cord} allowClipping | Copies Src buffer to a location on Dst */
void copyBuffer(BufferBox bufSrc, BufferBox bufDst, int32_t x, int32_t y, bool allowClipping) {
	if ((int32_t)bufDst.resX - x < 0 || (int32_t)bufDst.resY - y < 0) {
		return;
	}
	copyBuffer(
		bufSrc,bufDst,
		0, 0, bufSrc.resX, bufSrc.resY,
		x, y, bufDst.resX - x, bufDst.resY - y,
		allowClipping
	);
}

/* buf{src, dst}, allowClipping | Copies Src buffer to Dst */
void copyBuffer(BufferBox bufSrc, BufferBox bufDst, bool allowClipping) {
	copyBuffer(
		bufSrc,bufDst,
		0, 0, bufSrc.resX, bufSrc.resY,
		0, 0, bufDst.resX, bufDst.resY,
		allowClipping
	);
}