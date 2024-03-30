/*
**	Author: zerico2005 (2024)
**	Project: 
**	License: MIT License
**	A copy of the MIT License should be included with
**	this project. If not, see https://opensource.org/license/MIT
*/

#include <cstddef>
#include <cstdint>
#include <cstring>
#include "BufferCopy.hpp"

/* buf{src, dst}, src{cord, size}, dst{cord, size}, allowClipping | Copies a portion of Src buffer to a portion of Dst */
void BufferCopy(
	uint8_t* __restrict__ dstBuf, const uint8_t* __restrict__ srcBuf,
	const Buffer_Data dstData, const Buffer_Data srcData,
	int32_t dstPosX, int32_t dstPosY, uint32_t dstLenX, uint32_t dstLenY,
	uint32_t srcPosX, uint32_t srcPosY, uint32_t srcLenX, uint32_t srcLenY,
	bool allowClipping
) {
	if (
		/* nullptr buffers */	
			(dstBuf == nullptr || srcBuf == nullptr) ||
		/* Different pixel sizes */
			(dstData.pixelSize != srcData.pixelSize) ||
		/* dstData parameters are 0 */
			(dstData.resX <= 0 || dstData.resY <= 0) || 
			(dstData.pixelSize == 0 || dstData.pitch == 0) ||
		/* srcData parameters are 0 */
			(srcData.resX <= 0 || srcData.resY <= 0) || 
			(srcData.pixelSize == 0 || srcData.pitch == 0) ||
		/* Clipping sizes are 0 */
			(dstLenX == 0 || dstLenY == 0) ||
			(srcLenX == 0 || srcLenY == 0) ||
		/* Invalid Coordinates (Positive OOB) */
			(dstPosX >= (int32_t)dstData.resX || dstPosY >= (int32_t)dstData.resY) ||
			(srcPosX >= srcData.resX || srcPosY >= srcData.resY)
	) {
		return;
	}
	if (allowClipping) { /* Corrects clipping values */
		if ( /* Negative OOB */
			(dstPosX + (int32_t)srcPosX + (int32_t)srcLenX < 0) ||
			(dstPosY + (int32_t)srcPosY + (int32_t)srcLenY < 0)
		) {
			return;
		} 
		/* Negative values */
		if (dstPosX < 0) {
			srcPosX += (uint32_t)(-dstPosX);
			srcLenX -= (uint32_t)(-dstPosX);
			dstPosX = 0;
		}
		if (dstPosY < 0) {
			srcPosY += (uint32_t)(-dstPosY);
			srcLenY -= (uint32_t)(-dstPosY);
			dstPosY = 0;
		}
		/* Src dimension */
		if (srcPosX + srcLenX > srcData.resX) {
			srcLenX = srcData.resX - srcPosX;
		}
		if (srcPosY + srcLenY > srcData.resY) {
			srcLenY = srcData.resY - srcPosY;
		}
		/* Dst dimension */
		if ((uint32_t)dstPosX + dstLenX > srcData.resX) {
			dstLenX = dstData.resX - (uint32_t)dstPosX;
		}
		if ((uint32_t)dstPosY + dstLenY > srcData.resY) {
			dstLenY = dstData.resY - (uint32_t)dstPosY;
		}
		/* Copy size */
		srcLenX = (srcLenX > dstLenX) ? dstLenX : srcLenX;
		srcLenY = (srcLenY > dstLenY) ? dstLenY : srcLenY;
	} else if (
		/* Negative Dst OOB */
		(dstPosX < 0 || dstPosY < 0) ||
		/* Positive Src OOB */
		(srcPosX + srcLenX > srcData.resX) ||
		(srcPosY + srcLenY > srcData.resY) ||
		/* Positive Dst OOB */
		((uint32_t)dstPosX + dstLenX > dstData.resX) ||
		((uint32_t)dstPosY + dstLenY > dstData.resY) ||
		/* Src is larger than Dst */
		(srcLenX > dstLenX || srcLenY > dstLenY)
	) {
		return;
	}

	size_t srcOffset = ((size_t)srcPosY * srcData.pitch) + ((size_t)srcPosX * srcData.pixelSize);
	size_t dstOffset = ((size_t)dstPosY * dstData.pitch) + ((size_t)dstPosX * dstData.pixelSize);
	size_t copySize = ((size_t)srcLenX * srcData.pixelSize);
	for (uint32_t y = 0; y < srcLenY; y++) {
		memcpy(&dstBuf[dstOffset], &srcBuf[srcOffset], copySize);
		dstOffset += dstData.pitch;
		srcOffset += srcData.pitch;
	}
}

/* buf{src, dst}, dst{cord} allowClipping | Copies Src buffer to a location on Dst */
void BufferCopy(
	uint8_t* __restrict__ dstBuf, const uint8_t* __restrict__ srcBuf,
	const Buffer_Data dstData, const Buffer_Data srcData, 
	int32_t dstPosX, int32_t dstPosY,
	bool allowClipping
) {
	BufferCopy(
		dstBuf, srcBuf,
		dstData, srcData,
		dstPosX, dstPosY, dstData.resX, dstData.resY,
		0, 0, srcData.resX, srcData.resY,
		allowClipping
	);
}

/* buf{src, dst}, allowClipping | Copies Src buffer to Dst */
void BufferCopy(
	uint8_t* __restrict__ dstBuf, const uint8_t* __restrict__ srcBuf,
	const Buffer_Data dstData, const Buffer_Data srcData, 
	bool allowClipping
) {
	BufferCopy(
		dstBuf, srcBuf,
		dstData, srcData,
		0, 0, dstData.resX, dstData.resY,
		0, 0, srcData.resX, srcData.resY,
		allowClipping
	);
}