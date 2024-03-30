/*
**	Author: zerico2005 (2024)
**	Project: 
**	License: MIT License
**	A copy of the MIT License should be included with
**	this project. If not, see https://opensource.org/license/MIT
*/

#ifndef BUFFER_COPY_HPP
#define BUFFER_COPY_HPP

#include <cstddef>
#include <cstdint>

struct Buffer_Data {
	uint32_t resX;
	uint32_t resY;
	size_t pixelSize;
	size_t pitch;
};

inline void set_Buffer_Data(
	Buffer_Data& data,
	const uint32_t& resX, const uint32_t& resY,
	const size_t pixelSize, const size_t& pitch
) {
	data.resX = resX;
	data.resY = resY;
	data.pixelSize = pixelSize;
	data.pitch = pitch;
}

/* buf{src, dst}, src{cord, size}, dst{cord, size}, allowClipping | Copies a portion of Src buffer to a portion of Dst */
void BufferCopy(
	uint8_t* __restrict__ dstBuf, const uint8_t* __restrict__ srcBuf,
	const Buffer_Data dstData, const Buffer_Data srcData,
	int32_t dstPosX, int32_t dstPosY, uint32_t dstLenX, uint32_t dstLenY,
	uint32_t srcPosX, uint32_t srcPosY, uint32_t srcLenX, uint32_t srcLenY,
	bool allowClipping = true
);  

/* buf{src, dst}, dst{cord} allowClipping | Copies Src buffer to a location on Dst */
void BufferCopy(
	uint8_t* __restrict__ dstBuf, const uint8_t* __restrict__ srcBuf,
	const Buffer_Data dstData, const Buffer_Data srcData,
	int32_t dstPosX, int32_t dstPosY,
	bool allowClipping = true
);

/* buf{src, dst}, allowClipping | Copies Src buffer to Dst */
void BufferCopy(
	uint8_t* __restrict__ dstBuf, const uint8_t* __restrict__ srcBuf,
	const Buffer_Data dstData, const Buffer_Data srcData,
	bool allowClipping = true
);

#endif /* BufferCopy_HPP */